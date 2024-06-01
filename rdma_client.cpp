//
// Created by SunX on 2024/3/14.
//
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
#include <vector>
#include <algorithm>

#define SQ_DEPTH 16



rdma_client::rdma_client(const char *ip, int port, void *start_buf, int start_size, void *rdma_buf, int rdma_size) {
    this->ip = ip;
    this->port = htobe16(port);
    this->start_buf = (char *)start_buf;
    this->start_size = start_size;
    this->rdma_buf = (char *)rdma_buf;
    this->rdma_size = rdma_size;
}

void rdma_client::bindaddr(){
    struct addrinfo *res;
    int ret;
    this->sin = (struct sockaddr_storage *)malloc(sizeof(struct sockaddr_storage));
    if(!this->sin){
        std::cerr << "malloc error" << std::endl;
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

    ret = rdma_resolve_addr(this->cm_id, NULL, (struct sockaddr *)this->sin, 2000);
    if (ret) {
        std::cerr << "rdma_reslove_addr error: " << strerror(errno) << std::endl;
        exit(1);
    }
    while (state != ROUTE_RESOLVED){}
}

void rdma_client::cm_thread(){

    struct rdma_cm_event *event;
    while (true){
        int ret = rdma_get_cm_event(cm_channel, &event);
        if (ret) {
            std::cerr << "rdma_get_cm_event error: " << strerror(errno) << std::endl;
            exit(1);
        }
        switch (event->event) {
            case RDMA_CM_EVENT_ADDR_RESOLVED:
                ret = rdma_resolve_route(this->cm_id, 2000);
                if (ret){
                    std::cerr << "rdma_resolve_route error: " << strerror(errno) << std::endl;
                    exit(1);
                }
                state = ADDR_RESOLVED;
                break;
            case RDMA_CM_EVENT_ROUTE_RESOLVED:
                if (state != ADDR_RESOLVED){
                    std::cout << "error" << std::endl;
                }
                state = ROUTE_RESOLVED;
                break;


            case RDMA_CM_EVENT_ESTABLISHED:
                state = CONNECTED;
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

void rdma_client::recv_handler(struct ibv_wc &wc){
    if(wc.byte_len != sizeof(recv_buf)){
        std::cout << "接受错误" << std::endl;
        exit(1);
    }

    if(wc.wc_flags & IBV_WC_WITH_IMM){

        uint32_t imm_data;
        imm_data = be32toh(wc.imm_data);
        switch (imm_data) {
            case 1122:
                std::cout << "get server remote addr" << std::endl;
                remote_addr = be64toh(recv_buf.buf);
                remote_len = be32toh(recv_buf.size);
                remote_rkey = be32toh(recv_buf.rkey);

                std::cout << "接受到远程地址信息 addr: " << remote_addr << std::endl;
                std::cout << "接受到远程地址信息 len: " << remote_len << std::endl;
                std::cout << "接受到远程地址信息 rkey: " << remote_rkey << std::endl;
                GET_RDMA_ADDR = true;
                break;
            default:
                break;
        }

        ibv_post_recv(qp, &rq_wr, NULL);
    }


}

void rdma_client::cq_thread() {
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
        while ((ret = ibv_poll_cq(cq, 1, &wc)) == 1){
            ret = 0;
            switch (wc.opcode) {
                case IBV_WC_SEND:
                    if(print_flag){
                        std::cout << "send complete" << std::endl;
                    }
                    break;
                case IBV_WC_RECV:
                    if(print_flag)
                        std::cout << "recv complete" << std::endl;
                    recv_handler(wc);
                    break;
                case IBV_WC_RECV_RDMA_WITH_IMM:
                    std::cout << "recv rdma with imm" << std::endl;;
                    break;
                case IBV_WC_RDMA_READ:
                    if(print_flag)
                        std::cout << "rdma read complete" << std::endl;
                    RDMA_READ_COMPLETE = true;
//                    std::cout << "read data: " << rdma_buf << std::endl;
                    break;
                case IBV_WC_RDMA_WRITE:
                    RDMA_WRITE_COMPLETE = true;
                    std::cout << "rdma write complete" << std::endl;
                    break;
                default:
                    break;
            }

        }

        ibv_ack_cq_events(cq, 1);

    }

}

void rdma_client::ow_read(){
    if(!GET_RDMA_ADDR){
        std::cout << "error" << std::endl;
        exit(1);
    }
    RDMA_READ_COMPLETE = false;
    struct ibv_send_wr *bad_wr;
    int ret;

    std::vector<struct ibv_send_wr> wr_list;
    std::vector<struct ibv_sge> sge_list;

//    struct ibv_send_wr rdma_sq_wr_tmp;
//    struct ibv_sge rdma_sgl_tmp;

//    memset(&rdma_sq_wr_tmp, 0, sizeof(rdma_sq_wr_tmp));
//    memset(&rdma_sgl_tmp, 0, sizeof(rdma_sgl_tmp));

    int batch = 1000;
    for(int i = 0; i < remote_len; i += batch){


        rdma_sq_wr.opcode = IBV_WR_RDMA_READ;
        rdma_sq_wr.wr.rdma.remote_addr = remote_addr + batch;
        rdma_sq_wr.wr.rdma.rkey = remote_rkey;
        rdma_sq_wr.send_flags = IBV_SEND_SIGNALED;


        rdma_sgl.addr = (uint64_t )(unsigned long) this->rdma_buf + batch;
        rdma_sgl.lkey = this->rdma_mr->lkey;
        rdma_sgl.length = batch;

        rdma_sq_wr.num_sge = 1;
        rdma_sq_wr.sg_list = &rdma_sgl;

//        wr_list.push_back(rdma_sq_wr_tmp);
//        sge_list.push_back(rdma_sgl_tmp);

        ret = ibv_post_send(qp, &rdma_sq_wr, &bad_wr);
        if (ret) {
            std::cerr << "ibv_post_send error: " << strerror(errno) << std::endl;
            exit(1);
        }

        while (RDMA_READ_COMPLETE == false){}
        std::this_thread::sleep_for(std::chrono::microseconds(15));

    }

//    for (size_t i = 0; i < wr_list.size() - 1; ++i) {
//        wr_list[i].next = &wr_list[i + 1];
//    }
//    wr_list.back().next = nullptr;
    if (print_flag)
        std::cout << "read data: " << rdma_buf << std::endl;



}

void rdma_client::rdma_read() {
    if(!GET_RDMA_ADDR){
        std::cout << "error" << std::endl;
        exit(1);
    }

    RDMA_READ_COMPLETE = false;
    struct ibv_send_wr *bad_wr;
    int ret;
    rdma_sq_wr.opcode = IBV_WR_RDMA_READ;
    rdma_sq_wr.wr.rdma.remote_addr = remote_addr;
    rdma_sq_wr.wr.rdma.rkey = remote_rkey;

    rdma_sgl.addr = (uint64_t )(unsigned long) this->rdma_buf;
    rdma_sgl.lkey = this->rdma_mr->lkey;
    rdma_sgl.length = remote_len;

    rdma_sq_wr.num_sge = 1;
    rdma_sq_wr.sg_list = &rdma_sgl;


//    std::cout << "发送读请求" << std::endl;
    ret = ibv_post_send(qp, &rdma_sq_wr, &bad_wr);
    if (ret){
        std::cerr << "ibv_post_send error: " << strerror(errno) << std::endl;
        exit(1);
    }

    while (RDMA_READ_COMPLETE == false){}
    if(print_flag)
        std::cout << "read data: " << rdma_buf << std::endl;
}

void rdma_client::rdma_write() {

    RDMA_WRITE_COMPLETE = false;
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

//    while (RDMA_WRITE_COMPLETE == false){}
}

void rdma_client::setup_buffer() {
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

void rdma_client::start() {
    int ret;
    struct rdma_cm_id *id;
    cm_channel = create_first_event_channel();
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

    pd = ibv_alloc_pd(cm_id->verbs);
    if (!pd) {
        std::cerr << "ibv_alloc_pd error: " << strerror(errno) << std::endl;
        exit(1);
    }
    channel = ibv_create_comp_channel(cm_id->verbs);
    if (!channel) {
        std::cerr << "ibv_create_comp_channel error: " << strerror(errno) << std::endl;
        exit(1);
    }
    cq = ibv_create_cq(cm_id->verbs, SQ_DEPTH * 2, NULL, channel, 0);
    if (!cq){
        std::cerr << "ibv_create_cq error: " << strerror(errno) << std::endl;
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
    id = cm_id;

    ret = rdma_create_qp(id, pd, &init_attr);
    if(!ret){
        qp = id->qp;
    } else{
        std::cerr << "error" << strerror(errno) << std::endl;
        exit(1);
    }
    setup_buffer();

    struct ibv_recv_wr *bad_wr;
    ret = ibv_post_recv(qp, &rq_wr,  &bad_wr);
    if(ret){
        std::cout << "error in post recv" << std::endl;
    }

    //启动cq
    std::thread cqthread([this](){
        this->cq_thread();
    });

    cqthread.detach();

    struct rdma_conn_param conn_param = {};
    conn_param.responder_resources = 1;
    conn_param.initiator_depth = 1;
    conn_param.retry_count = 7;
    conn_param.rnr_retry_count = 7;

    ret = rdma_connect(this->cm_id, &conn_param);
    if (ret) {
        std::cerr << "rdma_accept error" << std::endl;
        exit(1);
    }


    while (state != CONNECTED){}
    std::cout << "连接建立" << std::endl;

    //尝试发送数据
    send_buf.buf = htobe64((uint64_t)(unsigned long)this->start_buf);
    send_buf.rkey = htobe32(this->start_mr->rkey);
    send_buf.size = htobe32(this->start_size);

    sq_wr.opcode = IBV_WR_SEND;
    struct ibv_send_wr *bad_send_wr;
    ret = ibv_post_send(qp, &sq_wr, &bad_send_wr);
    if(ret){
        std::cout << "post send error" << std::endl;
        exit(1);
    }

    //等待接受数据
    while(GET_RDMA_ADDR == false){}
}