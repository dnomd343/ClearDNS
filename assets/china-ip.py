#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
from IPy import IP, IPSet

source = [
    'curl -sL https://github.com/misakaio/chnroutes2/raw/master/chnroutes.txt | sed \'/^#/d\'',
    'curl -sL https://github.com/metowolf/iplist/raw/master/data/special/china.txt',
    'curl -sL https://github.com/17mon/china_ip_list/raw/master/china_ip_list.txt',
    'curl -sL https://gaoyifan.github.io/china-operator-ip/cernet.txt',
    'curl -sL https://gaoyifan.github.io/china-operator-ip/china.txt',
    'curl -sL https://gaoyifan.github.io/china-operator-ip/chinanet.txt',
    'curl -sL https://gaoyifan.github.io/china-operator-ip/cmcc.txt',
    'curl -sL https://gaoyifan.github.io/china-operator-ip/cstnet.txt',
    'curl -sL https://gaoyifan.github.io/china-operator-ip/drpeng.txt',
    'curl -sL https://gaoyifan.github.io/china-operator-ip/googlecn.txt',
    'curl -sL https://gaoyifan.github.io/china-operator-ip/tietong.txt',
    'curl -sL https://gaoyifan.github.io/china-operator-ip/unicom.txt',
    'curl -sL https://gaoyifan.github.io/china-operator-ip/cernet6.txt',
    'curl -sL https://gaoyifan.github.io/china-operator-ip/china6.txt',
    'curl -sL https://gaoyifan.github.io/china-operator-ip/chinanet6.txt',
    'curl -sL https://gaoyifan.github.io/china-operator-ip/cmcc6.txt',
    'curl -sL https://gaoyifan.github.io/china-operator-ip/cstnet6.txt',
    'curl -sL https://gaoyifan.github.io/china-operator-ip/drpeng6.txt',
    'curl -sL https://gaoyifan.github.io/china-operator-ip/googlecn6.txt',
    'curl -sL https://gaoyifan.github.io/china-operator-ip/tietong6.txt',
    'curl -sL https://gaoyifan.github.io/china-operator-ip/unicom6.txt',
]

ips = set()
for script in source:
    raw = os.popen(script).read().split('\n')
    ips.update(filter(None, raw))

v4 = IPSet()
v6 = IPSet()
for ip in ips:
    try:
        ipAddr = IP(ip)
        (v4 if ipAddr.version() == 4 else v6).add(ipAddr)
    except:
        pass

ips = []
for ip in v4:
    ips.append(str(ip) + ('' if '/' in str(ip) else '/32'))
for ip in v6:
    ips.append(str(ip) + ('' if '/' in str(ip) else '/128'))

with open('china-ip.txt', 'w') as fileObj:
    fileObj.write('\n'.join(ips) + '\n')
os.system('xz -kf9 china-ip.txt')
