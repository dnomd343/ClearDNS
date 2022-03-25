#!/bin/sh
ASSET_DIR="/etc/cleardns/asset"
[ -f "$ASSET_DIR/no-update" ] && exit

TEMP_DIR="$ASSET_DIR/temp"
rm -rf "$TEMP_DIR" && mkdir -p "$TEMP_DIR"
wget -P "$TEMP_DIR" https://res.dnomd343.top/Share/gfwlist/gfwlist.txt.gz
wget -P "$TEMP_DIR" https://res.dnomd343.top/Share/chinalist/china-ip.txt.gz
wget -P "$TEMP_DIR" https://res.dnomd343.top/Share/chinalist/chinalist.txt.gz

gunzip $TEMP_DIR/*.gz
[ -s "$TEMP_DIR/gfwlist.txt" ] && mv -f "$TEMP_DIR/gfwlist.txt" "$ASSET_DIR"
[ -s "$TEMP_DIR/chinalist.txt" ] && mv -f "$TEMP_DIR/chinalist.txt" "$ASSET_DIR"
[ -s "$TEMP_DIR/china-ip.txt" ] && mv -f "$TEMP_DIR/china-ip.txt" "$ASSET_DIR"

rm -rf "$TEMP_DIR"
pgrep overture | xargs kill
