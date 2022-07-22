FROM golang:1.18-alpine3.16 AS adguardhome
ENV ADGUARD_VERSION="v0.107.8"
RUN \
  apk add git make npm yarn && \
  git clone https://github.com/AdguardTeam/AdGuardHome.git && \
  cd ./AdGuardHome/ && git checkout ${ADGUARD_VERSION} && \
  make CHANNEL="release" VERSION=${ADGUARD_VERSION} && \
  mv ./AdGuardHome /tmp/

FROM golang:1.18-alpine3.16 AS dnsproxy
ENV DNSPROXY_VERSION="v0.43.1"
RUN \
  apk add git && \
  git clone https://github.com/AdguardTeam/dnsproxy.git && \
  cd ./dnsproxy/ && git checkout ${DNSPROXY_VERSION} && \
  env CGO_ENABLED=0 go build -trimpath \
    -ldflags "-X main.VersionString=${DNSPROXY_VERSION} -s -w" && \
  mv ./dnsproxy /tmp/

FROM golang:1.18-alpine3.16 AS overture
ENV OVERTURE_VERSION="v1.8"
RUN \
  apk add git && \
  git clone https://github.com/shawn1m/overture.git && \
  cd ./overture/ && git checkout ${OVERTURE_VERSION} && \
  env CGO_ENABLED=0 go build -o overture -trimpath \
    -ldflags "-X main.version=${OVERTURE_VERSION} -s -w" ./main/main.go && \
  mv ./overture /tmp/

# upx can't be compiled under gcc11, so we should use alpine:3.15
FROM alpine:3.15 AS upx
ENV UPX_VERSION="3.96"
RUN \
  apk add bash build-base perl ucl-dev zlib-dev && \
  wget https://github.com/upx/upx/releases/download/v${UPX_VERSION}/upx-${UPX_VERSION}-src.tar.xz && \
  tar xf upx-${UPX_VERSION}-src.tar.xz && \
  cd upx-${UPX_VERSION}-src/ && make all && \
  mv ./src/upx.out /tmp/upx

FROM alpine:3.16 AS cleardns
COPY . /ClearDNS
RUN \
  apk add build-base cmake glib-dev && \
  cd ./ClearDNS/ && mkdir ./build/ && \
  cd ./build/ && cmake -DCMAKE_BUILD_TYPE=Release .. && make && \
  mv ../bin/cleardns /tmp/

FROM alpine:3.16 AS asset
COPY --from=adguardhome /tmp/AdGuardHome /release/
COPY --from=dnsproxy /tmp/dnsproxy /release/
COPY --from=overture /tmp/overture /release/
COPY --from=cleardns /tmp/cleardns /release/
COPY --from=upx /tmp/upx /usr/bin/
COPY . /ClearDNS
RUN \
  apk add gcc ucl && \
  cd /release/ && strip cleardns && \
  upx -9 AdGuardHome dnsproxy overture && \
  wget https://res.dnomd343.top/Share/gfwlist/gfwlist.txt && \
  wget https://res.dnomd343.top/Share/chinalist/chinalist.txt && \
  wget https://res.dnomd343.top/Share/chinalist/china-ip.txt && \
  tar czf asset.tar.gz gfwlist.txt chinalist.txt china-ip.txt && rm -f *.txt && \
  mkdir -p /asset/etc/ && mkdir -p /asset/usr/ && \
  cp -r /ClearDNS/overture/ /asset/etc/ && mv asset.tar.gz /asset/etc/overture/ && \
  mv /release/ /asset/usr/bin/ && mv /ClearDNS/load.sh /asset/usr/bin/load

FROM alpine:3.16
COPY --from=asset /asset /
RUN apk add --no-cache ca-certificates glib iptables ip6tables && \
echo -e "0\t4\t*\t*\t*\t/etc/overture/update.sh" > /var/spool/cron/crontabs/root
ENTRYPOINT ["cleardns"]
