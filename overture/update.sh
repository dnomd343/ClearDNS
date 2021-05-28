LIST_DIR="/etc/cleardns/list"
TEMP_DIR="$LIST_DIR/temp"
[ -f "$LIST_DIR/no_auto_update" ] && exit
rm -rf $TEMP_DIR
mkdir -p $TEMP_DIR
wget -P $TEMP_DIR https://res.343.re/Share/chinalist/chinalist.txt
wget -P $TEMP_DIR https://res.343.re/Share/gfwlist/gfwlist.txt
wget -P $TEMP_DIR https://raw.fastgit.org/17mon/china_ip_list/master/china_ip_list.txt
[ -s "$TEMP_DIR/chinalist.txt" ] && mv -f $TEMP_DIR/chinalist.txt $LIST_DIR/
[ -s "$TEMP_DIR/gfwlist.txt" ] && mv -f $TEMP_DIR/gfwlist.txt $LIST_DIR/
[ -s "$TEMP_DIR/china_ip_list.txt" ] && mv -f $TEMP_DIR/china_ip_list.txt $LIST_DIR/
rm -rf $TEMP_DIR
ps -ef | grep overture | grep -v grep | grep -v sh | awk '{print $1}' | xargs kill -9
/usr/bin/overture -c /etc/overture/config.yml > /dev/null 2>&1 &
