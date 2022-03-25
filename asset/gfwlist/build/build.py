#!/usr/bin/python3
# -*- coding:utf-8 -*-

import os, re
workDir = os.path.split(os.path.realpath(__file__))[0]
domainList = os.popen('sh ' + workDir + '/source.sh | sort -u').read().split('\n')
regex = r'^(?=^.{3,255}$)[a-zA-Z0-9][a-zA-Z0-9\-]{0,62}(.[a-zA-Z0-9][a-zA-Z0-9\-]{0,62})+$'
for domain in domainList:
    if re.search(regex, domain) is None:
        domainList.remove(domain)
with open (workDir + '/../gfwlist.txt', 'w') as fileObj:
    fileObj.write('\n'.join(domainList) + '\n')
os.popen('gzip -cf9 ' + workDir + '/../gfwlist.txt > ' + workDir + '/../gfwlist.txt.gz')
