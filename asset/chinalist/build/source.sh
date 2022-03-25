curl -sL https://raw.githubusercontent.com/felixonmars/dnsmasq-china-list/master/accelerated-domains.china.conf  | sed 's/server=\///g;s/\/114.114.114.114//g' | sort -u
curl -sL https://raw.githubusercontent.com/hq450/fancyss/master/rules/WhiteList_new.txt  | sed 's/Server=\///g;s/\///g' | sort -u
