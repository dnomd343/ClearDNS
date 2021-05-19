rm -f /etc/overture/chinalist.txt
rm -f /etc/overture/gfwlist.txt
rm -f /etc/overture/china_ip_list.txt
wget -P /etc/overture https://res.343.re/Share/chinalist/chinalist.txt
wget -P /etc/overture https://res.343.re/Share/gfwlist/gfwlist.txt
wget -P /etc/overture https://raw.fastgit.org/17mon/china_ip_list/master/china_ip_list.txt
ps -ef | grep overture | grep -v grep | grep -v sh | awk '{print $1}' | xargs kill -9
overture -c /etc/overture/config.yml > /dev/null 2>&1 &
