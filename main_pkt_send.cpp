#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>


#include <rte_common.h>
#include <rte_vect.h>
#include <rte_byteorder.h>
#include <rte_log.h>
#include <rte_malloc.h>
#include <rte_memory.h>
#include <rte_memcpy.h>
#include <rte_eal.h>
#include <rte_launch.h>
#include <rte_atomic.h>
#include <rte_cycles.h>
#include <rte_prefetch.h>
#include <rte_lcore.h>
#include <rte_per_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_interrupts.h>
#include <rte_random.h>
#include <rte_debug.h>
#include <rte_ether.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>
#include <rte_ip.h>
#include <rte_tcp.h>
#include <rte_udp.h>
#include <rte_string_fns.h>
#include <rte_cpuflags.h>
#include <stdint.h>
#include <inttypes.h>
#include <rte_ethdev.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>

#include <iostream>
#include <vector>
#include <memory>
#include <random>
#include <thread>
#include <openssl/sha.h>
#include "packet_utils.h"


volatile bool force_quit = false;  // 标志变量,用于指示是否强制退出

void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("Caught signal %d, exiting...\n", sig);
        force_quit = true;  // 设置标志变量为 true,表示需要强制退出
    }
}


struct rte_mempool *mbuf_pool;
uint64_t total_bytes[MAX_CORES] = {0};  // 每个内核接收到的字节数

uint32_t num_rx_queues;
uint32_t num_tx_queues;
uint32_t num_cores;


uint32_t total_num[MAX_CORES] = {};
uint32_t burst_num[MAX_CORES] = {};
uint32_t send_offset[MAX_CORES] = {};

static const struct rte_eth_conf port_conf_default = {
        .rxmode = {
                .max_lro_pkt_size = RTE_ETHER_MAX_LEN,
        },
};

static inline int init_port(uint16_t port, struct rte_mempool *mbuf_pool, uint32_t rx_queues, uint32_t tx_queues)
{
    struct rte_eth_conf port_conf = port_conf_default;
//    const uint16_t rx_rings = rx_queues, tx_rings = tx_queues; // 接收队列和发送队列数量
    uint16_t nb_rxd = RX_RING_SIZE;
    uint16_t nb_txd = TX_RING_SIZE;
    int retval;
    uint16_t q;
    struct rte_eth_dev_info dev_info;
    struct rte_eth_txconf txconf;

    if (!rte_eth_dev_is_valid_port(port))
        return -1;

    retval = rte_eth_dev_info_get(port, &dev_info);
    if (retval != 0)
    {
        printf("Error during getting device (port %u) info: %s\n",
               port, strerror(-retval));
        return retval;
    }

    const uint16_t rx_rings = std::min((uint16_t)rx_queues, dev_info.max_rx_queues);
    const uint16_t tx_rings = std::min((uint16_t)tx_queues, dev_info.max_tx_queues);

    if (dev_info.tx_offload_capa & RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE)
        port_conf.txmode.offloads |=
                RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE;

    /* Configure the Ethernet device. */
    retval = rte_eth_dev_configure(port, rx_rings, tx_rings, &port_conf);
    if (retval != 0)
        return retval;

    retval = rte_eth_dev_adjust_nb_rx_tx_desc(port, &nb_rxd, &nb_txd);
    if (retval != 0)
        return retval;

    /* Allocate and set up 1 RX queue per Ethernet port. */
    for (q = 0; q < rx_rings; q++)
    {
        retval = rte_eth_rx_queue_setup(port, q, nb_rxd,
                                        rte_eth_dev_socket_id(port), NULL, mbuf_pool);
        if (retval < 0)
            return retval;
    }

    txconf = dev_info.default_txconf;
    txconf.offloads = port_conf.txmode.offloads;
    /* Allocate and set up 1 TX queue per Ethernet port. */
    for (q = 0; q < tx_rings; q++)
    {
        retval = rte_eth_tx_queue_setup(port, q, nb_txd,
                                        rte_eth_dev_socket_id(port), &txconf);
        if (retval < 0)
            return retval;
    }

    /* Start the Ethernet port. */
    retval = rte_eth_dev_start(port);
    if (retval < 0)
        return retval;

    /* Display the port MAC address. */
    struct rte_ether_addr addr;
    retval = rte_eth_macaddr_get(port, &addr);
    if (retval != 0)
        return retval;

    printf("Port %u MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8
           " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "\n",
           port,
           addr.addr_bytes[0], addr.addr_bytes[1],
           addr.addr_bytes[2], addr.addr_bytes[3],
           addr.addr_bytes[4], addr.addr_bytes[5]);

    /* Enable RX in promiscuous mode for the Ethernet device. */
    retval = rte_eth_promiscuous_enable(port);
    if (retval != 0)
        return retval;

    return 0;
}

