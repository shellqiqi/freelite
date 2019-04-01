#ifndef LITE_LIB_H
#define LITE_LIB_H

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <linux/unistd.h>

#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <time.h>

#include "../inc/rpc_types.h"
#include "../inc/func_code.h"

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

#endif /* LITE_LIB_H */
