#ifndef RPC_TYPES_H
#define RPC_TYPES_H

#include <stdbool.h>
#include <stdint.h>

struct join_req_msg
{
    char input_str[16];
    int eth_port;
    int ib_port;
};

struct alloc_remote_mem_req_msg
{
    unsigned int node_id;
    unsigned int size;
    bool atomic_flag;
    int password;
};

struct rdma_write_req_msg
{
    unsigned lite_handler;
    void *local_addr;
    unsigned int size;
    unsigned int offset;
    int password;
};

#define rdma_read_req_msg rdma_write_req_msg

struct send_reply_imm_fast_req_msg
{
    int target_node;
    unsigned int port;
    void *addr;
    int size;
    void *ret_addr;
    int *ret_length;
    int max_ret_size;
};

struct receive_message_fast_req_msg
{
    unsigned int port;
    void *ret_addr;
    int receive_size;
    uintptr_t *descriptor;
    int *ret_length;
    int block_call;
};

/* RPC Request message */
struct rpc_req_msg
{
    int func_code;
    union {
        struct join_req_msg join_req;
        struct alloc_remote_mem_req_msg alloc_remote_mem_req;
        struct rdma_write_req_msg rdma_write_req;
        struct rdma_read_req_msg rdma_read_req;
        struct send_reply_imm_fast_req_msg send_reply_imm_fast_req;
        struct receive_message_fast_req_msg receive_message_fast_req;
    } msg_body;
};

/* RPC Response message */
struct rpc_rsp_msg
{
    union {
        int int_rval;
    } msg_body;
};

#endif
