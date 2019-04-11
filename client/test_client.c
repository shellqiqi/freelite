#include "../inc/lite-lib.h"
#include "../inc/log.h"

int main(int argc, char *argv[])
{
    int val;

    LOG_INFO("userspace_liteapi_join: %d\n",
             userspace_liteapi_join("127.0.0.1", 18500, 1));
    LOG_INFO("userspace_liteapi_get_node_id: %d\n",
             userspace_liteapi_get_node_id());
    LOG_INFO("userspace_liteapi_alloc_remote_mem: %d\n",
             userspace_liteapi_alloc_remote_mem(1, 1024, 1, 12345));
    LOG_INFO("userspace_liteapi_rdma_write: %d\n",
             userspace_liteapi_rdma_write(1122, (void *)0x77f, 1024, 256, 12345));
    LOG_INFO("userspace_liteapi_rdma_read: %d\n",
             userspace_liteapi_rdma_read(2233, (void *)0x1ff, 2048, 128, 54321));
    LOG_INFO("userspace_liteapi_send_reply_imm_fast: %d\n",
             userspace_liteapi_send_reply_imm_fast(2, 1, (void *)0x100, 2048, (void *)0x200, (int *)0x300, 4096));
    uintptr_t descriptor = 0;
    int ret_length = 0;
    val = userspace_liteapi_receive_message_fast(1, (void *)0x100, 1024, &descriptor, &ret_length, 1);
    LOG_INFO("userspace_liteapi_receive_message_fast: %d, %lu, %d\n", val, descriptor, ret_length);
    LOG_INFO("userspace_liteapi_reply_message: %d\n",
             userspace_liteapi_reply_message((void *)0xf88, 128, 321));
    LOG_INFO("userspace_liteapi_register_application: %d\n",
             userspace_liteapi_register_application(1, 256, 4, "hello world", strlen("hello world")));
    LOG_INFO("userspace_liteapi_dist_barrier: %d\n",
             userspace_liteapi_dist_barrier(16));
    LOG_INFO("userspace_liteapi_query_port: %d\n",
             userspace_liteapi_query_port(2, 1));
    void *local_addr = NULL, *remote_addr = NULL;
    val = userspace_liteapi_alloc_local_mem_test("SHMALLOC", 1024, &local_addr, &remote_addr);
    LOG_INFO("userspace_liteapi_alloc_local_mem_test: %d, %p, %p\n", val, local_addr, remote_addr);
    LOG_INFO("userspace_liteapi_free_local_mem: %d\n",
             userspace_liteapi_free_local_mem("SHMFREE", 2048, local_addr, remote_addr));
    return 0;
}
