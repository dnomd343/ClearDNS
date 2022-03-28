curl -sL https://github.com/Loukky/gfwlist-by-loukky/raw/master/gfwlist.txt | base64 -d | sed '/^$\|@@/d' | sed 's#!.\+##;s#|##g;s#@##g;s#http:\/\/##;s#https:\/\/##;' | sed '/^[0-9]\+\.[0-9]\+\.[0-9]\+\.[0-9]\+$/d' | grep '^[0-9a-zA-Z\.-]\+$' | grep '\.' | sed 's#^\.\+##'
curl -sL https://github.com/gfwlist/gfwlist/raw/master/gfwlist.txt | base64 -d | sed '/^$\|@@/d' | sed 's#!.\+##;s#|##g;s#@##g;s#http:\/\/##;s#https:\/\/##;' | sed '/^[0-9]\+\.[0-9]\+\.[0-9]\+\.[0-9]\+$/d' | grep '^[0-9a-zA-Z\.-]\+$' | grep '\.' | sed 's#^\.\+##'
curl -sL https://github.com/hq450/fancyss/raw/master/rules/gfwlist.conf | sed 's/ipset=\/\.//g;s/\/gfwlist//g;/^server/d'
curl -sL https://github.com/Loyalsoldier/v2ray-rules-dat/releases/latest/download/gfw.txt
