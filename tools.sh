get_github_latest_version() {
  VERSION=$(curl --silent "https://api.github.com/repos/$1/releases/latest" | grep '"tag_name":' | sed -E 's/.*"([^"]+)".*/\1/');
}

TARGET_DIR="/tmp/tools"
mkdir -p $TARGET_DIR

get_github_latest_version "AdguardTeam/AdGuardHome"
wget -P /tmp/tools "https://hub.fastgit.org/AdguardTeam/AdGuardHome/releases/download/$VERSION/AdGuardHome_linux_arm64.tar.gz"
get_github_latest_version "shawn1m/overture"
wget -P /tmp/tools "https://hub.fastgit.org/shawn1m/overture/releases/download/$VERSION/overture-linux-arm64.zip"
get_github_latest_version "pymumu/smartdns"
wget -P /tmp/tools "https://hub.fastgit.org/pymumu/smartdns/releases/download/$VERSION/smartdns-aarch64"

mkdir $TARGET_DIR/AdGuard
tar xf $TARGET_DIR/AdGuardHome_linux_arm64.tar.gz -C $TARGET_DIR/AdGuard
unzip $TARGET_DIR/overture-linux-arm64.zip -d $TARGET_DIR/overture
mv $TARGET_DIR/AdGuard/AdGuardHome/AdGuardHome $TARGET_DIR/AdGuardHome
mv $TARGET_DIR/overture/overture-linux-arm64 $TARGET_DIR/overture-linux-arm64
rm -rf $TARGET_DIR/AdGuard
rm -rf $TARGET_DIR/overture
mv $TARGET_DIR/overture-linux-arm64 $TARGET_DIR/overture
mv $TARGET_DIR/smartdns-aarch64 $TARGET_DIR/smartdns
chmod +x $TARGET_DIR/smartdns
rm -f $TARGET_DIR/AdGuardHome_linux_arm64.tar.gz
rm -f $TARGET_DIR/overture-linux-arm64.zip
