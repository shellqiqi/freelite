# Freelite

## 运行Router

启动cluster manager，挂载LITE内核模块

创建文件`seccomp_allow_all.json`允许所有系统调用，生产环境中不推荐这样做

```
{
    "defaultAction": "SCMP_ACT_ALLOW"
}
```

启动容器

容器间通信使用Unix domain socket，因此容器需要映射`/var/tmp/`  
Router需要host模式的网络环境  
容器使用shm让Client与Router共享内存，因此容器需要挂在`/dev/shm/`

```
sudo docker run --name lite_router --security-opt seccomp=path_to/seccomp_allow_all.json --net host -v /var/tmp/:/var/tmp/ -v /dev/shm/:/dev/shm/ -it ubuntu:18.04 /bin/bash
```

容器内运行freelite router

## 运行Client

```
sudo docker run --name lite_client_0 -v /var/tmp/:/var/tmp/ -v /dev/shm/:/dev/shm/ -it ubuntu:18.04 /bin/bash
```

容器内运行freelite应用程序

## 问题

### Router与用户程序的内存管理

添加函数`userspace_liteapi_alloc_local_mem`，将用户需要的内存与Router分配的内存建立映射

### 参数传递问题

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
