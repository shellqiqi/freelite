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
           userspace_liteapi_rdma_write(1122, (void *)0x77f, 1024, 256, 12345));
    printf("userspace_liteapi_rdma_read: %d\n",
           userspace_liteapi_rdma_read(2233, (void *)0x1ff, 2048, 128, 54321));
    printf("userspace_liteapi_send_reply_imm_fast: %d\n",
           userspace_liteapi_send_reply_imm_fast(2, 1, (void *)0x100, 2048, (void *)0x200, (int *)0x300, 4096));
    printf("userspace_liteapi_receive_message_fast: %d\n",
           userspace_liteapi_receive_message_fast(1, (void *)0x100, 1024, (uintptr_t *)0x200, (int *)0x300, 1));
    printf("userspace_liteapi_reply_message: %d\n",
           userspace_liteapi_reply_message((void *)0xf88, 128, 321));
    printf("userspace_liteapi_register_application: %d\n",
           userspace_liteapi_register_application(1, 256, 4, "hello world", strlen("hello world")));
    printf("userspace_liteapi_dist_barrier: %d\n",
           userspace_liteapi_dist_barrier(16));
    return 0;
}
