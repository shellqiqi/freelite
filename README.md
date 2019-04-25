# Freelite

## LITE内核安装

参考项目内LITE运行环境搭建[文档](doc/LITE运行环境搭建.md)

## 编译

安装cmake, C/C++编译器。在项目根目录下新建build文件夹，并在build下执行

```
cmake ..
make
```

## 项目硬件环境示例

三台服务器，其中两台服务器上拥有IB网卡。没有IB网卡的服务器上启动vcm和LITE cluster manager。有网卡的两台服务器上启动容器并挂载LITE内核层，且在RDMA程序启动前启动Router。

搭建环境时请修改硬编码的IP地址，主要是vcm的IP地址在`router/test_router.cpp`，LITE cluster manager的IP地址在`client/`下所有`lite_join`函数调用。

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

### Router与用户程序的内存管理（已解决）

添加函数`userspace_liteapi_alloc_local_mem`，将用户需要的内存与Router分配的内存建立映射

### 参数传递问题（已解决）

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
                                           uintptr_t *descriptor, // Output
                                           int *ret_length,       // Output
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

### lite_rpc.c的运行问题

该问题继承自原项目

> Its RPC server must accept client at **nid 1**.
> Need more tests.
