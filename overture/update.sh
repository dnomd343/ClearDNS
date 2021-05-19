OVERTURE_DIR="/etc/overture"
TEMP_DIR="$OVERTURE_DIR/list/temp"
[ -f "$OVERTURE_DIR/list/no_auto_update" ] && exit
rm -rf $TEMP_DIR
mkdir -p $TEMP_DIR
wget -P $TEMP_DIR https://res.343.re/Share/chinalist/chinalist.txt
wget -P $TEMP_DIR https://res.343.re/Share/gfwlist/gfwlist.txt
wget -P $TEMP_DIR https://raw.fastgit.org/17mon/china_ip_list/master/china_ip_list.txt
[ -s "$TEMP_DIR/chinalist.txt" ] && mv -f $TEMP_DIR/chinalist.txt $OVERTURE_DIR/list/
[ -s "$TEMP_DIR/gfwlist.txt" ] && mv -f $TEMP_DIR/gfwlist.txt $OVERTURE_DIR/list/
[ -s "$TEMP_DIR/china_ip_list.txt" ] && mv -f $TEMP_DIR/china_ip_list.txt $OVERTURE_DIR/list/
rm -rf $TEMP_DIR
ps -ef | grep overture | grep -v grep | grep -v sh | awk '{print $1}' | xargs kill -9
overture -c /etc/overture/config.yml > /dev/null 2>&1 &
