# ClearDNS

+ 无污染的 DNS 解析，避开运营商和防火长城的污染与劫持

+ 支持多种加密协议，包括 DoH 、DoT 、DoQ 与 DNSCrypt

+ 部署后更改 DHCP 选项即可生效，无需配置内网设备

+ DNS 请求审计，记录不同设备的查询日志

+ 自定义拦截规则，可屏蔽指定应用，如 QQ 、微信、微博等

+ IPv6支持，拦截特定的 DNS 请求类型，修改指定域名的 TTL

+ 在 DNS 层面上实现去广告与防跟踪功能，按需求配置自定义的拦截规则

+ 无论 APP 、网页还是机顶盒、IoT 设备等均可拦截

同时兼具强制 hosts 功能，将指定域名直接解析到指定 IP 上

ClearDNS 可部署在主路由器上，但需要路由器刷入支持 Docker 的固件；对于性能较低或不支持刷机的路由器，建议部署在内网一台长期开机的设备上（树莓派、小主机、旁路由等）。

ClearDNS也可部署在公网服务器上，面向国内提供服务

## 设计架构

ClearDNS 运行架构：

> AdGuardHome 用于加载拦截规则，可以自定义是否开启

```mermaid
  graph LR
    input{{Input}} -.-> adguard(AdGuardHome)
    subgraph ClearDNS
      adguard --> diverter(Diverter)
      diverter --> domestic(Domestic)
      diverter --> foreign(Foreign)
    end
    domestic -. Plain DNS .-> domestic_1(223.5.5.5)
    domestic -. DNS over TLS .-> domestic_2(tls://dot.pub)
    foreign -. DNS over QUIC .-> foreign_1(Private Server)
    foreign -. DNS over HTTPS .-> foreign_2(Private Server)
```

DNS 请求在通过 AdGuardHome 处理后，发往分流器 Diverter ，在这里将借助路由资源、国内组 Domestic 与国外组 Foreign 的返回结果，甄别出被污染的数据，返回正确的 DNS 解析；两组请求都可拥有多个上游服务器，ClearDNS 可以逐个对服务器进行请求，亦可同时发起查询。

ClearDNS 支持多种 DNS 协议，首先是常规 DNS ，即基于 UDP 或 TCP 的明文查询，该方式无法抵抗 DNS 污染，对部分运营商有效，仅建议用于国内无劫持的环境下使用；其次为 `DNS over HTTPS` 、`DNS over TLS` 、`DNS over QUIC` 与 `DNSCrypt` ，它们都是加密的 DNS 服务协议，可以抵抗污染与劫持行为，但可能被防火长城拦截；在出境请求中，`DNS over TLS` 特别是标准端口的服务已经被大规模封杀，`DNSCrypt` 也基本无法使用，目前建议使用 `DNS over QUIC` 与非标准路径的 `DNS over HTTPS` 服务。

