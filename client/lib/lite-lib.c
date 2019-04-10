#include "../../inc/lite-lib.h"
#include "../../inc/log.h"
#include "rpc.h"

/**
 * Get the id of current node
 * Return: node id
 */
int userspace_liteapi_get_node_id()
{
    struct rpc_req_msg req_msg = {
        .func_code = FUNC_userspace_liteapi_get_node_id
    };
    struct rpc_rsp_msg rsp_msg;

    rpc_handler(&req_msg, &rsp_msg);
    return rsp_msg.rval.int_rsp;
}

/**
 * Establish a connection to a remote cluster manager
 * Input:
 *    input_str: ipv4 address in string form
 *    eth_port: respected remote ethernet port
 *    ib_port: local infiniband device port
 * Return: Node id or error code
 */
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
    return rsp_msg.rval.int_rsp;
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
    return rsp_msg.rval.int_rsp;
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
    return rsp_msg.rval.int_rsp;
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
    return rsp_msg.rval.int_rsp;
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
    return rsp_msg.rval.int_rsp;
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
    return rsp_msg.rval.int_rsp;
}

/**
 * Processing a reply request in RPC from userspace
 * Input:
 *    addr: input address
 *    size: reply size
 *    descriptor: header of reply message (returned by lite_api_receive)
 *    priority: priority of the request
 * Return: Error code
 */
int userspace_liteapi_reply_message(void *addr,
                                    int size,
                                    uintptr_t descriptor)
{
    struct rpc_req_msg req_msg = {
        .func_code = FUNC_userspace_liteapi_reply_message,
        .msg_body.reply_message_req = {
            .addr = addr,
            .size = size,
            .descriptor = descriptor
        }
    };
    struct rpc_rsp_msg rsp_msg;

    rpc_handler(&req_msg, &rsp_msg);
    return rsp_msg.rval.int_rsp;
}

/**
 * Register an application to a specific port for RPC function
 * Input:
 *    destined_port: the targetted port
 *    max_size_per_message: register the possible max size
 *    max_user_per_node: maximum user per node for this operation(not used in current version but for future QoS development)
 *    name: name/string of the application
 *    name_len: length of the name
 * Return: Error code
 */
int userspace_liteapi_register_application(unsigned int destined_port,
                                           unsigned int max_size_per_message,
                                           unsigned int max_user_per_node,
                                           char *name,
                                           uint64_t name_len)
{
    struct rpc_req_msg req_msg = {
        .func_code = FUNC_userspace_liteapi_register_application,
        .msg_body.register_application_req = {
            .destined_port = destined_port,
            .max_size_per_message = max_size_per_message,
            .max_user_per_node = max_user_per_node,
            .name_len = name_len
        }
    };
    strncpy(req_msg.msg_body.register_application_req.name, name, name_len);
    struct rpc_rsp_msg rsp_msg;

    rpc_handler(&req_msg, &rsp_msg);
    return rsp_msg.rval.int_rsp;
}

/**
 * Distributed barrier
 * Input:
 *    check_num: requested number of barrier message
 * Return: Error code
 */
int userspace_liteapi_dist_barrier(unsigned int num)
{
    struct rpc_req_msg req_msg = {
        .func_code = FUNC_userspace_liteapi_dist_barrier,
        .msg_body.dist_barrier_req = {
            .num = num
        }
    };
    struct rpc_rsp_msg rsp_msg;

    rpc_handler(&req_msg, &rsp_msg);
    return rsp_msg.rval.int_rsp;
}

/**
 * Get the metadata information for RPC request
 * must be performed before issueing a RPC request
 * Input:
 *    target_node: target node id
 *    designed_port: target port
 * Return: Error or info code
 */
int userspace_liteapi_query_port(int target_node,
                                 int designed_port)
{
    struct rpc_req_msg req_msg = {
        .func_code = FUNC_userspace_liteapi_query_port,
        .msg_body.query_port_req = {
            .target_node = target_node,
            .designed_port = designed_port
        }
    };
    struct rpc_rsp_msg rsp_msg;

    rpc_handler(&req_msg, &rsp_msg);
    return rsp_msg.rval.int_rsp;
}

/**
 * Alloc shared memory in router
 * Input:
 *    name: shared memory name
 *    size: shared memory size
 * Output:
 *    local_addr: address of client. Handle LOCALLY!
 *    remote_addr: address alloced in router process space
 *                 for release operation
 * Return: error code.
 */
int userspace_liteapi_alloc_local_mem_test(const char *name,
                                           const size_t size,
                                           void **local_addr,
                                           void **remote_addr)
{
    struct rpc_req_msg req_msg = {
        .func_code = FUNC_userspace_liteapi_alloc_local_mem,
        .msg_body.alloc_local_mem_req = {
            .size = size
        }
    };
    strncpy(req_msg.msg_body.alloc_local_mem_req.name, name, strlen(name));
    struct rpc_rsp_msg rsp_msg;

    rpc_handler(&req_msg, &rsp_msg);
    *local_addr = (void *)0x900;
    *remote_addr = rsp_msg.msg_body.alloc_local_mem_rsp.remote_addr;
    return rsp_msg.rval.int_rsp;
}

int userspace_liteapi_alloc_local_mem(const char *name,
                                      const size_t size,
                                      void **local_addr,
                                      void **remote_addr)
{
    int fd;
    struct rpc_req_msg req_msg = {
        .func_code = FUNC_userspace_liteapi_alloc_local_mem,
        .msg_body.alloc_local_mem_req = {
            .size = size
        }
    };
    strncpy(req_msg.msg_body.alloc_local_mem_req.name, name, strlen(name));
    struct rpc_rsp_msg rsp_msg;

    rpc_handler(&req_msg, &rsp_msg);
    if (rsp_msg.rval.int_rsp < 0) return -1;
    if ((fd = shm_open(name, O_RDWR, S_IRUSR | S_IWUSR)) < 0)
    {
        LOG_PERROR("SHM OPEN ERROR");
        return -1;
    }
    *local_addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (local_addr == MAP_FAILED)
    {
        LOG_PERROR("SHM MMAP ERROR");
        return -1;
    }
    *remote_addr = rsp_msg.msg_body.alloc_local_mem_rsp.remote_addr;
    return rsp_msg.rval.int_rsp;
}

/**
 * Free shared memory in router
 * Input:
 *    name: shared memory name
 *    size: shared memory size
 *    local_addr: address of client. Handle LOCALLY!
 *    remote_addr: address of router
 */
int userspace_liteapi_free_local_mem(const char *name,
                                     size_t size,
                                     void *local_addr,
                                     void *remote_addr)
{
    struct rpc_req_msg req_msg = {
        .func_code = FUNC_userspace_liteapi_free_local_mem,
        .msg_body.free_local_mem_req = {
            .size = size,
            .remote_addr = remote_addr
        }
    };
    strncpy(req_msg.msg_body.free_local_mem_req.name, name, strlen(name));
    struct rpc_rsp_msg rsp_msg;

    rpc_handler(&req_msg, &rsp_msg);
    return rsp_msg.rval.int_rsp;
}