# LITE容器内运行环境搭建

首先完成[LITE运行环境搭建](./LITE运行环境搭建.md)

## 安装Docker

在Ubuntu LTS发行版本参照 https://docs.docker.com/install/linux/docker-ce/ubuntu/ 安装。

在非LTS发行下直接获取可执行文件安装 https://mirrors.tuna.tsinghua.edu.cn/docker-ce/linux/static/stable/x86_64/docker-18.09.4.tgz

解压后将所有可执行文件复制进`/usr/bin/`下

## 启动Docker

```sh
sudo su
dockerd > /var/log/dockerd.log 2>&1 &
exit
```

## 启用容器内系统调用

这个文件默认启用所有的系统调用，保存为`seccomp_allow_all.json`

```json
{
    "defaultAction": "SCMP_ACT_ALLOW"
}
```

## 测试环境

启动cluster manager，挂载LITE内核模块。

```
sudo docker run --name lite_test --security-opt seccomp=/home/tian/seccomp_allow_all.json --net host -it ubuntu:18.04 /bin/bash
```

将`LITE/user`下的可执行文件复制进容器内测试运行。

## 参考

1. https://docs.docker.com/install/linux/docker-ce/binaries/
2. https://docs.docker.com/v17.09/engine/security/seccomp/
