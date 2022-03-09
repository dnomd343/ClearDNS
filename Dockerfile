FROM alpine:3.15
COPY . /tmp/ClearDNS
ENV OVERTURE_VERSION="v1.8"
ENV DNSPROXY_VERSION="v0.41.3"
ENV ADGUARD_VERSION="v0.107.5"
RUN \
apk add git build-base make cmake glib-dev go npm nodejs yarn upx && \
\
mkdir /tmp/release/ && cd /tmp/ && \
git clone https://github.com/shawn1m/overture.git && \
git clone https://github.com/AdguardTeam/dnsproxy.git && \
git clone https://github.com/AdguardTeam/AdGuardHome.git && \
\
cd /tmp/ClearDNS/ && mkdir ./build/ && \
cd ./build/ && cmake -DCMAKE_BUILD_TYPE=Release .. && make && \
mv ../bin/cleardns /tmp/release/ && \
\
cd /tmp/overture/ && git checkout $OVERTURE_VERSION && \
env CGO_ENABLED=0 go build -o overture -trimpath -ldflags "-X main.version=$OVERTURE_VERSION -s -w " ./main/main.go && \
mv ./overture /tmp/release/ && \
\
cd /tmp/dnsproxy/ && git checkout $DNSPROXY_VERSION && \
env CGO_ENABLED=0 go build -trimpath -ldflags "-X main.VersionString=$DNSPROXY_VERSION -s -w" && \
mv ./dnsproxy /tmp/release/ && \
\
cd /tmp/AdGuardHome/ && git checkout $ADGUARD_VERSION && \
make VERSION=$ADGUARD_VERSION && \
mv ./AdGuardHome /tmp/release/ && \
\
cd /tmp/release/ && \
strip * && upx -9 *

#
#FROM alpine as asset
#COPY ./asset.sh /
#RUN apk --update add --no-cache curl wget && \
#    sh /asset.sh
#
#FROM alpine
#COPY ["./overture/", "./init.sh", "/etc/overture/"]
#COPY --from=asset /tmp/asset/ /usr/bin/
#RUN mv /etc/overture/init.sh / && \
#    mkdir -p /etc/cleardns && \
#    sed -i '$i\0\t4\t*\t*\t*\t/etc/overture/update.sh' /var/spool/cron/crontabs/root
#CMD ["sh","/init.sh"]
