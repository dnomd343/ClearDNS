[ -f "/etc/cleardns/custom.sh" ] && sh /etc/cleardns/custom.sh

mkdir -p /etc/cleardns/list
mkdir -p /etc/cleardns/upstream
mkdir -p /etc/cleardns/AdGuardHome

[ ! -s "/etc/cleardns/upstream/domestic.conf" ] && echo -e "223.5.5.5\n119.29.29.29\n114.114.114.114" > /etc/cleardns/upstream/domestic.conf
[ ! -s "/etc/cleardns/upstream/foreign.conf" ] && echo -e "8.8.4.4\n8.8.8.8" > /etc/cleardns/upstream/foreign.conf

cmd="dnsproxy -p 4053"
while read -r raw
do
  cmd="$cmd -u $raw"
done < /etc/cleardns/upstream/domestic.conf
eval "$cmd --cache --all-servers > /dev/null 2>&1 &"

cmd="dnsproxy -p 6053"
while read -r raw
do
  cmd="$cmd -u $raw"
done < /etc/cleardns/upstream/foreign.conf
eval "$cmd --cache --all-servers > /dev/null 2>&1 &"

sh /etc/overture/update.sh
/usr/sbin/crond
/usr/bin/AdGuardHome -p 80 -w /etc/cleardns/AdGuardHome