对于多种 DNS 加密协议的简述，可以参考[浅谈DNS协议](https://blog.dnomd343.top/dns-server/#DNS%E5%90%84%E5%8D%8F%E8%AE%AE%E7%AE%80%E4%BB%8B)，里面讲解了不同协议的区别与优缺点，以及服务器分享格式。

在分流器部分，ClearDNS 需要借助三个资源文件工作：

+ `gfwlist.txt` ：记录常见的被墙域名

+ `chinalist.txt` ：记录服务器在国内的常见域名

+ `china-ip.txt` ：记录国内 IP 段数据（CIDR 格式）

> 防火长城的 DNS 污染有一个特点，被污染的结果必为境外 IP 地址

当分流器接到请求时，若在 `chinalist.txt` 中有所匹配，则只请求国内组，若在 `gfwlist.txt` 匹配，则仅请求国外组；两者均未未匹配的情况下，将同时请求国内组与国外组，若国内组返回结果在 `china-ip.txt` 中，则证明 DNS 未被污染，采纳国内组结果，若返回国外 IP 地址，则可能已经被污染，将返回国外组结果。

由于以上资源数据一直在变动，ClearDNS 内置了更新功能，可自动这些资源文件；数据从多个上游项目收集，每天进行一次合并整理，整合数据的源码可见[此处](./asset/)，您可以自由配置更新服务器，或者禁用更新。

## 配置格式

ClearDNS 支持 JSON 、 YAML 与 TOML 格式的配置文件，默认配置如下：

```yaml
port: 53

cache:
  enable: true
  size: 4194304
  optimistic: true

diverter:
  port: 5353

adguard:
  enable: true
  port: 80
  username: admin
  password: cleardns

domestic:
  port: 4053
  bootstrap: 223.5.5.5
  primary:
    - tls://dns.alidns.com
    - https://doh.pub/dns-query
  fallback:
    - 223.6.6.6
    - 119.29.29.29

foreign:
  port: 6053
  bootstrap: 8.8.8.8
  primary:
    - tls://dns.google
    - https://dns.cloudflare.com/dns-query
  fallback:
    - 1.1.1.1
    - 8.8.4.4

assets:
  cron: "0 4 * * *"
  update:
    gfwlist.txt: https://res.dnomd343.top/Share/gfwlist/gfwlist.txt
    china-ip.txt: https://res.dnomd343.top/Share/chinalist/china-ip.txt
    chinalist.txt: https://res.dnomd343.top/Share/chinalist/chinalist.txt
```

### Port

DNS 服务端口，支持常规的 TCP 与 UDP 查询，默认为 `53` ；若您想开放 `DNS over TLS` 、`DNS over HTTPS` 等其他协议的服务，可以在 AdGuardHome 中进行配置。

### Cache

DNS 缓存配置，此处与 AdGuardHome 中的缓存不相关，建议打开其中一个即可

```yaml
cache:
  size: 0
  enable: false
  optimistic: false
```

+ `enable` ：是否开启 DNS 缓存，默认为 `false`

+ `size` ：DNS 缓存容量，单位为字节，开启时建议设置在 `64k` 到 `4m` 量级，默认为 `0`

+ `optimistic` ：DNS 乐观缓存，开启后当数据 TTL 过期时，仍然返回原内容，但 TTL 修改为 10 ，同时立即向上游发起查询；由于绝大多数 DNS 记录在 TTL 期限内不会发生变化，这个机制可以显著减少请求平均延迟，但一旦出现变动，访问目标必须等待 10 秒后解析刷新才恢复正常。

### AdGuard

AdGuardHome 配置选项，此处选项将在每次重启后覆盖 AdGuardHome 的网页端配置。

```yaml
adguard:
  enable: true
  port: 80
  username: admin
  password: cleardns
```

+ `enable` ：是否开启 AdGuardHome 功能，默认为 `false`

+ `port` ：AdGuardHome 网页服务端口，默认为 `80`

+ `username` ：AdGuardHome 登录用户名，默认为 `admin`

+ `password` ：AdGuardHome 登录密码，默认为 `cleardns`

### Diverter

DNS 分流选项，将数据

```yaml
diverter:
  port: 5353
  gfwlist: []
  china_ip: []
  chinalist: []
```

+ `port` ：DNS 分流器端口，若 AdGuardHome 未开启，本选项将失效，默认为 `5353`

> 以下选项用于添加自定义规则，将优先覆盖在资源文件上

+ `gfwlist` ：自定义的 GFW 拦截域名列表，针对该域名的查询将屏蔽 `domestic` 组结果

+ `chinalist` ：自定义的国内域名列表，针对该域名的查询将屏蔽 `foreign` 组结果

+ `china-ip` ：自定义的国内 IP 段，`domestic` 组返回内容若命中则采纳，否则使用 `foreign` 组结果

### Domestic

国内组 DNS 配置选项

```yaml
domestic:
  port: 4053
  verify: true
  parallel: true
  bootstrap: "..."
  primary:
    - ...
    - ...
  fallback:
    - ...
    - ...
```

+ `port` ：国内组 DNS 端口，默认为 `4053`

+ `verify` ：是否验证证书合法性，关闭后允许无效的 TLS 证书，默认为 `true`

+ `parallel` ：是否对多个上游进行并行查询，默认为 `true`

+ `bootstrap` ：引导 DNS 服务器，用于 `primary` 与 `fallback` 中 DNS 服务器域名的查询，必须为常规 DNS 服务 ，此处允许为字符串或字符串数组

+ `primary` ：主 DNS 列表，用于默认情况下的查询

+ `fallback` ：备用 DNS 服务器，当 `primary` 中 DNS 服务器无效时，回落到此处再次查询

### Foreign

国外组 DNS 配置选项

```yaml
foreign:
  port: 6053
  verify: true
  parallel: true
  bootstrap: "..."
  primary:
    - ...
    - ...
  fallback:
    - ...
    - ...
```

+ `port` ：国外组 DNS 端口，默认为 `6053`

> Foreign 选项意义与 Domestic 相同，可参考上文描述

### Reject

DNS 拒绝类型，指定屏蔽的 DNS 记录类型，不同 DNS 类型编号可参考 [Wiki](https://en.wikipedia.org/wiki/List_of_DNS_record_types) ，默认为空。

```yaml
reject:
  - 255
```

### Hosts

Hosts 记录，指定域名对应 IP 地址，支持正则匹配，默认为空。

```yaml
hosts:
  - "10.0.0.1 example.com$"
  - "..."
```

### TTL

配置特定域名的 ttl 时长，支持正则表达式匹配，默认为空。

```yaml
ttl:
  - "example.com$ 300"
  - "..."
```

### Custom

自定义脚本，在启动前执行

> 本功能用于注入自定义功能，基于 Alpine 的 ash 执行，可能不支持部分 bash 语法

```yaml
custom:
  - "..."
```

### Assets

分流资源升级配置，用于自动更新资源文件

```yaml
assets:
  disable: false
  cron: "0 4 * * *"
  update:
    gfwlist.txt: https://res.dnomd343.top/Share/gfwlist/gfwlist.txt
    china-ip.txt: https://res.dnomd343.top/Share/chinalist/china-ip.txt
    chinalist.txt: https://res.dnomd343.top/Share/chinalist/chinalist.txt
```

+ `disable` ：是否关闭资源文件加载，默认为 `false`

+ `cron` ：指定触发升级的 Crontab 表达式

+ `url` ：指定资源升级的 URL 链接

## 部署教程

### 1. 网络配置

> 本项目基于 Docker 构建，在 [Docker Hub](https://hub.docker.com/repository/docker/dnomd343/cleardns) 或 [Github Package](https://github.com/dnomd343/ClearDNS/pkgs/container/cleardns) 可以查看已构建的各版本镜像。

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

XProxy 同时发布在多个镜像源上：

+ `Docker Hub` ：`dnomd343/cleardns`

+ `Github Package` ：`ghcr.io/dnomd343/cleardns`

+ `阿里云镜像` ：`registry.cn-shenzhen.aliyuncs.com/dnomd343/cleardns`

> 下述命令中，容器路径可替换为上述其他源，国内网络建议首选阿里云仓库

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
-v /etc/cleardns/:/cleardns/ \
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
-v /etc/cleardns/:/cleardns/ \
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
-v /etc/cleardns/:/cleardns/ \
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
