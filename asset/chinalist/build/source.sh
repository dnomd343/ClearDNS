curl -sL https://github.com/felixonmars/dnsmasq-china-list/raw/master/accelerated-domains.china.conf | sed '/^#/d' | sed 's/server=\///g;s/\/114.114.114.114//g'
curl -sL https://github.com/Loyalsoldier/v2ray-rules-dat/releases/latest/download/direct-list.txt | grep -v ':'
curl -sL https://github.com/hq450/fancyss/raw/master/rules/WhiteList_new.txt | sed 's/Server=\///g;s/\///g'
