#include "include/rdma-utils.h"
#include <infiniband/verbs.h>
#include <rdma/rdma_cma.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdint>
#include <iostream>
#include <netdb.h>
#include <thread>
#include <endian.h>
#include "include/common.h"
#include <thread>
#include <time.h>
#include <vector>
#include <algorithm>

#define SQ_DEPTH 16

void simple_server::setup_buffer() {
    int ret;
    recv_mr = ibv_reg_mr(pd, &recv_buf, sizeof(struct rdma_info),
                         IBV_ACCESS_LOCAL_WRITE);
    if (!this->recv_mr){
        std::cerr << "ibv_reg_mr error" << std::endl;
        exit(1);
    }

    send_mr = ibv_reg_mr(pd, &send_buf, sizeof(struct rdma_info), 0);
    if (!this->send_mr){
        std::cerr << "ibv_reg_mr error" << std::endl;
        exit(1);
    }


    //这里的rdma buf 的内存应该提前分配好
    rdma_mr = ibv_reg_mr(pd, rdma_buf, rdma_size,
                         IBV_ACCESS_LOCAL_WRITE |
                         IBV_ACCESS_REMOTE_READ |
                         IBV_ACCESS_REMOTE_WRITE);
    if (!rdma_mr){
        std::cerr << "ibv_reg_mr error" << std::endl;
        exit(1);
    }

    start_mr = ibv_reg_mr(pd, start_buf, start_size,
                          IBV_ACCESS_LOCAL_WRITE |
                          IBV_ACCESS_REMOTE_READ |
                          IBV_ACCESS_REMOTE_WRITE);
    if (!start_mr){
        std::cerr << "ibv_reg_mr error" << std::endl;
        exit(1);
    }


    recv_sgl.addr = (uint64_t )(unsigned long) &this->recv_buf;
    recv_sgl.length = sizeof(struct rdma_info);
    recv_sgl.lkey = this->recv_mr->lkey;
    rq_wr.num_sge = 1;
    rq_wr.sg_list = &this->recv_sgl;


    send_sgl.addr = (uint64_t )(unsigned long) &this->send_buf;
    send_sgl.length = sizeof(struct rdma_info);
    send_sgl.lkey = this->send_mr->lkey;
    sq_wr.num_sge = 1;
    sq_wr.sg_list = &this->send_sgl;
    sq_wr.send_flags = IBV_SEND_SIGNALED;
    sq_wr.opcode = IBV_WR_SEND;


    rdma_sgl.addr = (uint64_t )(unsigned long) this->rdma_buf;
    rdma_sgl.lkey = this->rdma_mr->lkey;
    rdma_sq_wr.num_sge = 1;
    rdma_sq_wr.sg_list = &this->rdma_sgl;
    rdma_sq_wr.send_flags = IBV_SEND_SIGNALED;
}

void simple_server::cq_thread() {
    struct ibv_cq *ev_cq;
    void *ev_ctx;
    int ret;

    while (1){
        ret = ibv_get_cq_event(channel, &ev_cq, &ev_ctx);
        if(ret){
            std::cout << "ibv_get_cq_event error" << std::endl;
            exit(1);
        }
        if(ev_cq != cq){
            std::cout << "ibv_get_cq_event error" << std::endl;
            exit(1);
        }

        ret = ibv_req_notify_cq(cq, 0);
        if(ret){
            std::cout << "ibv_req_notify_cq error" << std::endl;
            exit(1);
        }

        struct ibv_wc wc;
        //接受的一个wc的列表（指针）wc[0] 是第一个
        while ((ret = ibv_poll_cq(cq, 1, &wc)) == 1){
            ret = 0;
            switch (wc.opcode) {
                case IBV_WC_RECV:
                    std::cout << "rdma recv success" << std::endl;
                    server_recv_handler(wc);
                    break;
                case IBV_WC_SEND:
                    std::cout << "rdma send success" << std::endl;
                    state = SERVER_RDMA_ADDR_SEND_COMPLETE;
                    break;
                case IBV_WC_RDMA_READ:
                    std::cout << "rdma read success" << std::endl;
//                    memcpy(&sketch_data[0], rdma_buf, remote_len);
//                    for (int i = 0; i < (remote_len / sizeof(std::pair<int, int>)); i++){
//                        std::cout << sketch_data[i].first << " " << sketch_data[i].second << std::endl;
//                    }
                    std::cout << "read data: " << rdma_buf << std::endl;
                    break;
                default:
                    break;

            }
        }
        ibv_ack_cq_events(cq, 1);
    }
}

