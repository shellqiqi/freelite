#ifndef RPC_H
#define RPC_H

#include "rpc_types.h"
#include "func_code.h"

int rpc_handler(const struct rpc_req_msg *req_msg,
                struct rpc_rsp_msg *rsp_msg);

#endif
