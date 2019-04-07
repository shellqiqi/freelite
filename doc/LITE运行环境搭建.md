# LITE运行环境搭建

由于原版LITE基于的`linux 3.11.1`不能在当前硬件环境中启动, 并且不支持ConnectX-5网卡,
因此我们采用[lastweek/LITE](https://github.com/lastweek/LITE), 该版本基于`linux 4.9.103`.

## Requirements

- OS: Ubuntu 17.04
- RNIC: ConnectX-5

## apt源配置

由于Ubuntu 17.04已停止支持, 我们需要使用以下命令更换apt源, 然后运行`sudo apt update`使之生效.

```sh
sudo sed -i 's/archive.ubuntu.com/mirrors.nju.edu.cn\/ubuntu-old-releases/g' /etc/apt/sources.list
```

## 内核编译

1. 通过`apt`安装内核构建工具:

```sh
sudo apt install git build-essential libncurses5-dev bc libssl-dev
```

2. 下载并解压内核源码:

```sh
wget https://mirrors.nju.edu.cn/kernel/linux/kernel/v4.x/linux-4.9.103.tar.xz
sudo tar xvf linux-4.9.103.tar.xz -C /usr/src
sudo mv /usr/src/linux-4.9.103 /usr/src/linux-4.9.103-lite
```

3. 打内核补丁:

使用`git`下载LITE源码, 并运行`patch`打补丁.

```sh
cd ~/Projects
git clone https://github.com/lastweek/LITE.git
cd /usr/src/linux-4.9.103-lite
sudo patch -p1 ~/Projects/LITE/lite_kernel_patch_4.9.103
```

4. 调整内核编译配置:

在Ubuntu 17.04提供的配置基础上进行修改, 并调整以下两个选项:

- `CONFIG_LOCALVERSION="-lite"`
- turn off `CONFIG_VMAP_STACK`

```sh
cd /usr/src/linux-4.9.103-lite
sudo cp /boot/config-4.10.0-42-generic .config
sudo make olddefconfig
```

5. 编译并安装内核:

```sh
sudo make -j96 bzImage
sudo make -j96 modules
sudo make modules_install
sudo make install
```

## 加载RDMA内核模块

由于Ubuntu 17.04并不会自动加载RDMA相关的内核模块,
因此我们需要修改配置, 使系统在启动时自动加载相应的内核模块.

```sh
cat <<EOF > /etc/modules-load.d/rdma.conf
mlx5_ib
ib_uverbs
ib_umad
ib_ucm
rdma_ucm
EOF
```

若在`/dev/infiniband/`目录下能看到`rdma_cm`, `uverbs0`等设备, 说明内核模块已经成功加载.

## 安装userspace driver

Ubuntu 17.04的源中的`libmlx5-1`支持ConnectX-5网卡, 因此可以直接通过`apt`安装userspace driver和util:

```sh
sudo apt install libibverbs-dev librdmacm-dev libmlx5-dev libibumad-dev libibcm-dev ibverbs-utils perftest
```

Ubuntu 16.04需要从[linux-rdma/rdma-core](https://github.com/linux-rdma/rdma-core)获取最新版本的`rdma-core`, 并根据README编译然后手动安装.

## 参考

1. [https://wiki.ubuntu.com/Kernel/BuildYourOwnKernel](https://wiki.ubuntu.com/Kernel/BuildYourOwnKernel)
2. [https://wiki.ubuntu.com/Kernel/Dev/KernelGitGuide](https://wiki.ubuntu.com/Kernel/Dev/KernelGitGuide)
3. [https://github.com/Mellanox/libvma/wiki/VMA-over-Ubuntu-16.04-and-inbox-driver](https://github.com/Mellanox/libvma/wiki/VMA-over-Ubuntu-16.04-and-inbox-driver)
4. [http://www.mellanox.com/page/inbox\_drivers](http://www.mellanox.com/page/inbox_drivers)
