# Centos LITE 内核修改

## 构建工具

```sh
yum groupinstall "Development Tools"
yum install asciidoc audit-libs-devel bash bc binutils binutils-devel bison curl diffutils elfutils elfutils-devel elfutils-libelf-devel findutils flex gawk gcc gettext gzip hmaccalc hostname java-devel m4 make module-init-tools mock ncurses-devel net-tools newt-devel numactl-devel openssl openssl-devel patch pciutils-devel perl perl-ExtUtils-Embed pesign python-devel python-docutils redhat-rpm-config rpm-build sh-utils tar vim wget xmlto xz zlib-devel
```

## 完整的内核源码

切换至`~`，找到需要的内核源码包，安装。例如：

```sh
wget https://mirrors.tuna.tsinghua.edu.cn/centos-vault/7.1.1503/os/Source/SPackages/kernel-3.10.0-229.el7.src.rpm
wget https://mirrors.nju.edu.cn/centos-vault/7.2.1511/os/Source/SPackages/kernel-3.10.0-327.el7.src.rpm
rpm -i kernel-3.10.0-229.el7.src.rpm
```

源码展开至`~/rpmbuild`下。

## 修改源码

在`~/rpmbuild/SOURCES`下修改源码，在`~/rpmbuild/SPECS`下修改包的相关信息。

如果不修改`SPECS`包版本，相同内核包将无法安装，修改后有可能出现版本降低的可能。

CentOS-7.6下用229、327无法启动，957内核的`ib_verbs.h`头文件与代码不匹配。

CentOS-7.1下用229无法启动。

229和327都需要根据 https://lore.kernel.org/patchwork/patch/850380/ 来修改perl相关的代码。

## 构建RPM包

在`~/rpmbuild/SPECS`下构建。

```sh
rpmbuild -bb --target=`uname -m` kernel.spec | tee ~/build-out.log
```

## 安装RPM包

一般只需要安装kernel和kernel-dev即可。若报告版本较旧则尝试选项`--oldpackage`，用于安装旧版本的包。`--force`用于强制替换同版本，最好不用。

```sh
rpm -ivh kernel-*.rpm
```

## 参考

1. https://wiki.centos.org/zh/HowTos/Custom_Kernel
2. https://wiki.centos.org/HowTos/I_need_the_Kernel_Source
