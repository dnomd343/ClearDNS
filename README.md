# ClearDNS

> 容器化的无污染DNS服务，同时兼具DNS层面的广告拦截与防跟踪功能

## 部署教程

Docker可以选择两种部署模式，bridge模式与macvlan模式，前者通过端口映射和外界联通，占用宿主机53端口，后者拥有自己独立的IP地址，与宿主机分离。

使用CleatDNS时，需要在路由器DHCP服务里选定DNS服务器，对于网桥模式，指定为宿主机IP，而macvlan模式则指定为容器IP地址。

### 网桥模式

运行容器

```
shell> docker run --restart always \
--name cleardns -d \
-v /etc/cleardns/:/etc/cleardns/ \
-v /etc/timezone:/etc/timezone:ro \
-v /etc/localtime:/etc/localtime:ro \
-p 53:53/udp -p 53:53 -p 80:80 \
dnomd343/cleardns
```

### macvlan模式

启动容器之前需要创建一个macvlan网络

```
# 开启网卡混杂模式
shell> ip link set eth0 promisc on

# 创建macvlan网络，按实际情况指定网络信息
shell> docker network create -d macvlan \
--subnet=192.168.2.0/24 \
--gateway=192.168.2.1 \
-o parent=eth0 macvlan
```

宿主机网络更改配置

```
# Debian系示例
shell> vim /etc/network/interfaces

# 添加以下内容，按实际情况指定网络信息
auto eth0
iface eth0 inet manual

auto macvlan
iface macvlan inet static
  address 192.168.2.34
  netmask 255.255.255.0
  gateway 192.168.2.2
  dns-nameservers 192.168.2.3
  pre-up ip link add macvlan link eth0 type macvlan mode bridge
  post-down ip link del macvlan link eth0 type macvlan mode bridge

# 重启宿主机
shell> reboot
```

运行容器

```
shell> docker run --restart always \
--name cleardns \
--network macvlan \
--privileged -d \
-v /etc/cleardns/:/etc/cleardns/ \
-v /etc/timezone:/etc/timezone:ro \
-v /etc/localtime:/etc/localtime:ro \
dnomd343/cleardns
```

编辑macvlan网路的IP地址

```
# 编辑自定义初始化脚本
shell> vim /etc/cleardns/custom.sh

# 写入以下内容，修改容器的IP与网关，按实际网络情况指定
ip addr flush dev eth0
ip addr add 192.168.2.3/24 brd 192.168.2.255 dev eth0
ip route add default via 192.168.2.2

# 重启容器生效
shell> docker restart cleardns
```

## 初始化服务

### 指定上游DNS服务器

上游DNS信息位于 `/etc/cleardns/upstream`，分为国内外两组，国内组可指定阿里DNS、DNSPod、114DNS等国内公共DNS服务，国外组需要指定可用的加密DNS服务，建议自行搭建DoH或DoT服务器。

ClearDNS支持多种[DNS服务协议](https://blog.343.re/dns-server/#DNS%E5%90%84%E5%8D%8F%E8%AE%AE%E7%AE%80%E4%BB%8B)，包括常规DNS、DNS-over-TLS、DNS-over-HTTPS、DNS-over-QUIC、DNSCrypt，写入时每一条记录一行，切勿加入任何无关注释。

DNSCrypt上游使用DNS Stamp封装，可以在[这里](https://dnscrypt.info/stamps)在线解析或生成链接内容。

```
# 常规DNS
1.1.1.1
8.8.8.8

# DNS-over-TLS
tls://223.5.5.5
tls://dns.alidns.com

# DNS-over-HTTPS
https://dns.pub/dns-query
https://223.5.5.5/dns-query

# DNS-over-QUIC
quic://dns.adguard.com

# DNSCrypt
sdns://AQIAAAAAAAAAFDE3Ni4xMDMuMTMwLjEzMDo1NDQzINErR_JS3PLCu_iZEIbq95zkSV2LFsigxDIuUso_OQhzIjIuZG5zY3J5cHQuZGVmYXVsdC5uczEuYWRndWFyZC5jb20
```

ClearDNS默认配置了以下DNS服务器

```
# 国内DNS服务器
# 可酌情添加其他DNS服务，运行时将并行请求，选取最快返回的记录
shell> /etc/cleardns/upstream/domestic.conf
223.5.5.5
119.29.29.29
114.114.114.114

# 国外DNS服务器
# 基于53端口的DNS服务会被GFW劫持，请移除默认的谷歌DNS服务
# 此处必须使用加密DNS服务地址，切勿使用UDP53方式
shell> /etc/cleardns/upstream/foreign.conf
8.8.4.4
8.8.8.8
```

重启Docker容器生效

```
shell> docker restart cleardns
```

### 配置域名分流规则

ClearDNS依据规则列表分流解析，使用以下规则文件，位于文件夹 `/etc/cleardns/list`

+ `china_ip_list.txt`：国内IP段，默认来自 `https://raw.fastgit.org/17mon/china_ip_list/master/china_ip_list.txt`

+ `chinalist.txt`：国内常见域名，默认来自 `https://res.343.re/Share/chinalist/chinalist.txt`

+ `gfwlist.txt`：被墙的常见域名，默认来自 `https://res.343.re/Share/gfwlist/gfwlist.txt`

以上文件将在每天凌晨2点自动更新，如果不想启用该功能，创建 `/etc/cleardns/list/no_auto_update` 文件即可。

### 配置AdGuardHome

浏览器打开ClearDNS服务，bridge模式输入宿主机IP地址，macvlan模式输入容器IP，进入AdGuardHome配置界面，设置账号和密码。

登录进入AdGuardHome管理界面，修改上游DNS为 `127.0.0.1:5353`，同时启用DNSSEC。

DNS封锁清单中，建议配置以下规则

+ `AdGuard DNS filte`：`https://adguardteam.github.io/AdGuardSDNSFilter/Filters/filter.txt`

+ `Anti-AD`：`https://anti-ad.net/easylist.txt`

### 配置DHCP服务

使用ClearDNS时，需要在路由器DHCP服务中指定DNS服务器，对于网桥模式，指定为宿主机IP，macvlan模式则指定为容器IP地址。

## 构建

如果需要修改ClearDNS或构建自己的容器，可按如下操作

**本地构建**

```
# 克隆仓库
shell> git clone https://github.com/dnomd343/ClearDNS.git
shell> cd ClearDNS
# 构建镜像
shell> docker build -t cleardns .
```

**交叉构建**

```
# 构建并推送至Docker Hub
shell> docker buildx build -t dnomd343/cleardns --platform="linux/amd64,linux/arm64,linux/386,linux/arm/v7" https://github.com/dnomd343/ClearDNS.git#master --push
```

## 许可证

MIT ©2021 [@dnomd343](https://github.com/dnomd343)
