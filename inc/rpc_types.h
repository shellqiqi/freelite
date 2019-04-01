#ifndef RPC_TYPES_H
#define RPC_TYPES_H

struct join_req_msg
{
    int func_code;
    char input_str[16];
    int eth_port;
    int ib_port;
};

/* RPC Request message */
struct rpc_req_msg
{
    int func_code;
    union {
        struct join_req_msg join_req;
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
