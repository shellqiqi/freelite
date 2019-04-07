# Freelite

## 容器间通信

需要映射`/var/tmp/`

```
sudo docker run --name lite_client_0 -v /var/tmp/:/var/tmp/ -it ubuntu:18.04 /bin/bash
```

## 参数传递问题

以下三个函数的参数意义不明确，特别是第二个函数参数中带有`uintptr_t *descriptor`，第三个中有`uintptr_t descriptor`

```c
int userspace_liteapi_send_reply_imm_fast(int target_node,
                                          unsigned int port,
                                          void *addr,
                                          int size,
                                          void *ret_addr,
                                          int *ret_length,
                                          int max_ret_size);
int userspace_liteapi_receive_message_fast(unsigned int port,
                                           void *ret_addr,
                                           int receive_size,
                                           uintptr_t *descriptor,
                                           int *ret_length,
                                           int block_call);
int userspace_liteapi_reply_message(void *addr,
                                    int size,
                                    uintptr_t descriptor);
```

在`lite_core.c`中`userspace_liteapi_send_reply_imm_fast`有强制类型转换

```c
struct imm_message_metadata *descriptor;
*reply_descriptor = (uintptr_t) descriptor;
```

需要进一步确定这三个函数的变量是否是输出型
