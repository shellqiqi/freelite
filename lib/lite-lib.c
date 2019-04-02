#include "lite-lib.h"
#include "rpc.h"

int userspace_liteapi_get_node_id()
{
    struct rpc_req_msg req_msg = {
        .func_code = FUNC_userspace_liteapi_get_node_id
    };
    struct rpc_rsp_msg rsp_msg;

    rpc_handler(&req_msg, &rsp_msg);
    return rsp_msg.msg_body.int_rval;
}

int userspace_liteapi_join(char *input_str,
                           int eth_port,
                           int ib_port)
{
    struct rpc_req_msg req_msg = {
        .func_code = FUNC_userspace_liteapi_join,
        .msg_body.join_req = {
            .eth_port = eth_port,
            .ib_port = ib_port
        }
    };
    strncpy(req_msg.msg_body.join_req.input_str, input_str, strlen(input_str));
    struct rpc_rsp_msg rsp_msg;

    rpc_handler(&req_msg, &rsp_msg);
    return rsp_msg.msg_body.int_rval;
}

/**
 * Alloc a memory space at the remote side as a LMR
 * Input:
 *    target_node: id of the remote node
 *    size: requested size
 *    atomic_flag: will this memory space be used for atomic operations?
 *    password: pin code for this memory region
 * Return: LMR Key or error code
 */
int userspace_liteapi_alloc_remote_mem(unsigned int node_id,
                                       unsigned int size,
                                       bool atomic_flag,
                                       int password)
{
    struct rpc_req_msg req_msg = {
        .func_code = FUNC_userspace_liteapi_alloc_remote_mem,
        .msg_body.alloc_remote_mem_req = {
            .node_id = node_id,
            .size = size,
            .atomic_flag = atomic_flag,
            .password = password
        }
    };
    struct rpc_rsp_msg rsp_msg;

    rpc_handler(&req_msg, &rsp_msg);
    return rsp_msg.msg_body.int_rval;
}

/**
 * Processing write request from userspace
 * Input:
 *    lite_handler: lite_handler behind the targetted LMR
 *    local_addr: input address
 *    size: request size
 *    priority: high, low, or non
 *    offset: request offset
 *    password: pin code of the lite_handler
 * Return: Error or info code
 */
int userspace_liteapi_rdma_write(unsigned lite_handler,
                                 void *local_addr,
                                 unsigned int size,
                                 unsigned int offset,
                                 int password)
{
    struct rpc_req_msg req_msg = {
        .func_code = FUNC_userspace_liteapi_rdma_write,
        .msg_body.rdma_write_req = {
            .lite_handler = lite_handler,
            .local_addr = local_addr,
            .size = size,
            .offset = offset,
            .password = password
        }
    };
    struct rpc_rsp_msg rsp_msg;

    rpc_handler(&req_msg, &rsp_msg);
    return rsp_msg.msg_body.int_rval;
}

/**
 * Processing read request from userspace
 * Input:
 *    lite_handler: lite_handler behind the targetted LMR
 *    local_addr: input address
 *    size: request size
 *    priority: high, low, or non
 *    offset: request offset
 *    password: pin code of the lite_handler
 * Return: Error of info code
 */
int userspace_liteapi_rdma_read(unsigned lite_handler,
                                void *local_addr,
                                unsigned int size,
                                unsigned int offset,
                                int password)
{
    struct rpc_req_msg req_msg = {
        .func_code = FUNC_userspace_liteapi_rdma_read,
        .msg_body.rdma_read_req = {
            .lite_handler = lite_handler,
            .local_addr = local_addr,
            .size = size,
            .offset = offset,
            .password = password
        }
    };
    struct rpc_rsp_msg rsp_msg;

    rpc_handler(&req_msg, &rsp_msg);
    return rsp_msg.msg_body.int_rval;
}

/**
 * Processing a send-reply request (RPC) from userspace
 * Input:
 *    target_node: target node id
 *    port: the combination of size and port
 *    addr: input address
 *    ret_addr: address to keep received message
 *    ret_length: keep the returned length of the message (for fast_receive)
 *    max_ret_size: the combination of max_ret_size and priority
 * Return: length of received message
 */
int userspace_liteapi_send_reply_imm_fast(int target_node,
                                          unsigned int port,
                                          void *addr,
                                          int size,
                                          void *ret_addr,
                                          int *ret_length,
                                          int max_ret_size)
{
    struct rpc_req_msg req_msg = {
        .func_code = FUNC_userspace_liteapi_send_reply_imm_fast,
        .msg_body.send_reply_imm_fast_req = {
            .target_node = target_node,
            .port = port,
            .addr = addr,
            .size = size,
            .ret_addr = ret_addr,
            .ret_length = ret_length,
            .max_ret_size = max_ret_size
        }
    };
    struct rpc_rsp_msg rsp_msg;

    rpc_handler(&req_msg, &rsp_msg);
    return rsp_msg.msg_body.int_rval;
}

/**
 * Processing a receive request (RPC-server) from userspace
 * Input:
 *    port: port
 *    ret_addr: address to keep received message
 *    receive_size: receive size
 *    descriptor: address to keep the header/descriptor of the received message (for reply usage)
 *    ret_length: keep the returned length of the message (for fast_receive)
 *    block_call: flag to show whether this is a blocking call or not
 * Return: length of received message
 */
int userspace_liteapi_receive_message_fast(unsigned int port,
                                           void *ret_addr,
                                           int receive_size,
                                           uintptr_t *descriptor,
                                           int *ret_length,
                                           int block_call)
{
    struct rpc_req_msg req_msg = {
        .func_code = FUNC_userspace_liteapi_receive_message_fast,
        .msg_body.receive_message_fast_req = {
            .port = port,
            .ret_addr = ret_addr,
            .receive_size = receive_size,
            .descriptor = descriptor,
            .ret_length = ret_length,
            .block_call = block_call
        }
    };
    struct rpc_rsp_msg rsp_msg;

    rpc_handler(&req_msg, &rsp_msg);
    return rsp_msg.msg_body.int_rval;
}