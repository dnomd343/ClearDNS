# 分流资源文件

本目录下的脚本用于拉取合并上游数据，生成以下对应的资源文件：

+ `gfwlist.txt` ：常见的被墙域名

+ `chinalist.txt` ：服务器在国内的常见域名

+ `china-ip.txt` ：国内 IP 段数据（CIDR 格式）

## 运行说明

在运行前，请检查 `netaddr` 模块是否安装，若无则执行以下命令：

```bash
pip3 install netaddr
```

> 由于 Python 的性能问题，脚本对 CIDR 的合并效率偏低，获取国内 IP 段可能花费较长时间。

```bash
$ ./gfwlist.py
$ ./china-ip.py
$ ./chinalist.py
$ ls
china-ip.txt  chinalist.txt  gfwlist.txt  ...
```

## 上游信息

### gfwlist

```yaml
https://github.com/gfwlist/gfwlist/raw/master/gfwlist.txt

https://github.com/hq450/fancyss/raw/master/rules/gfwlist.conf

https://github.com/Loukky/gfwlist-by-loukky/raw/master/gfwlist.txt

https://github.com/Loyalsoldier/v2ray-rules-dat/releases/latest/download/gfw.txt
```

### china-ip

```yaml
https://gaoyifan.github.io/china-operator-ip/

https://github.com/misakaio/chnroutes2/raw/master/chnroutes.txt

https://github.com/17mon/china_ip_list/raw/master/china_ip_list.txt

https://github.com/metowolf/iplist/raw/master/data/special/china.txt
```

### chinalist

```yaml
https://github.com/hq450/fancyss/raw/master/rules/WhiteList_new.txt'

https://github.com/Loyalsoldier/v2ray-rules-dat/releases/latest/download/direct-list.txt'

https://github.com/felixonmars/dnsmasq-china-list/raw/master/accelerated-domains.china.conf'
```

## 下发地址

FTP 链接：

+ `gfwlist.txt` ：`https://res.343.re/Share/cleardns/gfwlist.txt`
+ `china-ip.txt` ：`https://res.343.re/Share/cleardns/china-ip.txt`
+ `chinalist.txt` ：`https://res.343.re/Share/cleardns/chinalist.txt`

> 纯文本文件在压缩后可显著减少体积，加快下载速度。

压缩资源链接：

+ `gfwlist.txt` ：`https://res.343.re/Share/cleardns/gfwlist.txt.xz`
+ `china-ip.txt` ：`https://res.343.re/Share/cleardns/china-ip.txt.xz`
+ `chinalist.txt` ：`https://res.343.re/Share/cleardns/chinalist.txt.xz`

> `res.343.re` 为主 FTP 地址，`res.dnomd343.top` 为 CDN 镜像地址，国内用户访问更快。

国内加速地址：

+ `gfwlist.txt` ：`https://res.dnomd343.top/Share/cleardns/gfwlist.txt.xz`
+ `china-ip.txt` ：`https://res.dnomd343.top/Share/cleardns/china-ip.txt.xz`
+ `chinalist.txt` ：`https://res.dnomd343.top/Share/cleardns/chinalist.txt.xz`
