#ifndef LITE_LIB_H
#define LITE_LIB_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <linux/unistd.h>
#include <asm/unistd.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>

#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <time.h>

#define __NR_lite_remote_memset 400
#define __NR_lite_fetch_add 401
#define __NR_lite_rdma_synwrite 402
#define __NR_lite_rdma_read 403
#define __NR_lite_ask_lmr 404
#define __NR_lite_rdma_asywrite 405
#define __NR_lite_dist_barrier 406
#define __NR_lite_add_ask_mr_table 407
#define __NR_lite_compare_swp 408
#define __NR_lite_alloc_remote 409
#define __NR_lite_umap_lmr 410
#define __NR_lite_register_application 411
#define __NR_lite_unregister_application 412
#define __NR_lite_receive_message 413
#define __NR_lite_send_reply_imm 414
#define __NR_lite_reply_message 415
#define __NR_lite_get_node_id 416
#define __NR_lite_query_port 417
#define __NR_lite_alloc_continuous_memory 418
#define __NR_lite_wrap_alloc_for_remote_access 419
#define __NR_lite_create_lock 420
#define __NR_lite_ask_lock 421
#define __NR_lite_lock 422
#define __NR_lite_unlock 423
#define __NR_lite_get_total_node 424
#define __NR_lite_reply_and_receive_message 425
#define __NR_lite_join 426
#define __NR_lite_rdma_read_async 427

#define max(x, y)      \
    ({                 \
        x > y ? x : y; \
    })

struct lmr_info
{
    void *addr;
    size_t length;
    uint32_t lkey;
    uint32_t rkey;
    uint32_t node_id;
};

struct lite_lock_form
{
    int lock_num;
    struct lmr_info lock_mr;
    uint64_t ticket_num;
};

typedef struct lite_lock_form remote_spinlock_t;

struct reply_struct
{
    void *addr;
    int size;
    uintptr_t descriptor;
};

struct receive_struct
{
    unsigned int designed_port;
    void *ret_addr;
    int receive_size;
    void *descriptor;
    int block_call;
};

enum permission_mode
{
    MR_READ_FLAG = 0x01,
    MR_WRITE_FLAG = 0x02,
    MR_SHARE_FLAG = 0x04,
    MR_ADMIN_FLAG = 0x08,
    MR_ATOMIC_FLAG = 0x10,
    MR_ASK_SUCCESS = 0,
    MR_ASK_REFUSE = 1,
    MR_ASK_UNPERMITTED = 2,
    MR_ASK_HANDLER_ERROR = 3,
    MR_ASK_UNKNOWN = 4
};

#define IMM_SEND_ONLY_FLAG 0xffffffffffffffff

#define __ACTIVE_NODES 3
#define LIMITATION 1024 * 1024 * 4
#define PAGE_SHIFT 12

#define IMM_MAX_PORT 64
#define IMM_MAX_PORT_BIT 6
#define IMM_MAX_PRIORITY 64
#define IMM_MAX_PRIORITY_BIT 6

#define SEND_REPLY_WAIT (-101)

#define CHECK_LENGTH 100000

#define USERSPACE_HIGH_PRIORITY 16
#define USERSPACE_LOW_PRIORITY 17
#define NULL_PRIORITY 0

#define BLOCK_CALL 1

int userspace_liteapi_get_node_id(void);
int userspace_liteapi_get_total_node(void);
int userspace_liteapi_dist_barrier(unsigned int num);
int userspace_liteapi_register_application(unsigned int destined_port,
                                           unsigned int max_size_per_message,
                                           unsigned int max_user_per_node,
                                           char *name,
                                           uint64_t name_len);
int userspace_liteapi_receive_message(unsigned int port,
                                      void *ret_addr,
                                      int receive_size,
                                      uintptr_t *descriptor,
                                      int block_call);
int userspace_liteapi_receive_message_high(unsigned int port,
                                           void *ret_addr,
                                           int receive_size,
                                           uintptr_t *descriptor,
                                           int block_call);
int userspace_liteapi_receive_message_low(unsigned int port,
                                          void *ret_addr,
                                          int receive_size,
                                          uintptr_t *descriptor,
                                          int block_call);
int userspace_liteapi_receive_message_fast(unsigned int port,
                                           void *ret_addr,
                                           int receive_size,
                                           uintptr_t *descriptor,
                                           int *ret_length,
                                           int block_call);
double userspace_liteapi_receive_message_fast_record(unsigned int port,
                                                     void *ret_addr,
                                                     int receive_size,
                                                     uintptr_t *descriptor,
                                                     int *ret_length,
                                                     int block_call);
