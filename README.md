# ClearDNS

> 容器化的无污染DNS服务，同时兼具广告拦截与防跟踪功能

ClearDNS基于Docker容器运行，用于提供纯净的DNS服务，避开运营商与防火长城的DNS污染与劫持，连接到路由器的内网设备无需任何改动即可使用，它还会记录所有解析请求，用于查询哪些设备都访问了哪些域名。

ClearDNS可以在DNS层面上实现去广告与防跟踪功能，按需求配置自定义的拦截规则，无论APP、网页还是机顶盒、IoT设备等，只要接入到同个网络下均有效。同时兼具强制host功能，将指定域名直接解析到指定IP上，也可用于屏蔽特定的应用，如QQ、微信、微博等。

ClearDNS可部署在主路由器上，但需要路由器刷入支持Docker的固件；对于性能较低或不支持刷机的路由器，建议部署在内网一台长期开机的设备上（树莓派、小主机、旁路由等）。

## 部署教程

### 1. 网络配置

ClearDNS基于Docker网络有以下三种部署模式：

| | host模式 | bridge模式 | macvlan模式 |
| :-: | :-: | :-: | :-: |
| 网络原理 | 宿主机网络 | 桥接网络 | 虚拟独立mac网卡 |
| 服务IP | 宿主机IP | 宿主机IP | 容器独立IP |
| 宿主机IP | 静态IP地址 | 静态IP地址 | 静态/动态IP地址 |
| 宿主机网络 | 无需改动网络配置 | Docker自动适配 | 手动修改底层网络配置 |
| 宿主机端口 | 占用宿主机53,80,4053,5353,6053端口 | 占用宿主机53与80端口 | 不占用端口 |
| 管理完整性 | 完全 | 无法区分客户端 | 完全 |
| 宿主机耦合 | 强耦合 | 一般耦合 | 链路层以上完全分离 |
| 网络性能 | 相对较高 | 相对较低 | 相对适中 |
| 部署难度 | 简单 | 简单 | 复杂 |

> 不熟悉Linux网络配置请勿使用macvlan模式，新手建议首选host模式。

以下操作均于root用户下执行

```
# 检查Docker环境
shell> docker --version
Docker version ···, build ···

# 无Docker环境请先执行安装
shell> wget -qO- https://get.docker.com/ | bash
```

ClearDNS可以从多个镜像源获取，其数据完全相同，国内用户建议首选阿里云镜像。

```
# Docker Hub
shell> docker pull docker.io/dnomd343/cleardns

# Github Package
shell> docker pull ghcr.io/dnomd343/cleardns

# 阿里云个人镜像
shell> docker pull registry.cn-shenzhen.aliyuncs.com/dnomd343/cleardns
```

<details>

<summary><b>bridge模式</b></summary>

<br/>

```
# 检查端口占用
shell> netstat -tlnpu | grep -E ":53|:80"
# 如果TCP/53或UDP/53已被占用，请先关闭对应进程
# 如果TCP/80端口被占用，可以关闭对应进程，也可使用其他端口
```

```
# 运行ClearDNS容器
# 映射系统时间文件以同步容器内部时区
shell> docker run --restart always \
--name cleardns -d \
-v /etc/cleardns/:/etc/cleardns/ \
-v /etc/timezone:/etc/timezone:ro \
-v /etc/localtime:/etc/localtime:ro \
-p 53:53/udp -p 53:53 -p 80:80 \
dnomd343/cleardns
# 此处为DockerHub镜像源，可按上文链接替换为其他源
```

</details>

<details>

<summary><b>host模式</b></summary>

<br/>

```
# 检查端口占用
shell> netstat -tlnpu | grep -E ":53|:80|:4053|:5353|:6053"
# 如果UDP/53、UDP/4053、UDP/5353、UDP/6053、TCP/53、TCP/80、TCP/4053、TCP/5353、TCP/6053已被占用，请先关闭对应进程
```

```
# 运行ClearDNS容器
docker run --restart always \
--name cleardns -d \
--network host \
-v /etc/cleardns/:/etc/cleardns/ \
-v /etc/timezone:/etc/timezone:ro \
-v /etc/localtime:/etc/localtime:ro \
dnomd343/cleardns
# 此处为DockerHub镜像源，可按上文链接替换为其他源
```

</details>

<details>

<summary><b>macvlan模式</b></summary>

<br/>

启动容器前需要创建一个macvlan网络

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
# 此处为DockerHub镜像源，可按上文链接替换为其他源
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

</details>

<hr/>

