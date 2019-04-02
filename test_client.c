#include "lib/lite-lib.h"

int main(int argc, char *argv[])
{
    printf("userspace_liteapi_join: %d\n",
           userspace_liteapi_join("127.0.0.1", 18500, 1));
    printf("userspace_liteapi_get_node_id: %d\n",
           userspace_liteapi_get_node_id());
    printf("userspace_liteapi_alloc_remote_mem: %d\n",
           userspace_liteapi_alloc_remote_mem(1, 1024, 1, 12345));
    printf("userspace_liteapi_rdma_write: %d\n",
           userspace_liteapi_rdma_write(1122, (void *) 0x77f, 1024, 256, 12345));
    printf("userspace_liteapi_rdma_read: %d\n",
           userspace_liteapi_rdma_read(2233, (void *) 0x1ff, 2048, 128, 54321));
    return 0;
}
