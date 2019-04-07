#include "../../inc/lite-lib.h"

/* liteapi_dist_barrier */
inline int userspace_liteapi_dist_barrier(unsigned int num)
{
    return syscall(__NR_lite_dist_barrier, num);
}

/* liteapi_alloc_remote_mem */
inline int userspace_liteapi_alloc_remote_mem(unsigned int node_id,
                                              unsigned int size,
                                              bool atomic_flag,
                                              int password)
{
    return syscall(__NR_lite_alloc_remote, node_id, size, atomic_flag, password);
}

/* liteapi_register_application */
inline int userspace_liteapi_register_application(unsigned int destined_port,
                                                  unsigned int max_size_per_message,
                                                  unsigned int max_user_per_node,
                                                  char *name,
                                                  uint64_t name_len)
{
    return syscall(__NR_lite_register_application,
                   destined_port,
                   max_size_per_message,
                   max_user_per_node,
                   name,
                   name_len);
}

/* liteapi_receive_message_userspace */
inline int userspace_liteapi_receive_message(unsigned int port,
                                             void *ret_addr,
                                             int receive_size,
                                             uintptr_t *descriptor,
                                             int block_call)
{
    int ret;
    ret = syscall(__NR_lite_receive_message,
                  (receive_size << IMM_MAX_PORT_BIT) + port,
                  ret_addr,
                  descriptor,
                  0,
                  block_call,
                  NULL_PRIORITY);
    return ret;
}

/* liteapi_receive_message_userspace */
inline int userspace_liteapi_receive_message_high(unsigned int port,
                                                  void *ret_addr,
                                                  int receive_size,
                                                  uintptr_t *descriptor,
                                                  int block_call)
{
    int ret;
    ret = syscall(__NR_lite_receive_message,
                  (receive_size << IMM_MAX_PORT_BIT) + port,
                  ret_addr,
                  descriptor,
                  0,
                  block_call,
                  USERSPACE_HIGH_PRIORITY);
    return ret;
}

/* liteapi_receive_message_userspace */
inline int userspace_liteapi_receive_message_low(unsigned int port,
                                                 void *ret_addr,
                                                 int receive_size,
                                                 uintptr_t *descriptor,
                                                 int block_call)
{
    int ret;
    ret = syscall(__NR_lite_receive_message,
                  (receive_size << IMM_MAX_PORT_BIT) + port,
                  ret_addr,
                  descriptor,
                  0,
                  block_call,
                  USERSPACE_LOW_PRIORITY);
    return ret;
}

/* liteapi_receive_message_userspace */
int userspace_liteapi_receive_message_fast(unsigned int port,
                                           void *ret_addr,
                                           int receive_size,
                                           uintptr_t *descriptor,
                                           int *ret_length,
                                           int block_call)
{
    int ret;
    int i = 0;

    *descriptor = 0;

    ret = syscall(__NR_lite_receive_message,
                  (receive_size << IMM_MAX_PORT_BIT) + port,
                  ret_addr,
                  descriptor,
                  ret_length,
                  block_call,
                  NULL_PRIORITY);

    while (*(descriptor) == 0 && ++i < CHECK_LENGTH)
        ;

    while (*(descriptor) == 0)
        usleep(1);

    if (*(descriptor) == IMM_SEND_ONLY_FLAG)
        *(descriptor) = 0;

    if (ret < 0)
        return *ret_length;
    return ret;
}

/* liteapi_receive_message_userspace */
int record_flag = 0;
inline double userspace_liteapi_receive_message_fast_record(unsigned int port,
                                                            void *ret_addr,
                                                            int receive_size,
                                                            uintptr_t *descriptor,
                                                            int *ret_length,
                                                            int block_call)
{
    int ret;
    userspace_liteapi_receive_message_fast(port,
                                           ret_addr,
                                           receive_size,
                                           descriptor,
                                           ret_length,
                                           block_call);
    return ret;
}