```
# ClearDNS会将数据持久化，以在重启Docker或宿主机后保留配置及日志
# 使用以下命令清除之前的ClearDNS配置及数据
shell> rm -rf /etc/cleardns
```

### 2. 指定上游DNS服务器

上游DNS信息位于 `/etc/cleardns/upstream.json`，分为国内外两组，国内组可指定阿里DNS、DNSPod、114DNS等国内公共DNS服务，国外组需要指定可用的加密DNS服务，建议自行搭建DoH或DoT服务器。

ClearDNS支持多种[DNS服务协议](https://blog.dnomd343.top/dns-server/#DNS%E5%90%84%E5%8D%8F%E8%AE%AE%E7%AE%80%E4%BB%8B)，包括常规DNS、DNS-over-TLS、DNS-over-HTTPS、DNS-over-QUIC、DNSCrypt。

> DNSCrypt使用DNS Stamp封装，可以在[这里](https://dnscrypt.info/stamps)在线解析或生成链接内容。

各协议格式示例如下：

```
# Plain DNS
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
# 此处指定的国外组服务器可能已被GFW屏蔽，强烈建议使用私有加密DNS服务器
# bootstrap指定DNS服务器IP地址，用于解析加密DNS服务器域名，省略时使用系统DNS
# 若指定多台服务器，DNS请求时将并发查询，因此不宜指定过多服务器
# 当primary中DNS均不可用时，将自动尝试fallback中的DNS地址
{
  "domestic": {
    "bootstrap": "223.5.5.5",
    "primary": [
      "tls://dns.alidns.com",
      "https://doh.pub/dns-query"
    ],
    "fallback": [
      "223.5.5.5",
      "119.29.29.29"
    ]
  },
  "foreign": {
    "bootstrap": "8.8.8.8",
    "primary": [
      "tls://dns.google",
      "https://dns.cloudflare.com/dns-query"
    ],
    "fallback": [
      "1.1.1.1",
      "8.8.8.8"
    ]
  }
}
```

重启Docker容器生效

```
shell> docker restart cleardns
```

### 3. 配置域名分流规则

ClearDNS依据规则列表分流解析，使用以下规则文件，位于文件夹 `/etc/cleardns/asset` 中

+ [`china-ip.txt`](https://res.343.re/Share/chinalist/china-ip.txt)：国内IP段

+ [`chinalist.txt`](https://res.343.re/Share/chinalist/chinalist.txt)：国内常见域名

+ [`gfwlist.txt`](https://res.343.re/Share/gfwlist/gfwlist.txt)：被GFW屏蔽的常见域名

以上文件将在每天凌晨4点自动更新，如果不想启用该功能，创建 `/etc/cleardns/asset/no-update` 文件即可。

规则文件默认从 `res.dnomd343.top` 拉取，若该域名被污染或屏蔽，你可以使用本项目 `asset/gfwlist/build` 与 `asset/chinalist/build` 下的构建脚本自行生成规则列表。

> 构建脚本需要访问Github获取资源，手动构建时请注意网络连通性。

### 4. 配置AdGuardHome

浏览器打开ClearDNS服务，host与bridge模式输入宿主机IP地址，macvlan模式输入容器IP，进入AdGuardHome配置界面，设置账号和密码，登录进入AdGuardHome管理界面。

进入 `设置` - `DNS设置`，修改上游DNS为 `127.0.0.1:5353`，其他选项保持默认。此外，下方其他设置中建议启用DNSSEC，内存允许的情况下适当拉大缓存大小，并开启乐观缓存。

DNS封锁清单中，可配置以下规则：

+ `AdGuard`：`https://adguardteam.github.io/AdGuardSDNSFilter/Filters/filter.txt`

+ `Anti-AD`：`https://anti-ad.net/easylist.txt`

+ `AdAway`：`https://adaway.org/hosts.txt`

+ `乘风规则`：`https://res.343.re/Share/Adblock-Rules/xinggsf.txt`

> 配置过多的规则会导致设备负载变大，请酌情添加。

### 5. 配置DHCP信息

为了使ClearDNS生效，需要在路由器DHCP服务中指定DNS服务器，host与bridge模式指定为宿主机IP，macvlan模式指定为容器IP。

> 对于内网中一些固定IP信息的设备，需要手动更改其DNS为上述IP地址。

## 手动编译

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
shell> docker buildx build -t dnomd343/cleardns --platform="linux/amd64,linux/arm64,linux/386,linux/arm/v7" https://github.com/dnomd343/ClearDNS.git --push
```

## 许可证

MIT ©2022 [@dnomd343](https://github.com/dnomd343)
