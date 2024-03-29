#ifndef RPC_TYPES_H
#define RPC_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* Request messages */

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
    int block_call;
};

struct reply_message_req_msg
{
    void *addr;
    int size;
    uintptr_t descriptor;
};

struct register_application_req_msg
{
    unsigned int destined_port;
    unsigned int max_size_per_message;
    unsigned int max_user_per_node;
    char name[32];
    uint64_t name_len;
};

struct dist_barrier_req_msg
{
    unsigned int num;
};

struct query_port_req_msg
{
    int target_node;
    int designed_port;
};

struct alloc_local_mem_req_msg
{
    char name[256];
    size_t size;
};

struct free_local_mem_req_msg
{
    char name[256];
    size_t size;
    void *remote_addr;
};

/* Response messages */

struct receive_message_fast_rsp_msg
{
    uintptr_t descriptor;
    int ret_length;
};

struct alloc_local_mem_rsp_msg
{
    void *remote_addr;
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
        struct reply_message_req_msg reply_message_req;
        struct register_application_req_msg register_application_req;
        struct dist_barrier_req_msg dist_barrier_req;
        struct query_port_req_msg query_port_req;
        struct alloc_local_mem_req_msg alloc_local_mem_req;
        struct free_local_mem_req_msg free_local_mem_req;
    } msg_body;
};

/* RPC Response message */
struct rpc_rsp_msg
{
    union {
        int int_rsp;
    } rval;
    union {
        struct alloc_local_mem_rsp_msg alloc_local_mem_rsp;
        struct receive_message_fast_rsp_msg receive_message_fast_rsp;
    } msg_body;
};

#endif
