get_github_latest_version() {
  VERSION=$(curl --silent "https://api.github.com/repos/$1/releases/latest" | grep '"tag_name":' | sed -E 's/.*"([^"]+)".*/\1/');
}

get_architecture() {
  case "$(uname -m)" in
    'i386' | 'i686')
      MACHINE='i386'
      ;;
    'amd64' | 'x86_64')
      MACHINE='amd64'
      ;;
    'armv7' | 'armv7l')
      MACHINE='arm'
      ;;
    'armv8' | 'aarch64')
      MACHINE='arm64'
      ;;
    *)
      echo "The architecture is not supported."
      exit 1
      ;;
  esac
}

ASSET_DIR="/tmp/asset"
PKG_DIR="$ASSET_DIR/pkg"
mkdir -p $PKG_DIR

get_architecture
case "$MACHINE" in
  'i386')
    AGH_PKG_NAME="AdGuardHome_linux_386.tar.gz"
    OVERTURE_FILE_NAME="overture-linux-386"
    OVERTURE_PKG_NAME="$OVERTURE_FILE_NAME.zip"
    DNSPROXY_FOLDER_NAME="linux-386"
    DNSPROXY_PKG_PREFIX="dnsproxy-$DNSPROXY_FOLDER_NAME-"
    ;;
  'amd64')
    AGH_PKG_NAME="AdGuardHome_linux_amd64.tar.gz"
    OVERTURE_FILE_NAME="overture-linux-amd64"
    OVERTURE_PKG_NAME="$OVERTURE_FILE_NAME.zip"
    DNSPROXY_FOLDER_NAME="linux-amd64"
    DNSPROXY_PKG_PREFIX="dnsproxy-$DNSPROXY_FOLDER_NAME-"
    ;;
  'arm')
    AGH_PKG_NAME="AdGuardHome_linux_armv7.tar.gz"
    OVERTURE_FILE_NAME="overture-linux-arm"
    OVERTURE_PKG_NAME="$OVERTURE_FILE_NAME.zip"
    DNSPROXY_FOLDER_NAME="linux-arm6"
    DNSPROXY_PKG_PREFIX="dnsproxy-$DNSPROXY_FOLDER_NAME-"
    ;;
  'arm64')
    AGH_PKG_NAME="AdGuardHome_linux_arm64.tar.gz"
    OVERTURE_FILE_NAME="overture-linux-arm64"
    OVERTURE_PKG_NAME="$OVERTURE_FILE_NAME.zip"
    DNSPROXY_FOLDER_NAME="linux-arm64"
    DNSPROXY_PKG_PREFIX="dnsproxy-$DNSPROXY_FOLDER_NAME-"
    ;;
  *)
    exit 1
    ;;
esac

get_github_latest_version "AdguardTeam/AdGuardHome"
wget -P $PKG_DIR "https://github.com/AdguardTeam/AdGuardHome/releases/download/$VERSION/$AGH_PKG_NAME"
tar xf $PKG_DIR/$AGH_PKG_NAME -C $PKG_DIR
mv $PKG_DIR/AdGuardHome/AdGuardHome $ASSET_DIR/

get_github_latest_version "shawn1m/overture"
wget -P $PKG_DIR "https://github.com/shawn1m/overture/releases/download/$VERSION/$OVERTURE_PKG_NAME"
unzip $PKG_DIR/$OVERTURE_PKG_NAME -d $PKG_DIR
mv $PKG_DIR/$OVERTURE_FILE_NAME $ASSET_DIR/overture

get_github_latest_version "AdguardTeam/dnsproxy"
DNSPROXY_PKG_NAME="$DNSPROXY_PKG_PREFIX$VERSION.tar.gz"
wget -P $PKG_DIR "https://github.com/AdguardTeam/dnsproxy/releases/download/$VERSION/$DNSPROXY_PKG_NAME"
tar xf $PKG_DIR/$DNSPROXY_PKG_NAME -C $PKG_DIR
mv $PKG_DIR/$DNSPROXY_FOLDER_NAME/dnsproxy $ASSET_DIR

rm -rf $PKG_DIR
