#ifndef RPC_TYPES_H
#define RPC_TYPES_H

struct para_join {
    int func_code;
    char input_str[16];
    int eth_port;
    int ib_port;
};

#endif