/* liteapi_send_reply_imm_userspace */
int userspace_liteapi_send(int target_node, unsigned int port, void *addr, int size)
{
    if (size >= LIMITATION)
    {
        printf("%s: size %d too big\n", __func__, size);
        return -1;
    }
    return syscall(__NR_lite_send_reply_imm,
                   target_node,
                   (size << IMM_MAX_PORT_BIT) + port,
                   addr,
                   0,
                   0,
                   0);
}

/* liteapi_send_reply_imm_userspace */
int userspace_liteapi_send_reply_imm(int target_node,
                                     unsigned int port,
                                     void *addr,
                                     int size,
                                     void *ret_addr,
                                     int max_ret_size)
{
    if (size >= LIMITATION || max_ret_size >= LIMITATION)
    {
        printf("%s: size %d max_ret_size %d too big\n", __func__, size, max_ret_size);
        return -1;
    }
    return syscall(__NR_lite_send_reply_imm,
                   target_node,
                   (size << IMM_MAX_PORT_BIT) + port,
                   addr,
                   ret_addr,
                   0,
                   max_ret_size);
}

/* liteapi_send_reply_imm_userspace */
int userspace_liteapi_send_reply_imm_high(int target_node,
                                          unsigned int port,
                                          void *addr,
                                          int size,
                                          void *ret_addr,
                                          int max_ret_size)
{
    if (size >= LIMITATION || max_ret_size >= LIMITATION)
    {
        printf("%s: size %d max_ret_size %d too big\n", __func__, size, max_ret_size);
        return -1;
    }
    return syscall(__NR_lite_send_reply_imm,
                   target_node,
                   (size << IMM_MAX_PORT_BIT) + port,
                   addr,
                   ret_addr,
                   0,
                   (max_ret_size << IMM_MAX_PRIORITY_BIT) + USERSPACE_HIGH_PRIORITY);
}

/* liteapi_send_reply_imm_userspace */
int userspace_liteapi_send_reply_imm_low(int target_node,
                                         unsigned int port,
                                         void *addr,
                                         int size,
                                         void *ret_addr,
                                         int max_ret_size)
{
    if (size >= LIMITATION || max_ret_size >= LIMITATION)
    {
        printf("%s: size %d max_ret_size %d too big\n", __func__, size, max_ret_size);
        return -1;
    }
    return syscall(__NR_lite_send_reply_imm,
                   target_node,
                   (size << IMM_MAX_PORT_BIT) + port,
                   addr,
                   ret_addr,
                   0,
                   (max_ret_size << IMM_MAX_PRIORITY_BIT) + USERSPACE_LOW_PRIORITY);
}

/*
 * This is the version with RPC syscall optimizations.
 */
/* liteapi_send_reply_imm_userspace */
int userspace_liteapi_send_reply_imm_fast(int target_node,
                                          unsigned int port,
                                          void *addr,
                                          int size,
                                          void *ret_addr,
                                          int *ret_length,
                                          int max_ret_size)
{
    int ret;

    if (size >= LIMITATION || max_ret_size >= LIMITATION)
    {
        printf("%s: size %d max_ret_size %d too big\n", __func__, size, max_ret_size);
        return -1;
    }

    ret = syscall(__NR_lite_send_reply_imm,
                  target_node,
                  (size << IMM_MAX_PORT_BIT) + port,
                  addr,
                  ret_addr,
                  ret_length,
                  (max_ret_size << IMM_MAX_PRIORITY_BIT) + NULL_PRIORITY);
    if (ret < 0)
        printf("[significant error] error in fast send setup %d\n", ret);

    /*
	 * This is where userspace poll when a RPC finished.
	 */
    while (*ret_length == SEND_REPLY_WAIT)
        ;

    if (*ret_length < 0)
        printf("[significant error] error in fast send %d\n", *ret_length);
    return *ret_length;
}

/* liteapi_reply_message_userspace */
int userspace_liteapi_reply_message(void *addr, int size, uintptr_t descriptor)
{
    if (size >= LIMITATION)
    {
        printf("%s: size %d too big\n", __func__, size);
        return -1;
    }
    return syscall(__NR_lite_reply_message, addr, size, descriptor, NULL_PRIORITY);
}

