//
// Created by SunX on 2024/4/11.
//
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
// #include <time>

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

#define RX_RING_SIZE 1024
#define TX_RING_SIZE 1024

#define NUM_MBUFS 8191
#define MBUF_CACHE_SIZE 250
#define BURST_SIZE 32

#define _WRS_PACK_ALIGN(x) __attribute__((packed, aligned(x)))

#include <stdint.h>

#define MAX_PRIME32 1229

static inline uint32_t rotl32(uint32_t x, int8_t r)
{
    return (x << r) | (x >> (32 - r));
}

static inline uint64_t rotl64(uint64_t x, int8_t r)
{
    return (x << r) | (x >> (64 - r));
}

#define ROTL32(x, y) rotl32(x, y)
#define ROTL64(x, y) rotl64(x, y)

#define getblock(p, i) (p[i])

static inline uint32_t fmix32(uint32_t h)
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;

    return h;
}

//-----------------------------------------------------------------------------

uint32_t murmur3(const void *key, int len, uint32_t seed)
{
    const uint8_t *data = (const uint8_t *)key;
    const int nblocks = len / 4;
    int i;

    uint32_t h1 = seed;

    uint32_t c1 = 0xcc9e2d51;
    uint32_t c2 = 0x1b873593;

    //----------
    // body

    const uint32_t *blocks = (const uint32_t *)(data + nblocks * 4);

    for (i = -nblocks; i; i++)
    {
        uint32_t k1 = getblock(blocks, i);

        k1 *= c1;
        k1 = ROTL32(k1, 15);
        k1 *= c2;

        h1 ^= k1;
        h1 = ROTL32(h1, 13);
        h1 = h1 * 5 + 0xe6546b64;
    }

    //----------
    // tail

    const uint8_t *tail = (const uint8_t *)(data + nblocks * 4);

    uint32_t k1 = 0;

    switch (len & 3)
    {
        case 3:
            k1 ^= tail[2] << 16;
        case 2:
            k1 ^= tail[1] << 8;
        case 1:
            k1 ^= tail[0];
            k1 *= c1;
            k1 = ROTL32(k1, 15);
            k1 *= c2;
            h1 ^= k1;
    };

    //----------
    // finalization

    h1 ^= len;

    h1 = fmix32(h1);

    return h1;
}

#define ct_k 4
#define w_ct 2000000
#define w_bf 16000000
#define bf_k 4

//_Static_assert((COLUMNS & (COLUMNS - 1)) == 0, "COLUMNS must be a power of two");

struct bloomfilter
{
    uint32_t values[w_bf];
    uint32_t k;
};

struct FRBucket
{
    uint64_t FlowXOR;
    uint32_t FlowCount;
    uint32_t PacketCount;
};

struct countingtable
{
    struct FRBucket bucket[w_ct];
};
typedef struct
{
    uint32_t sip;
    uint32_t dip;
    uint16_t sp;
    uint16_t dp;
    uint8_t proto;
} _WRS_PACK_ALIGN(1) Fivetuple;

uint64_t get_time()
{
    struct timespec time1 = {0, 0};
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time1);
    uint64_t ns = time1.tv_sec * 1000000000 + time1.tv_nsec;

    return ns;
}

