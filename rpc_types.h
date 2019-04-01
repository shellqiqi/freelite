#ifndef RPC_TYPES_H
#define RPC_TYPES_H

#include "func_code.h"

struct para_join {
    int func_code;
    char input_str[3];
    int eth_port;
    int ib_port;
};

#endif
