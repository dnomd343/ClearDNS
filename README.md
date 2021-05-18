# ClearDNS

> 容器化的无污染DNS服务

## 使用示例

启动容器之前需要为Docker创建一个macvlan网络

```
# 开启网卡混杂模式
shell> ip link set eth0 promisc on

# 创建macvlan网络
shell> docker network create -d macvlan \
--subnet=192.168.2.0/24 \
--gateway=192.168.2.1 \
-o parent=eth0 macvlan
```

宿主机网络需要重新配置

```
# Debian系示例
shell> vim /etc/network/interfaces

# 添加以下内容
auto eth0
iface eth0 inet manual

auto macvlan
iface macvlan inet static
  address 192.168.2.34
  netmask 255.255.255.0
  gateway 192.168.2.2
  dns-nameservers 192.168.2.1
  pre-up ip link add macvlan link eth0 type macvlan mode bridge
  post-down ip link del macvlan link eth0 type macvlan mode bridge

# 重启宿主机
```

运行容器

```
shell> docker run --name cleardns \
--network macvlan \
--privileged -d \
-v /etc/cleardns/:/etc/cleardns/ \
-v /etc/cleardns/SmartDNS/:/etc/smartdns/expose/ \
-v /etc/cleardns/AdGuardHome/:/opt/AdGuardHome/ \
-v /etc/timezone:/etc/timezone:ro \
-v /etc/localtime:/etc/localtime:ro \
dnomd343/cleardns
```

编辑macvlan网路的IP地址

```
# 编辑自定义初始化脚本
shell> vim /etc/cleardns/custom.sh

# 写入以下内容，修改容器的IP与网关
ip addr flush dev eth0
ip addr add 192.168.2.3/24 brd 192.168.2.255 dev eth0
ip route add default via 192.168.2.2

# 重启容器生效
shell> docker restart cleardns
```

设置国外DNS服务

```
shell> vim /etc/cleardns/SmartDNS/foreign.conf
# 默认为8.8.8.8，该地址与协议无抗污染功能，移除该行
# 建议使用DoH或DoT地址，切勿使用UDP53方式
# 修改完毕后重启容器生效
```

浏览器打开 `容器IP:3000`，进入AdGuardHome配置界面，设置账号后修改上游DNS为 `127.0.0.1:5353`，同时建议启用DNSSEC

配置DHCP服务器，设置DNS地址为容器IP，即可使用无污染DNS服务