void simple_server::rdma_read() {
    if(!SERVER_GET_REMOTE_ADDR){
        std::cout << "error" << std::endl;
        exit(1);
    }
    struct ibv_send_wr *bad_wr;
    int ret;

    if(SERVER_GET_REMOTE_ADDR = false){
        std::cout << "error" << std::endl;
        exit(1);
    }

    rdma_sq_wr.opcode = IBV_WR_RDMA_READ;
    rdma_sq_wr.wr.rdma.remote_addr = remote_addr;
    rdma_sq_wr.wr.rdma.rkey = remote_rkey;

    rdma_sgl.addr = (uint64_t )(unsigned long) rdma_buf;
    rdma_sgl.lkey = rdma_mr->lkey;
    rdma_sgl.length = remote_len;

    rdma_sq_wr.num_sge = 1;
    rdma_sq_wr.sg_list = &this->rdma_sgl;

    ret = ibv_post_send(qp, &rdma_sq_wr, &bad_wr);
    if (ret){
        std::cerr << "ibv_post_send error: " << strerror(errno) << std::endl;
        exit(1);
    }
}

void simple_server::rdma_write() {
    struct ibv_send_wr *bad_wr;
    int ret;

    strcpy(rdma_buf, "write some data from client");

    rdma_sq_wr.opcode = IBV_WR_RDMA_WRITE;
    rdma_sq_wr.wr.rdma.remote_addr = remote_addr;
    rdma_sq_wr.wr.rdma.rkey = remote_rkey;

    rdma_sgl.addr = (uint64_t )(unsigned long) this->rdma_buf;
    rdma_sgl.lkey = this->rdma_mr->lkey;
    rdma_sgl.length = remote_len;

    rdma_sq_wr.num_sge = 1;
    rdma_sq_wr.sg_list = &rdma_sgl;

    std::cout << "发送写请求" << std::endl;

    ret = ibv_post_send(qp, &rdma_sq_wr, &bad_wr);
    if (ret){
        std::cerr << "ibv_post_send error: " << strerror(errno) << std::endl;
        exit(1);
    }
}

void simple_server::server_recv_handler(struct ibv_wc& wc) {
    if(wc.byte_len != sizeof(recv_buf)){
        std::cout << "接受错误" << std::endl;
        exit(1);
    }

    remote_addr = be64toh(recv_buf.buf);
    remote_len = be32toh(recv_buf.size);
    remote_rkey = be32toh(recv_buf.rkey);

    std::cout << "接受到远程地址信息 addr: " << remote_addr << std::endl;
    std::cout << "接受到远程地址信息 len: " << remote_len << std::endl;
    std::cout << "接受到远程地址信息 rkey: " << remote_rkey << std::endl;

    SERVER_GET_REMOTE_ADDR = true;


}

void simple_server::cm_thread(){
    struct rdma_cm_event *event;
    struct rdma_cm_id *id;
    while (true){
        int ret = rdma_get_cm_event(cm_channel, &event);
        if (ret) {
            std::cerr << "rdma_get_cm_event error: " << strerror(errno) << std::endl;
            exit(1);
        }
        id = event->id;
        switch (event->event) {
            case RDMA_CM_EVENT_ADDR_RESOLVED:
//                ret = rdma_resolve_route(id, 2000);
//                if (ret){
//                    std::cerr << "rdma_resolve_route error: " << strerror(errno) << std::endl;
//                    exit(1);
//                }
//                state = ADDR_RESOLVED;
                break;
            case RDMA_CM_EVENT_ROUTE_RESOLVED:
//                if (state != ADDR_RESOLVED){
//                    std::cout << "error" << std::endl;
//                }
//                state = ROUTE_RESOLVED;
                break;
            case RDMA_CM_EVENT_CONNECT_REQUEST:
                std::cout << "服务端接收到连接请求" << std::endl;
                state = REQUEST_GET;
                child_cm_id = id;
                break;
            case RDMA_CM_EVENT_CONNECT_RESPONSE:
                state = SERVER_CONNECTED;
                break;

            case RDMA_CM_EVENT_ESTABLISHED:
                state = SERVER_CONNECTED;
                break;
            default:
                break;
        }
        ret = rdma_ack_cm_event(event);
        if (ret) {
            std::cerr << "rdma_ack_cm_event error: " << strerror(errno) << std::endl;
            exit(1);
        }
    }
}

