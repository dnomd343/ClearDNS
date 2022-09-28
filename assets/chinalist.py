#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import re

source = [
    'curl -sL https://github.com/felixonmars/dnsmasq-china-list/raw/master/accelerated-domains.china.conf'
    ' | sed \'/^#/d\' | sed \'s/server=\\///g;s/\\/114.114.114.114//g\'',
    'curl -sL https://github.com/Loyalsoldier/v2ray-rules-dat/releases/latest/download/direct-list.txt'
    ' | grep -v \':\'',
    'curl -sL https://github.com/hq450/fancyss/raw/master/rules/WhiteList_new.txt'
    ' | sed \'s/Server=\\///g;s/\\///g\'',
]

domains = set()
for script in source:
    raw = os.popen(script).read().split('\n')
    domains.update(filter(None, raw))
regex = r'^(?=^.{3,255}$)[a-zA-Z0-9][a-zA-Z0-9\-]{0,62}(.[a-zA-Z0-9][a-zA-Z0-9\-]{0,62})+$'
domains = {x for x in domains if re.search(regex, str(x)) is not None}
print('\n'.join(sorted(domains)))
