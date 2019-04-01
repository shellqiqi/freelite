#include "lite-lib.h"
#include "rpc.h"

int userspace_liteapi_get_node_id(void)
{
    return rpc_get_node_id();
}

int userspace_liteapi_join(char *input_str,
                           int eth_port,
                           int ib_port)
{
    struct para_join para = {
        .func_code = FUNC_userspace_liteapi_join,
        .eth_port = eth_port,
        .ib_port = ib_port
    };
    strncpy(para.input_str, input_str, strlen(input_str));
    return rpc_join(&para);
}
