#ifndef RPC_H
#define RPC_H

#include <stdint.h>

int conn(const char *name);
void disconn(int client_sock);

#endif
