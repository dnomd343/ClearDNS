#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import re

source = [
    'curl -sL https://github.com/Loukky/gfwlist-by-loukky/raw/master/gfwlist.txt | base64 -d | '
    'sed \'/^$\\|@@/d\' | sed \'s#!.\\+##;s#|##g;s#@##g;s#http:\\/\\/##;s#https:\\/\\/##;\' | '
    'sed \'/^[0-9]\\+\\.[0-9]\\+\\.[0-9]\\+\\.[0-9]\\+$/d\' | grep \'^[0-9a-zA-Z\\.-]\\+$\' | '
    'grep "\\." | sed \'s#^\\.\\+##\'',
    'curl -sL https://github.com/gfwlist/gfwlist/raw/master/gfwlist.txt | base64 -d | '
    'sed \'/^$\\|@@/d\' | sed \'s#!.\\+##;s#|##g;s#@##g;s#http:\\/\\/##;s#https:\\/\\/##;\' | '
    'sed \'/^[0-9]\\+\\.[0-9]\\+\\.[0-9]\\+\\.[0-9]\\+$/d\' | grep \'^[0-9a-zA-Z\\.-]\\+$\' | '
    'grep "\\." | sed \'s#^\\.\\+##\'',
    'curl -sL https://github.com/hq450/fancyss/raw/master/rules/gfwlist.conf | '
    'sed \'s/ipset=\\/\\.//g;s/\\/gfwlist//g;/^server/d\'',
    'curl -sL https://github.com/Loyalsoldier/v2ray-rules-dat/releases/latest/download/gfw.txt',
]

domains = set()
for script in source:
    raw = os.popen(script).read().split('\n')
    domains.update(filter(None, raw))
regex = r'^(?=^.{3,255}$)[a-zA-Z0-9][a-zA-Z0-9\-]{0,62}(.[a-zA-Z0-9][a-zA-Z0-9\-]{0,62})+$'
domains = {x for x in domains if re.search(regex, str(x)) is not None}

with open('gfwlist.txt', 'w') as fileObj:
    fileObj.write('\n'.join(sorted(domains)) + '\n')