int userspace_liteapi_send_reply_imm(int target_node,
                                     unsigned int port,
                                     void *addr,
                                     int size,
                                     void *ret_addr,
                                     int max_ret_size);
int userspace_liteapi_send_reply_imm_high(int target_node,
                                          unsigned int port,
                                          void *addr,
                                          int size,
                                          void *ret_addr,
                                          int max_ret_size);
int userspace_liteapi_send_reply_imm_low(int target_node,
                                         unsigned int port,
                                         void *addr,
                                         int size,
                                         void *ret_addr,
                                         int max_ret_size);
int userspace_liteapi_send_reply_imm_fast(int target_node,
                                          unsigned int port,
                                          void *addr,
                                          int size,
                                          void *ret_addr,
                                          int *ret_length,
                                          int max_ret_size);
int userspace_liteapi_reply_message(void *addr,
                                    int size,
                                    uintptr_t descriptor);
int userspace_liteapi_reply_message_high(void *addr,
                                         int size,
                                         uintptr_t descriptor);
int userspace_liteapi_reply_message_low(void *addr,
                                        int size,
                                        uintptr_t descriptor);
int userspace_liteapi_reply_and_receive_message(void *addr,
                                                int size,
                                                uintptr_t descriptor,
                                                unsigned int port,
                                                void *ret_addr,
                                                int receive_size,
                                                uintptr_t *receive_descriptor);
int userspace_liteapi_query_port(int target_node,
                                 int designed_port);
int userspace_liteapi_wrap_alloc(void *data,
                                 int size,
                                 uint64_t identifier,
                                 int password);
int userspace_liteapi_ask_lmr(int memory_node,
                              uint64_t identifier,
                              uint64_t permission,
                              int password);
int userspace_liteapi_rdma_read(unsigned lite_handler,
                                void *local_addr,
                                unsigned int size,
                                unsigned int offset,
                                int password);
int userspace_liteapi_rdma_read_high(unsigned lite_handler,
                                     void *local_addr,
                                     unsigned int size,
                                     unsigned int offset,
                                     int password);
int userspace_liteapi_rdma_read_low(unsigned lite_handler,
                                    void *local_addr,
                                    unsigned int size,
                                    unsigned int offset,
                                    int password);
int userspace_liteapi_rdma_write(unsigned lite_handler,
                                 void *local_addr,
                                 unsigned int size,
                                 unsigned int offset,
                                 int password);
int userspace_liteapi_rdma_write_high(unsigned lite_handler,
                                      void *local_addr,
                                      unsigned int size,
                                      unsigned int offset,
                                      int password);
int userspace_liteapi_rdma_write_low(unsigned lite_handler,
                                     void *local_addr,
                                     unsigned int size,
                                     unsigned int offset,
                                     int password);
void *userspace_liteapi_alloc_memory(unsigned long size);
int userspace_liteapi_create_lock(int target_node,
                                  remote_spinlock_t *input);
int userspace_liteapi_ask_lock(int target_node,
                               int target_idx,
                               remote_spinlock_t *input);
int userspace_liteapi_lock(remote_spinlock_t *input);
int userspace_liteapi_unlock(remote_spinlock_t *input);
int userspace_liteapi_memset(unsigned lite_handler,
                             int offset,
                             int size);
int userspace_liteapi_alloc_remote_mem(unsigned int node_id,
                                       unsigned int size,
                                       bool atomic_flag,
                                       int password);
int userspace_liteapi_compare_swp(unsigned long lite_handler,
                                  void *local_addr,
                                  unsigned long long guess_value,
                                  unsigned long long set_value);
int userspace_liteapi_add_ask_mr_table(uint64_t identifier,
                                       uint64_t lmr,
                                       uint64_t permission,
                                       int password);
int userspace_liteapi_remote_memset(unsigned lite_handler,
                                    int offset,
                                    int size);
int userspace_liteapi_fetch_add(unsigned long lite_handler,
                                void *local_addr,
                                unsigned long long input_value);
int userspace_syscall_test(void);
int userspace_liteapi_join(char *input_str,
                           int eth_port,
                           int ib_port);
int stick_this_thread_to_core(int core_id);
int userspace_liteapi_send(int target_node,
                           unsigned int port,
                           void *addr,
                           int size);

int userspace_liteapi_alloc_local_mem(const char *name,
                                      const size_t size,
                                      void **local_addr,
                                      void **remote_addr);
int userspace_liteapi_free_local_mem(const char *name,
                                     size_t size,
                                     void *local_addr,
                                     void *remote_addr);

int userspace_liteapi_alloc_local_mem_test(const char *name,
                                           const size_t size,
                                           void **local_addr,
                                           void **remote_addr);

#endif /* LITE_LIB_H */
