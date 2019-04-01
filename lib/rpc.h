#ifndef RPC_H
#define RPC_H

#include <stdint.h>

#include "../inc/rpc_types.h"
#include "../inc/func_code.h"

int rpc_get_node_id();
int rpc_join(const struct para_join *para);

#endif