/* liteapi_reply_message_userspace */
int userspace_liteapi_reply_message_high(void *addr, int size, uintptr_t descriptor)
{
    if (size >= LIMITATION)
    {
        printf("%s: size %d too big\n", __func__, size);
        return -1;
    }
    return syscall(__NR_lite_reply_message, addr, size, descriptor, USERSPACE_HIGH_PRIORITY);
}

/* liteapi_reply_message_userspace */
int userspace_liteapi_reply_message_low(void *addr, int size, uintptr_t descriptor)
{
    if (size >= LIMITATION)
    {
        printf("%s: size %d too big\n", __func__, size);
        return -1;
    }
    return syscall(__NR_lite_reply_message, addr, size, descriptor, USERSPACE_LOW_PRIORITY);
}

/* liteapi_query_port */
inline int userspace_liteapi_query_port(int target_node, int designed_port)
{
    return syscall(__NR_lite_query_port, target_node, designed_port, 0);
}

/* liteapi_ask_mr */
inline int userspace_liteapi_ask_lmr(int memory_node,
                                     uint64_t identifier,
                                     uint64_t permission,
                                     int password)
{
    return syscall(__NR_lite_ask_lmr, memory_node, identifier, permission, password);
}

/* liteapi_get_node_id */
int userspace_liteapi_get_node_id(void)
{
    return syscall(__NR_lite_get_node_id);
}

/* liteapi_get_total_node */
inline int userspace_liteapi_get_total_node(void)
{
    return syscall(__NR_lite_get_total_node);
}

/* liteapi_rdma_write_offset_userspace */
inline int userspace_liteapi_rdma_write(unsigned lite_handler,
                                        void *local_addr,
                                        unsigned int size,
                                        unsigned int offset,
                                        int password)
{
    return syscall(__NR_lite_rdma_synwrite,
                   lite_handler,
                   local_addr,
                   size,
                   NULL_PRIORITY,
                   offset,
                   password);
}

/* liteapi_rdma_write_offset_userspace */
inline int userspace_liteapi_rdma_write_high(unsigned lite_handler,
                                             void *local_addr,
                                             unsigned int size,
                                             unsigned int offset,
                                             int password)
{
    return syscall(__NR_lite_rdma_synwrite,
                   lite_handler,
                   local_addr,
                   size,
                   USERSPACE_HIGH_PRIORITY,
                   offset,
                   password);
}

/* liteapi_rdma_write_offset_userspace */
inline int userspace_liteapi_rdma_write_low(unsigned lite_handler,
                                            void *local_addr,
                                            unsigned int size,
                                            unsigned int offset,
                                            int password)
{
    return syscall(__NR_lite_rdma_synwrite,
                   lite_handler,
                   local_addr,
                   size,
                   USERSPACE_LOW_PRIORITY,
                   offset,
                   password);
}

/* liteapi_rdma_read_offset_userspace */
inline int userspace_liteapi_rdma_read(unsigned lite_handler,
                                       void *local_addr,
                                       unsigned int size,
                                       unsigned int offset,
                                       int password)
{
    return syscall(__NR_lite_rdma_read,
                   lite_handler,
                   local_addr,
                   size,
                   NULL_PRIORITY,
                   offset,
                   password);
}

/* liteapi_rdma_read_offset_userspace */
inline int userspace_liteapi_rdma_read_high(unsigned lite_handler,
                                            void *local_addr,
                                            unsigned int size,
                                            unsigned int offset,
                                            int password)
{
    return syscall(__NR_lite_rdma_read,
                   lite_handler,
                   local_addr,
                   size,
                   USERSPACE_HIGH_PRIORITY,
                   offset,
                   password);
}

/* liteapi_rdma_read_offset_userspace */
inline int userspace_liteapi_rdma_read_low(unsigned lite_handler,
                                           void *local_addr,
                                           unsigned int size,
                                           unsigned int offset,
                                           int password)
{
    return syscall(__NR_lite_rdma_read,
                   lite_handler,
                   local_addr,
                   size,
                   USERSPACE_LOW_PRIORITY,
                   offset,
                   password);
}

