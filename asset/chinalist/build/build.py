#!/usr/bin/python3
# -*- coding:utf-8 -*-

import os, re
workDir = os.path.split(os.path.realpath(__file__))[0]
domainList = os.popen('sh ' + workDir + '/source.sh | sort -u').read().split('\n')
regex = r'^(?=^.{3,255}$)[a-zA-Z0-9][a-zA-Z0-9\-]{0,62}(.[a-zA-Z0-9][a-zA-Z0-9\-]{0,62})+$'
for domain in domainList:
    if re.search(regex, domain) is None:
        domainList.remove(domain)
with open (workDir + '/../chinalist.txt', 'w') as fileObj:
    fileObj.write('\n'.join(domainList) + '\n')
os.popen('gzip -cf9 ' + workDir + '/../chinalist.txt > ' + workDir + '/../chinalist.txt.gz')

from IPy import IP, IPSet
ipv4List = IPSet()
ipv6List = IPSet()
ipList = os.popen('sh ' + workDir + '/source-ip.sh | sort -u').read().split('\n')
for ipStr in ipList:
    try:
        ipAddr = IP(ipStr)
        if ipAddr.version() == 4:
            ipv4List.add(ipAddr)
        else:
            ipv6List.add(ipAddr)
    except:
        pass
ipList = []
for ipAddr in ipv4List:
    ipList.append(str(ipAddr) if str(ipAddr).find('/') != -1 else str(ipAddr) + '/32')
for ipAddr in ipv6List:
    ipList.append(str(ipAddr) if str(ipAddr).find('/') != -1 else str(ipAddr) + '/128')
with open (workDir + '/../china-ip.txt', 'w') as fileObj:
    fileObj.write('\n'.join(ipList) + '\n')
os.popen('gzip -cf9 ' + workDir + '/../china-ip.txt > ' + workDir + '/../china-ip.txt.gz')
