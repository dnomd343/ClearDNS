FROM golang:1.17.8-alpine3.15 AS asset
COPY . /build/ClearDNS
ENV UPX_VERSION="3.96"
ENV OVERTURE_VERSION="v1.8"
ENV DNSPROXY_VERSION="v0.42.0"
ENV ADGUARD_VERSION="v0.107.5"
RUN \
apk add git build-base bash make cmake glib-dev npm nodejs yarn perl ucl-dev zlib-dev && \
\
mkdir /build/release/ && cd /build/ && \
git clone https://github.com/shawn1m/overture.git && \
git clone https://github.com/AdguardTeam/dnsproxy.git && \
git clone https://github.com/AdguardTeam/AdGuardHome.git && \
wget https://github.com/upx/upx/releases/download/v${UPX_VERSION}/upx-${UPX_VERSION}-src.tar.xz && \
\
cd /build/ && tar xf upx-${UPX_VERSION}-src.tar.xz && \
cd upx-${UPX_VERSION}-src && make all && \
mv ./src/upx.out /usr/bin/upx && \
\
cd /build/ClearDNS/ && mkdir ./build/ && \
cd ./build/ && cmake -DCMAKE_BUILD_TYPE=Release .. && make && \
mv ../bin/cleardns /build/release/ && \
\
cd /build/overture/ && git checkout ${OVERTURE_VERSION} && \
env CGO_ENABLED=0 go build -o overture -trimpath -ldflags "-X main.version=${OVERTURE_VERSION} -s -w" ./main/main.go && \
mv ./overture /build/release/ && \
\
cd /build/dnsproxy/ && git checkout ${DNSPROXY_VERSION} && \
env CGO_ENABLED=0 go build -trimpath -ldflags "-X main.VersionString=${DNSPROXY_VERSION} -s -w" && \
mv ./dnsproxy /build/release/ && \
\
cd /build/AdGuardHome/ && git checkout ${ADGUARD_VERSION} && \
make CHANNEL="release" VERSION=${ADGUARD_VERSION} && \
mv ./AdGuardHome /build/release/ && \
\
cd /build/release/ && \
strip cleardns && \
upx -9 AdGuardHome dnsproxy overture && \
\
cd /build/ && \
wget https://res.dnomd343.top/Share/gfwlist/gfwlist.txt && \
wget https://res.dnomd343.top/Share/chinalist/chinalist.txt && \
wget https://res.dnomd343.top/Share/chinalist/china-ip.txt && \
tar czf asset.tar.gz gfwlist.txt chinalist.txt china-ip.txt && \
\
mkdir -p /asset/etc/ && mkdir -p /asset/usr/ && \
cp -r /build/ClearDNS/overture/ /asset/etc/ && \
mv asset.tar.gz /asset/etc/overture/ && \
mv /build/release/ /asset/usr/bin/ && \
mv /build/ClearDNS/load.sh /asset/usr/bin/load

FROM alpine:3.15.2
COPY --from=asset /asset /
RUN apk add --no-cache ca-certificates glib iptables ip6tables && \
echo -e "0\t4\t*\t*\t*\t/etc/overture/update.sh" > /var/spool/cron/crontabs/root
ENTRYPOINT ["cleardns"]
