[ -f "/etc/cleardns/custom.sh" ] && sh /etc/cleardns/custom.sh

mkdir -p /etc/cleardns/list
mkdir -p /etc/cleardns/upstream
mkdir -p /etc/cleardns/AdGuardHome

[ ! -s "/etc/cleardns/upstream/domestic.conf" ] && echo -e "223.5.5.5\n119.29.29.29\n114.114.114.114" > /etc/cleardns/upstream/domestic.conf
[ ! -s "/etc/cleardns/upstream/foreign.conf" ] && echo -e "8.8.4.4\n8.8.8.8" > /etc/cleardns/upstream/foreign.conf

cat /etc/cleardns/upstream/domestic.conf | sed 's/^[ \t]*//g' | sed 's/[ \t]*$//g' | tr -s '\n' | sed '/^#/d' | sed '/^\/\//d' > /etc/cleardns/upstream/domestic
cat /etc/cleardns/upstream/foreign.conf | sed 's/^[ \t]*//g' | sed 's/[ \t]*$//g' | tr -s '\n' | sed '/^#/d' | sed '/^\/\//d' > /etc/cleardns/upstream/foreign

cmd="dnsproxy -p 4053"
while read -r raw
do
  cmd="$cmd -u $raw"
done < /etc/cleardns/upstream/domestic
eval "$cmd --cache --all-servers > /dev/null 2>&1 &"

cmd="dnsproxy -p 6053"
while read -r raw
do
  cmd="$cmd -u $raw"
done < /etc/cleardns/upstream/foreign
eval "$cmd --cache --all-servers > /dev/null 2>&1 &"

rm -f /etc/cleardns/upstream/domestic
rm -f /etc/cleardns/upstream/foreign

/etc/overture/update.sh
/usr/sbin/crond
/usr/bin/AdGuardHome -p 80 -w /etc/cleardns/AdGuardHome --no-check-update
