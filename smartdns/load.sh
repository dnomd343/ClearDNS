load_domestic(){
cat>/etc/smartdns/expose/domestic.conf<<EOF
server 223.5.5.5
server 223.6.6.6
server 119.29.29.29
server 119.28.28.28
server 180.76.76.76
server 114.114.114.114
server 114.114.115.115
EOF
}

load_foreign(){
cat>/etc/smartdns/expose/foreign.conf<<EOF
server 8.8.8.8
EOF
}

mkdir -p /etc/smartdns/expose/
[ ! -s "/etc/smartdns/expose/domestic.conf" ] && load_domestic
[ ! -s "/etc/smartdns/expose/foreign.conf" ] && load_foreign
cat /etc/smartdns/expose/domestic.conf >> /etc/smartdns/config.conf
while read -r raw
do
  echo "$raw -group foreign -exclude-default-group" >> /etc/smartdns/config.conf
done < /etc/smartdns/expose/foreign.conf
