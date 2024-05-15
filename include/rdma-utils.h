//
// Created by SunX on 2024/3/12.
//

#ifndef HYPER_RDMA_RDMA_UTILS_H
#define HYPER_RDMA_RDMA_UTILS_H

#include <iostream>
#include <rdma/rdma_cma.h>
#include <infiniband/verbs.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdint>

#define SQ_DEPTH 16

struct rdma_info{
    __be64 buf;
    __be32 rkey;
    __be32 size;
};

enum ClientState{
    INIT,
    ROUTE_RESOLVED,
    ADDR_RESOLVED,
    CONNECTED,
};

class rdma_client{
public:
    rdma_client(const char *ip, int port, void *start_buf, int start_size, void *rdma_buf, int rdma_size);
    void init();
    void start();
    void rdma_read();
    void rdma_write();
    void bindaddr();
    void cm_thread();
    void cq_thread();
    void setup_buffer();
    void recv_handler(struct ibv_wc &wc);
    char *get_rdma_buf() const{
        return rdma_buf;
    }
    uint32_t get_rdma_size() const{
        return be32toh(recv_buf.size);
    }

    bool print_flag = true;

private:
    const char *ip;
    __be16 port;
    struct sockaddr_storage *sin;
    char *start_buf;
    int start_size;
    char *rdma_buf;
    int rdma_size;
    enum ClientState state = INIT;
    bool GET_RDMA_ADDR = false;
    bool RDMA_READ_COMPLETE = false;
    bool RDMA_WRITE_COMPLETE = false;

    struct rdma_event_channel *cm_channel;
    struct rdma_cm_id *cm_id;	/* connection on client side,*/

    struct ibv_comp_channel *channel;
    struct ibv_cq *cq;
    struct ibv_pd *pd;
    struct ibv_qp *qp;

    struct ibv_recv_wr rq_wr;	/* recv work request record */
    struct ibv_sge recv_sgl;	/* recv single SGE */
    struct rdma_info recv_buf;/* malloc'd buffer */
    struct ibv_mr *recv_mr;		/* MR associated with this buffer */

    struct ibv_send_wr sq_wr;	/* send work request record */
    struct ibv_sge send_sgl;
    struct rdma_info send_buf;/* single send buf */
    struct ibv_mr *send_mr;        /* MR associated with this buffer */

    struct ibv_send_wr rdma_sq_wr;	/* rdma work request record */
    struct ibv_sge rdma_sgl;	/* rdma single SGE */
    struct ibv_mr *rdma_mr;

    struct ibv_mr *start_mr;

    uint32_t remote_rkey;		/* remote guys RKEY */
    uint64_t remote_addr;		/* remote guys TO */
    uint32_t remote_len;		/* remote guys LEN */


};


enum ServerState{
    SERVER_INIT,
    SERVER_BIND,
    REQUEST_GET,
    SERVER_CONNECTED,
    SERVER_READ_ADV,
    SERVER_WRITE_ADV,
    SERVER_WRITE_COMPLETE1,
    SERVER_READ_COMPLETE1,
    SERVER_RDMA_ADDR_SEND_COMPLETE,
};


class rdma_server{
public:
    rdma_server(const char *ip, int port, void *start_buf, int start_size, void *rdma_buf, int rdma_size){
        this->ip = ip;
        this->port = htobe16(port);
        this->start_buf = (char *)start_buf;
        this->start_size = start_size;
        this->rdma_buf = (char *)rdma_buf;
        this->rdma_size = rdma_size;
    }
    void init();
    void start();
    void rdma_read();
    void rdma_write();
    void bindaddr();
    void cm_thread();
    void cq_thread();
    void setup_buffer();
    void server_recv_handler(struct ibv_wc&);


private:

    const char *ip;
    __be16 port;
    struct sockaddr_storage *sin;
    char *start_buf;
    int start_size;
    char *rdma_buf;
    int rdma_size;
    enum ServerState state = SERVER_INIT;
    bool SERVER_GET_REMOTE_ADDR = false;

    struct rdma_event_channel *cm_channel;
    struct rdma_cm_id *cm_id;	/* connection on client side,*/
    struct rdma_cm_id *child_cm_id;

    struct ibv_comp_channel *channel;
    struct ibv_cq *cq;
    struct ibv_pd *pd;
    struct ibv_qp *qp;

    struct ibv_recv_wr rq_wr;	/* recv work request record */
    struct ibv_sge recv_sgl;	/* recv single SGE */
    struct rdma_info recv_buf;/* malloc'd buffer */
    struct ibv_mr *recv_mr;		/* MR associated with this buffer */

    struct ibv_send_wr sq_wr;	/* send work request record */
    struct ibv_sge send_sgl;
    struct rdma_info send_buf;/* single send buf */
    struct ibv_mr *send_mr;        /* MR associated with this buffer */

    struct ibv_send_wr rdma_sq_wr;	/* rdma work request record */
    struct ibv_sge rdma_sgl;	/* rdma single SGE */
    struct ibv_mr *rdma_mr;

    struct ibv_mr *start_mr;

    uint32_t remote_rkey;		/* remote guys RKEY */
    uint64_t remote_addr;		/* remote guys TO */
    uint32_t remote_len;		/* remote guys LEN */
};



#endif //HYPER_RDMA_RDMA_UTILS_H
