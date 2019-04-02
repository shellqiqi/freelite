#include "lib/lite-lib.h"

int main(int argc, char *argv[])
{
    printf("userspace_liteapi_join: %d\n",
           userspace_liteapi_join("127.0.0.1", 18500, 1));
    printf("userspace_liteapi_get_node_id: %d\n",
           userspace_liteapi_get_node_id());
    printf("userspace_liteapi_alloc_remote_mem: %d\n",
           userspace_liteapi_alloc_remote_mem(1, 1024, 1, 12345));
    return 0;
}
