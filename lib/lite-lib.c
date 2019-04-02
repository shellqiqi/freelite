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
    struct rpc_rsp_msg rsp_msg;

    strncpy(req_msg.msg_body.join_req.input_str, input_str, strlen(input_str));
    rpc_handler(&req_msg, &rsp_msg);
    return rsp_msg.msg_body.int_rval;
}
