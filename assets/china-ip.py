#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
from IPy import IP, IPSet

operators = ['china', 'cmcc', 'chinanet', 'unicom', 'tietong', 'cernet', 'cstnet', 'drpeng', 'googlecn']
operators += ['%s6' % x for x in operators]  # add `...6` suffix
source = [
    'curl -sL https://github.com/misakaio/chnroutes2/raw/master/chnroutes.txt | sed \'/^#/d\'',
    'curl -sL https://github.com/metowolf/iplist/raw/master/data/special/china.txt',
    'curl -sL https://github.com/17mon/china_ip_list/raw/master/china_ip_list.txt',
] + ['curl -sL https://gaoyifan.github.io/china-operator-ip/%s.txt' % x for x in operators]

ipv4 = IPSet()
ipv6 = IPSet()
ipAddrs = set()
for script in source:  # traverse fetch commands
    raw = os.popen(script).read().split('\n')
    ipAddrs.update(filter(None, raw))
for ipAddr in ipAddrs:
    try:
        ip = IP(ipAddr)  # ip format check
        (ipv4 if ip.version() == 4 else ipv6).add(ip)
    except:
        pass

release = [('%s' if '/' in str(ip) else '%s/32') % str(ip) for ip in ipv4]  # format into CIDR
release += [('%s' if '/' in str(ip) else '%s/128') % str(ip) for ip in ipv6]
with open('china-ip.txt', 'w') as fileObj:
    fileObj.write('\n'.join(release) + '\n')
