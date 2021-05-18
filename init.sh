[ ! -s "/etc/cleardns/custom.sh" ] && touch /etc/cleardns/custom.sh
sh /etc/cleardns/custom.sh
sh /etc/smartdns/load.sh
sh /etc/overture/update.sh
/usr/sbin/crond
smartdns -c /etc/smartdns/config.conf
overture -c /etc/overture/config.yml > /dev/null 2>&1 &
/usr/bin/AdGuardHome -w /opt/AdGuardHome