void simple_server::bindaddr() {
    int ret;
    struct addrinfo *res;
    sin = (struct sockaddr_storage*)malloc(sizeof(struct sockaddr_storage));
    if(!sin){
        std::cout << "malloc error" << std::endl;
        exit(1);
    }


    ret = getaddrinfo(this->ip, NULL, NULL, &res);
    if (ret) {
        std::cerr << "getaddrinfo error: " << gai_strerror(ret) << std::endl;
        exit(1);
    }
    if (res->ai_family == PF_INET){
        memcpy(this->sin, res->ai_addr, sizeof(struct sockaddr_in));
    } else if (res->ai_family == PF_INET6){
        memcpy(this->sin, res->ai_addr, sizeof(struct sockaddr_in6));
    } else {
        std::cerr << "unknown address family" << std::endl;
        exit(1);
    }
    if(this->sin->ss_family == AF_INET){
        ((struct sockaddr_in *)this->sin)->sin_port = this->port;
    } else
        ((struct sockaddr_in6 *)this->sin)->sin6_port = this->port;

    ret = rdma_bind_addr(cm_id, (struct sockaddr *)this->sin);
    if (ret){
        std::cerr << "rdma_bind_addr error: " << strerror(errno) << std::endl;
        exit(1);
    }

    ret = rdma_listen(cm_id, 3);
    if (ret){
        std::cerr << "rdma_listen error: " << strerror(errno) << std::endl;
        exit(1);
    }

    //等待接收到连接请求

    while (state != REQUEST_GET) {}
}

void simple_server::start() {
    int ret;
    struct ibv_recv_wr *bad_wr;

    cm_channel = create_first_event_channel();
    if(!cm_channel){
        std::cout << "create cm channel error" << std::endl;
        exit(1);
    }

    ret = rdma_create_id(cm_channel, &cm_id, NULL, RDMA_PS_TCP);
    if (ret){
        std::cerr << "rdma_create_id error: " << strerror(errno) << std::endl;
        exit(1);
    }

    //创建监听cm
    std::thread cmthread([this](){
        this->cm_thread();
    });
    cmthread.detach();

    bindaddr();

    pd = ibv_alloc_pd(child_cm_id->verbs);
    if (!pd){
        std::cerr << "ibv_alloc_pd error: " << strerror(errno) << std::endl;
        exit(1);
    }

    channel = ibv_create_comp_channel(child_cm_id->verbs);
    if (!channel){
        std::cerr << "ibv_create_comp_channel error: " << strerror(errno) << std::endl;
        exit(1);
    }

    cq = ibv_create_cq(child_cm_id->verbs, SQ_DEPTH * 2, NULL, channel, 0);
    if (!cq){
        std::cerr << "ibv_create_cq error: " << strerror(errno) << std::endl;
        exit(1);
    }

    ret = ibv_req_notify_cq(cq, 0);
    if (ret){
        std::cerr << "ibv_req_notify_cq error: " << strerror(errno) << std::endl;
        exit(1);
    }

    struct ibv_qp_init_attr init_attr;
    memset(&init_attr, 0, sizeof(init_attr));
    init_attr.cap.max_send_wr = SQ_DEPTH;
    init_attr.cap.max_recv_wr = 2;
    init_attr.cap.max_recv_sge = 1;
    init_attr.cap.max_send_sge = 1;
    init_attr.qp_type = IBV_QPT_RC;
    init_attr.send_cq = cq;
    init_attr.recv_cq = cq;

    ret = rdma_create_qp(child_cm_id, pd, &init_attr);
    if(!ret){
        qp = child_cm_id->qp;
    } else{
        std::cerr << "error" << strerror(errno) << std::endl;
        exit(1);
    }
    setup_buffer();

    ret = ibv_post_recv(qp, &rq_wr, &bad_wr);
    if (ret){
        std::cerr << "ibv_post_recv error: " << strerror(errno) << std::endl;
        exit(1);
    }
    //启动cq
    std::thread cqthread([this](){
        this->cq_thread();
    });

    cqthread.detach();

    ret = rdma_accept(child_cm_id, NULL);
    if (ret){
        std::cerr << "rdma_accept error: " << strerror(errno) << std::endl;
        exit(1);
    }

    while (state != CONNECTED){}
    std::cout << "连接建立" << std::endl;

    //等待接受数据
    while (SERVER_GET_REMOTE_ADDR == false){}


    //发送数据
    send_buf.buf = htobe64((uint64_t)(unsigned long)this->start_buf);
    send_buf.rkey = htobe32(this->start_mr->rkey);
    send_buf.size = htobe32(this->start_size);

    sq_wr.opcode = IBV_WR_SEND_WITH_IMM;
    sq_wr.imm_data = htobe32(1122);

    struct ibv_send_wr *bad_send_wr;
    ret = ibv_post_send(qp, &sq_wr, &bad_send_wr);
    if(ret){
        std::cout << "post send error" << std::endl;
        exit(1);
    }

    while (state != SERVER_RDMA_ADDR_SEND_COMPLETE){}


}


int main() {
//    char *start_buf = (char *)malloc(32);
//    char *rdma_buf = (char *)malloc(32);

    char *start_buf, *rdma_buf;

    start_buf = (char *)malloc(1000);
    rdma_buf = (char *)malloc(1000);

    strcpy(start_buf, "hello world form server");


    simple_server *server = new simple_server("10.0.0.5", 1245, start_buf, 1000, rdma_buf, 1000);
    server->start();
//    server->rdma_read();




    while (1){}
};

