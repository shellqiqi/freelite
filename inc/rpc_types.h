#ifndef RPC_TYPES_H
#define RPC_TYPES_H

#include <stdbool.h>

struct join_req_msg {
    char input_str[16];
    int eth_port;
    int ib_port;
};

struct alloc_remote_mem_req_msg {
    unsigned int node_id;
    unsigned int size;
    bool atomic_flag;
    int password;
};

/* RPC Request message */
struct rpc_req_msg {
    int func_code;
    union {
        struct join_req_msg join_req;
        struct alloc_remote_mem_req_msg alloc_remote_mem_req;
    } msg_body;
};

/* RPC Response message */
struct rpc_rsp_msg {
    union {
        int int_rval;
    } msg_body;
};

#endif
