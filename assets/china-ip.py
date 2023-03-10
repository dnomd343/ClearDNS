#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
from netaddr import IPSet
from netaddr import IPAddress
from netaddr import IPNetwork

operators = ['china', 'cmcc', 'chinanet', 'unicom', 'tietong', 'cernet', 'cstnet', 'drpeng', 'googlecn']
operators += ['%s6' % x for x in operators]  # add `...6` suffix
source = [
    'curl -sL https://github.com/misakaio/chnroutes2/raw/master/chnroutes.txt | sed \'/^#/d\'',
    'curl -sL https://github.com/metowolf/iplist/raw/master/data/special/china.txt',
    'curl -sL https://github.com/17mon/china_ip_list/raw/master/china_ip_list.txt',
] + ['curl -sL https://gaoyifan.github.io/china-operator-ip/%s.txt' % x for x in operators]

ipAddrs = set()
for script in source:  # traverse fetch commands
    raw = os.popen(script).read().split('\n')
    ipAddrs.update(filter(None, raw))

ipv4 = IPSet()
ipv6 = IPSet()
for ipAddr in ipAddrs:  # load all IP data
    try:
        ip = IPNetwork(ipAddr) if '/' in ipAddr else IPAddress(ipAddr)
        ipv4.add(ip) if ip.version == 4 else ipv6.add(ip)
    except:
        pass

with open('china-ip.txt', 'w') as fileObj:  # save to file
    fileObj.write('\n'.join([str(ip) for ip in ipv4.iter_cidrs()]) + '\n')  # format into CIDR
    fileObj.write('\n'.join([str(ip) for ip in ipv6.iter_cidrs()]) + '\n')