uint64_t rdtsc()
{
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc"
            : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

static const struct rte_eth_conf port_conf_default = {
        .rxmode = {
                .max_lro_pkt_size = RTE_ETHER_MAX_LEN,
        },
};

const int num_rx_queues = 1; // 接收队列，最多有8个，这里只设置1个接收队列数量
const int num_tx_queues = 0;

// 低位在前面
void show_ip(uint32_t ip)
{
    uint8_t *p = (uint8_t *)&ip;
    for (int i = 3; i > 0; i--)
    {
        printf("%" PRIu8 ":", p[i]);
    }
    printf("%" PRIu8 "\n", p[0]);
}

static inline int
init_port(uint16_t port, struct rte_mempool *mbuf_pool)
{
    struct rte_eth_conf port_conf = port_conf_default;
    const uint16_t rx_rings = 1, tx_rings = 1;
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

// todo 中断处理
// CMsketch sketch;
Fivetuple *ft;
struct bloomfilter *bl;
struct countingtable *ct;

int64_t pkts_count = 0, pkts_num = 1000000000;

int next_packet = 1;
int next_bucket = 0;


//TODO 起一个线程用于处理RDMA操作

int main(int argc, char *argv[])
{
    int ret;
    bool flag;
    // Fivetuple *ft;
    ft = (Fivetuple *)malloc(sizeof(Fivetuple));
    bl = (struct bloomfilter *)malloc(sizeof(struct bloomfilter));
    ct = (struct countingtable *)malloc(sizeof(struct countingtable));

    // cms_init(&sketch, 7, 10000, NULL);
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
    struct rte_mempool *mbuf_pool = rte_pktmbuf_pool_create(
            "mbuf pool", NUM_MBUFS, MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());

    if (mbuf_pool == NULL)
    {
        rte_exit(EXIT_FAILURE, "Could not create mbuf pool\n");
    }

    // 启动dpdk
    uint16_t portid = 0;
    ret = init_port(portid, mbuf_pool);
    if (ret != 0)
    {
        printf("port init error!\n");
    }

    printf("start to recveve data!\n");
    uint64_t start_cycle = rdtsc();
    uint64_t start_time = get_time();
    while (1)
    {

        struct rte_mbuf *mbufs[BURST_SIZE];
        struct rte_ether_hdr *eth_hdr;

        uint16_t num_recv = rte_eth_rx_burst(portid, 0, mbufs, BURST_SIZE);
        if (num_recv > BURST_SIZE)
        {
            // 溢出
            rte_exit(EXIT_FAILURE, "Error receiving from eth\n");
        }
        flag = 1;
        for (int i = 0; i < num_recv; i++)
        {
            if (flag)
            {
                // printf("recieve %d packet!\n", num_recv);
                flag = 0;
            }
            struct rte_mbuf *buf = mbufs[i];

            memset(ft, 0, sizeof(Fivetuple));

            uint32_t sip, dip;
            uint16_t sp, dp;
            uint8_t proto_id;

            // 获取五元组

            struct rte_ipv4_hdr *ipv4_hdr = rte_pktmbuf_mtod_offset(buf, struct rte_ipv4_hdr *, sizeof(struct rte_ether_hdr));
            sip = rte_be_to_cpu_32(ipv4_hdr->src_addr);
            dip = rte_be_to_cpu_32(ipv4_hdr->dst_addr);
            proto_id = ipv4_hdr->next_proto_id;

            struct rte_tcp_hdr *tcp_hdr = rte_pktmbuf_mtod_offset(buf, struct rte_tcp_hdr *, sizeof(struct rte_ether_hdr) + sizeof(struct rte_ipv4_hdr));
            sp = rte_be_to_cpu_16(tcp_hdr->src_port);
            dp = rte_be_to_cpu_16(tcp_hdr->dst_port);

            // 给结构体赋值
            ft->sip = sip;
            ft->dip = dip;
            ft->sp = sp;
            ft->dp = dp;
            ft->proto = proto_id;

            unsigned int len = sizeof(Fivetuple) / sizeof(char);

            // printf("size of Fivetuple: %d\n", sizeof(Fivetuple));
            // printf("size of char %d\n", sizeof(char));
            // printf("size of uint_32_t %d\n", sizeof(uint32_t));

            /// 这一段用来打印
            printf("src_addr = ");
            show_ip(ft->sip);

            printf("des_addr = ");
            show_ip(ft->dip);

            printf("src_port = %" PRIu32 "\n", ft->sp);
            printf("des_port = %" PRIu32 "\n", ft->dp);

            printf("proto_id = %d\n", (int)proto_id);
            printf("length = %d\n", len);
            /// 这一段用来打印

            // 这一段是时间瓶颈，因为要计算hash

            // bl->k = bf_k;

            bool res = true;
            // uint64_t len = sizeof(ft);
            for (unsigned i = 0; i < bf_k; i++)
            {
                uint32_t hash = murmur3(ft, sizeof(ft), i * i);
                uint32_t target_idx = hash & (w_bf - 1);
                if (bl->values[target_idx] == 0)
                {
                    res = false;
                    break;
                }
            }
            if (res == false)
            {
                for (unsigned i = 0; i < bf_k; i++)
                {
                    uint32_t hash = murmur3(ft, sizeof(ft), i * i);
                    uint32_t target_idx = hash & (w_bf - 1);
                    bl->values[target_idx] = 1;
                }
            }

            uint64_t k1, k2;
            k1 = (uint64_t)ft->sip;
            k2 = (uint64_t)ft->dip;
            uint64_t combined_k = (k1 << 32) + k2;

            for (unsigned i = 0; i < ct_k; i++)
            {
                uint32_t hash = murmur3(ft, sizeof(ft), i * i);
                uint32_t target_idx = hash & (w_ct - 1);
                if (res == false)
                { // 之前没有出现这个流
                    ct->bucket[target_idx].FlowXOR ^= combined_k;
                    ct->bucket[target_idx].FlowCount++;
                }
                ct->bucket[target_idx].PacketCount++;
            }

            pkts_count++;

            if (pkts_count % 100000000 == 0)
            {
                uint64_t end_cycle = rdtsc();
                uint64_t end_time = get_time();

                printf("avg throughoutput:%ld\n", pkts_count * 1000000000 / (end_time - start_time));
                printf("avg cpu/packet:%ld\n", (end_cycle - start_cycle) / pkts_count);
                // printf("%ld\n", (end_cycle - start_cycle));
                if (pkts_count / 100000000 == 2)
                {
                    for (uint16_t i = 0; i < num_recv; i++)
                    {
                        rte_pktmbuf_free(mbufs[i]);
                    }
                    free(ft);
                    exit(0);
                }
            }

            // if(num > 10)
            // printf("10 times detect");
        }

        for (uint16_t i = 0; i < num_recv; i++)
        {
            rte_pktmbuf_free(mbufs[i]);
        }
    }

    free(ft);
    // cms_destroy(&sketch);
}

// 五元组：源Ip (source IP), 源端口(source port),目标Ip (destination IP), 目标端口(destination port),4层通信协议 (the layer 4 protocol)

/*
void add(CountMinSketch* cms, const char* key){
    uint64_t* hashs = cms->hash_function(cms->depth, key);
    uint64_t bin;
    for(int i = 0; i < cms->depth; i++){
        bin = hashs[i] % cms->width + i * cms->width;
        cms->bins[bin] += 1;
    }
    free(hashs);
}
*/

// int mainq(){
//     cms_init(&cms, 10000, 7);
//
//     int i, res;
//     uint32_t r;
//     for (i = 0; i < 10; i++) {
////        res = add(&cms, "this is a test");
//        res = cms_add(&cms, "this is a test");
//    }
//
//    r = quary(&cms, "this is a test");
//    printf("%d", r);
//    if (r != 10) {
//        printf("Error with lookup: %d\n", r);
//    }
//    cms_destroy(&cms);
//    return 0;
//};