static int packet_send_process(void *arg){
    uint32_t core_id = rte_lcore_id();
    uint32_t queue_id;
    struct rte_mbuf *bufs[BURST_SIZE];

    auto config = new struct send_config;
    config = (struct send_config *)arg;
    queue_id = (config->core_queues)[core_id];

    std::cout << "core_id: " << core_id << " queue id: " << queue_id << std::endl;

    uint32_t send_index = 0, update_times = 0;
    uint32_t pre_core_works = (ARRAY_NUM * ARRAY_SIZE) / num_cores;
    uint32_t offset = pre_core_works * core_id;

    // 在循环之前预先分配内存
    for (int i = 0; i < BURST_SIZE; i++) {
        bufs[i] = rte_pktmbuf_alloc(mbuf_pool);
        if (bufs[i] == NULL) {
            printf("rte_pktmbuf_alloc failed\n");
            // 处理内存分配失败的情况
            return -1;
        }
    }

    struct rte_ether_addr src_addr;
    rte_eth_macaddr_get(config->port_id, &src_addr);
    uint16_t ether_type = rte_cpu_to_be_16(RTE_ETHER_TYPE_IPV4);

    while (!force_quit){

        if(send_index > pre_core_works){
            send_index = send_index % pre_core_works;
            update_times++;
//            std::cout << "core: " << core_id << " update times: " << update_times << std::endl;
        }

        for (int i = 0; i < BURST_SIZE; i++){
//            bufs[i] = rte_pktmbuf_alloc(mbuf_pool);
//            if (bufs[i] == NULL){
//                printf("rte_pktmbuf_alloc failed\n");
//                continue;
//            }
            struct my_pkt2 *pkt = rte_pktmbuf_mtod(bufs[i], struct my_pkt2*);

            // 填充以太网报头
            pkt->eth_hdr.src_addr = src_addr;
            pkt->eth_hdr.dst_addr = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06}; // 设置目的 MAC 地址
            pkt->eth_hdr.ether_type = ether_type;


            // 填充 IP 报头
            pkt->ip_hdr.version_ihl = RTE_IPV4_VHL_DEF;
            pkt->ip_hdr.type_of_service = 0;
            pkt->ip_hdr.total_length = rte_cpu_to_be_16(sizeof(my_pkt2) - sizeof(struct rte_ether_hdr));
            pkt->ip_hdr.packet_id = 0;
            pkt->ip_hdr.fragment_offset = 0;
            pkt->ip_hdr.time_to_live = 64;
            pkt->ip_hdr.next_proto_id = IPPROTO_UDP;
            pkt->ip_hdr.src_addr = rte_cpu_to_be_32(RTE_IPV4(192, 168, core_id+1, send_index % 254 + 1)); // 根据 core_id 和 send_index 设置源 IP
            pkt->ip_hdr.dst_addr = rte_cpu_to_be_32(RTE_IPV4(192, 168, 1, 2)); // 设置目的 IP 地址

            // 填充 UDP 报头
            pkt->udp_hdr.src_port = rte_cpu_to_be_16(1234 + queue_id); // 根据 queue_id 设置源端口
            pkt->udp_hdr.dst_port = rte_cpu_to_be_16(5678);
            uint16_t udp_len = sizeof(my_pkt2) - sizeof(struct rte_ether_hdr) - sizeof(struct rte_ipv4_hdr);
            pkt->udp_hdr.dgram_len = rte_cpu_to_be_16(udp_len);
            pkt->udp_hdr.dgram_cksum = 0;

            //fill kv
            pkt->idx = send_index + offset;
            pkt->value = *(&array[0][0] + pkt->idx);

            // 填充payload
            memset(pkt->payload, 0xFF, sizeof(pkt->payload));


            send_index++;
            bufs[i]->pkt_len = bufs[i]->data_len = sizeof(my_pkt2);
//            total_bytes[core_id] += bufs[i]->pkt_len;

        }
        uint16_t nb_tx = rte_eth_tx_burst(config->port_id, queue_id, bufs, BURST_SIZE);
        if (nb_tx < BURST_SIZE){
            for (int i = nb_tx; i < BURST_SIZE; i++){
                rte_pktmbuf_free(bufs[i]);
            }
        }
        total_num[core_id] += nb_tx;
        burst_num[core_id]++;
        total_bytes[core_id] += sizeof(my_pkt2) * nb_tx;
//        std::cout << "total send num: " << total_num[core_id] << " from core: " << core_id << std::endl;
    }

    printf("Core %d send %lu bytes\n", core_id, total_bytes[core_id]);

    return 0;
}

