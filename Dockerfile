FROM alpine:3.15 as build
COPY . /tmp/ClearDNS
ENV UPX_VERSION="3.96"
ENV OVERTURE_VERSION="v1.8"
ENV DNSPROXY_VERSION="v0.41.3"
ENV ADGUARD_VERSION="v0.107.5"
RUN \
apk add git build-base bash make cmake glib-dev go npm nodejs yarn perl ucl-dev zlib-dev && \
\
mkdir /tmp/release/ && cd /tmp/ && \
git clone https://github.com/shawn1m/overture.git && \
git clone https://github.com/AdguardTeam/dnsproxy.git && \
git clone https://github.com/AdguardTeam/AdGuardHome.git && \
wget https://github.com/upx/upx/releases/download/v${UPX_VERSION}/upx-${UPX_VERSION}-src.tar.xz && \
\
cd /tmp/ && tar xf upx-${UPX_VERSION}-src.tar.xz && \
cd upx-${UPX_VERSION}-src && make all && \
mv ./src/upx.out /usr/bin/upx && \
\
cd /tmp/ClearDNS/ && mkdir ./build/ && \
cd ./build/ && cmake -DCMAKE_BUILD_TYPE=Release .. && make && \
mv ../bin/cleardns /tmp/release/ && \
\
cd /tmp/overture/ && git checkout ${OVERTURE_VERSION} && \
env CGO_ENABLED=0 go build -o overture -trimpath -ldflags "-X main.version=${OVERTURE_VERSION} -s -w" ./main/main.go && \
mv ./overture /tmp/release/ && \
\
cd /tmp/dnsproxy/ && git checkout ${DNSPROXY_VERSION} && \
env CGO_ENABLED=0 go build -trimpath -ldflags "-X main.VersionString=${DNSPROXY_VERSION} -s -w" && \
mv ./dnsproxy /tmp/release/ && \
\
cd /tmp/AdGuardHome/ && git checkout ${ADGUARD_VERSION} && \
make CHANNEL="release" VERSION=${ADGUARD_VERSION} && \
mv ./AdGuardHome /tmp/release/ && \
\
cd /tmp/release/ && strip cleardns && \
upx -9 AdGuardHome && upx -9 dnsproxy && upx -9 overture

FROM alpine:3.15 as asset
COPY . /tmp/ClearDNS
COPY --from=build /tmp/release /tmp/release
RUN \
wget https://res.dnomd343.top/Share/gfwlist/gfwlist.txt && \
wget https://res.dnomd343.top/Share/chinalist/chinalist.txt && \
wget https://res.dnomd343.top/Share/chinalist/china_ip.txt && \
tar czf asset.tar.gz gfwlist.txt chinalist.txt china_ip.txt && \
mkdir -p /tmp/asset/etc/ && mkdir -p /tmp/asset/usr/ && \
cp -r /tmp/ClearDNS/overture/ /tmp/asset/etc/ && \
mv asset.tar.gz /tmp/asset/etc/overture/ && \
mv /tmp/release/ /tmp/asset/usr/bin/ && \
mv /tmp/ClearDNS/load.sh /tmp/asset/usr/bin/load

FROM alpine:3.15
COPY --from=asset /tmp/asset /
RUN apk add --no-cache ca-certificates glib && \
echo -e "0\t4\t*\t*\t*\t/etc/overture/update.sh" > /var/spool/cron/crontabs/root
ENTRYPOINT ["cleardns"]
EXPOSE 53/udp 53/tcp 80/tcp