/* liteapi_rdma_read_offset_userspace_async */
int async_rdma_read(unsigned lite_handler,
                    void *local_addr,
                    unsigned int size,
                    unsigned int offset,
                    int *poll)
{
    return syscall(__NR_lite_rdma_read_async,
                   lite_handler,
                   local_addr,
                   size,
                   NULL_PRIORITY,
                   offset,
                   poll);
}

void *userspace_liteapi_alloc_memory(unsigned long size)
{
    unsigned long roundup_size = (((1 << PAGE_SHIFT) + size - 1) >> PAGE_SHIFT) << PAGE_SHIFT;

    int fd;
    char *addr;
    fd = open("/dev/lite_mmap", O_RDONLY);

    addr = (char *)mmap(NULL, roundup_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED)
    {
        perror("mmaptest user ");
        return 0;
    }
    //char *addr=malloc(roundup_size);
    //syscall(__NR_lite_alloc_memory, addr, size);
    return addr;
}

/* liteapi_create_lock */
inline int userspace_liteapi_create_lock(int target_node,
                                         remote_spinlock_t *input)
{
    int ret;
    ret = syscall(__NR_lite_create_lock, target_node, input);
    if (ret >= 0)
        return ret;
    else
        printf("create lock error\n");
    return 0;
}

/* liteapi_ask_lock */
inline int userspace_liteapi_ask_lock(int target_node,
                                      int target_idx,
                                      remote_spinlock_t *input)
{
    int ret;
    ret = syscall(__NR_lite_ask_lock, target_node, target_idx, input);
    if (ret >= 0)
        return ret;
    else
        printf("ask lock error\n");
    return 0;
}

/* liteapi_lock */
inline int userspace_liteapi_lock(remote_spinlock_t *input)
{
    return syscall(__NR_lite_lock, input);
}

/* liteapi_unlock */
inline int userspace_liteapi_unlock(remote_spinlock_t *input)
{
    return syscall(__NR_lite_unlock, input);
}

/* liteapi_remote_memset */
inline int userspace_liteapi_remote_memset(unsigned lite_handler, int offset, int size)
{
    return syscall(__NR_lite_remote_memset, lite_handler, offset, size);
}

/* liteapi_add_askmr_table */
inline int userspace_liteapi_add_ask_mr_table(uint64_t identifier,
                                              uint64_t lmr,
                                              uint64_t permission,
                                              int password)
{
    return syscall(__NR_lite_add_ask_mr_table, identifier, lmr, permission, password);
}

/* liteapi_rdma_compare_and_swp */
inline int userspace_liteapi_compare_swp(unsigned long lite_handler,
                                         void *local_addr,
                                         unsigned long long guess_value,
                                         unsigned long long set_value)
{
    return syscall(__NR_lite_compare_swp, lite_handler, local_addr, guess_value, set_value, 0);
}

/* liteapi_rdma_fetch_and_add */
inline int userspace_liteapi_fetch_add(unsigned long lite_handler,
                                       void *local_addr,
                                       unsigned long long input_value)
{
    return syscall(__NR_lite_fetch_add, lite_handler, local_addr, input_value, 0);
}

/* liteapi_reply_and_receive_message_userspace */
inline int userspace_liteapi_reply_and_receive_message(void *addr,
                                                       int size,
                                                       uintptr_t descriptor,
                                                       unsigned int port,
                                                       void *ret_addr,
                                                       int receive_size,
                                                       uintptr_t *receive_descriptor)
{
    return syscall(__NR_lite_reply_and_receive_message,
                   addr,
                   size * IMM_MAX_PORT + port,
                   descriptor, ret_addr,
                   receive_size,
                   receive_descriptor);
}

/* liteapi_establish_conn */
int userspace_liteapi_join(char *input_str, int eth_port, int ib_port)
{
    char ipstr[32];
    memset(ipstr, 0, 32);
    strcpy(ipstr, input_str);
    return syscall(__NR_lite_join, ipstr, eth_port, ib_port);
}
