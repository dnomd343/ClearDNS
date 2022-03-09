#!/bin/sh
mkdir -p /etc/cleardns/asset/
mkdir -p /etc/cleardns/AdGuardHome/

cleardns_dir="/etc/cleardns"
overture_dir="/etc/overture"
cd $overture_dir && tar xf asset.tar
[ ! -f "$cleardns_dir/gfwlist.txt" ] && cp "$overture_dir/gfwlist.txt" "$cleardns_dir/gfwlist.txt"
[ ! -f "$cleardns_dir/chinalist.txt" ] && cp "$overture_dir/chinalist.txt" "$cleardns_dir/chinalist.txt"
[ ! -f "$cleardns_dir/china_ip.txt" ] && cp "$overture_dir/china_ip.txt" "$cleardns_dir/china_ip.txt"

upstream_file="/etc/cleardns/upstream.json"
if [ ! -f "$upstream_file" ]; then
cat > $upstream_file << EOF
{
  "domestic": {
    "bootstrap": "223.5.5.5",
    "primary": [
      "tls://dns.alidns.com",
      "https://doh.pub/dns-query"
    ],
    "fallback": [
      "223.5.5.5",
      "119.29.29.29"
    ]
  },
  "foreign": {
    "bootstrap": "8.8.8.8",
    "primary": [
      "tls://dns.google",
      "https://dns.cloudflare.com/dns-query"
    ],
    "fallback": [
      "1.1.1.1",
      "8.8.8.8"
    ]
  }
}
EOF
fi

/etc/overture/update.sh&