void simulate_recv(){
    while (true){
        update_flow();
    }
}

void print_max(){
    while (true){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        uint32_t max_num = 0;
        for(int i = 0; i < ARRAY_NUM; ++i){
            for(int j = 0; j < ARRAY_SIZE; ++j){
                if (array[i][j] > max_num){
                    max_num = array[i][j];
                }
            }
            std::cout << "array: " << i << " max num: " << max_num << std::endl;
        }

    }
}

int main(int argc, char *argv[])
{
    int ret;
    struct send_config config = {};

    // 模拟收包
    std::thread recv_thread(simulate_recv);
//    std::thread print_thread(print_max);

    /* Initialize EAL */
    ret = rte_eal_init(argc, argv);
    if (ret < 0)
    {
        rte_exit(EXIT_FAILURE, "Failed to initialize EAL.\n");
    }

    argc -= ret;
    argv += ret;

    /* Parse application arguments */
    if (argc < 1)
    {
        rte_exit(EXIT_FAILURE, "Invalid number of arguments.\n");
    }

    uint16_t nb_sys_ports = rte_eth_dev_count_avail();
    if (nb_sys_ports < 1)
    {
        rte_exit(EXIT_FAILURE, "No Supported eth found\n");
        return -1;
    }

    // 内存池初始化，发送和接收的数据都在内存池里
     mbuf_pool = rte_pktmbuf_pool_create(
            "mbuf pool", NUM_MBUFS, MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());

    if (mbuf_pool == NULL)
    {
        rte_exit(EXIT_FAILURE, "Could not create mbuf pool\n");
    }

    uint32_t used_cores = rte_lcore_count();
    std::cout << "used cores: " << used_cores << std::endl;

    num_cores = used_cores;
    num_rx_queues = used_cores - 1;
    num_tx_queues = used_cores - 1;

    // 启动dpdk
    uint16_t portid = 0;
    ret = init_port(portid, mbuf_pool, num_rx_queues, num_tx_queues);
    if (ret != 0)
    {
        printf("port init error!\n");
    }

    for(int i = 1; i <= num_tx_queues; i++){
         config.core_queues[i] = i - 1;
    }

    config.port_id = portid;

    signal(SIGINT, signal_handler);
    auto start = std::chrono::high_resolution_clock::now();
    rte_eal_mp_remote_launch(packet_send_process, &config, SKIP_MAIN);

    std::cout << "start to send data" << std::endl;

    // 等待,直到 force_quit 变为 true
    while (!force_quit) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    double total_time_s = elapsed.count();

    std::cout << "total time: " << total_time_s << " s" << std::endl;


    uint64_t total_bytes_all = 0;
    for (int i = 0; i < MAX_CORES; i++) {
        total_bytes_all += total_bytes[i];  // 计算所有内核接收到的总字节数
    }

//    double throughput_gbps = (double)total_bytes_all * 8 / total_time_s / 1000000000.0;
    double throughput_gbps = (double)total_bytes_all / total_time_s * 8 / 1000000000.0;

    printf("Total Throughput: %.2f Gbps\n", throughput_gbps);

    uint32_t lcore_id;
    RTE_LCORE_FOREACH(lcore_id){
        if (rte_eal_wait_lcore(lcore_id) < 0){
            return -1;
        }
    }
    return 0;

}
