ARG ALPINE_IMG="alpine:3.16"
ARG GOLANG_IMG="golang:1.18-alpine3.16"

FROM ${ALPINE_IMG} AS upx
ENV UPX_VER="3.96"
RUN sed -i 's/v3.\d\d/v3.15/' /etc/apk/repositories && apk add bash build-base perl ucl-dev zlib-dev
RUN wget https://github.com/upx/upx/releases/download/v${UPX_VER}/upx-${UPX_VER}-src.tar.xz && tar xf upx-${UPX_VER}-src.tar.xz
WORKDIR ./upx-${UPX_VER}-src/
RUN make -C ./src/ && mkdir -p /upx/bin/ && mv ./src/upx.out /upx/bin/upx && \
    mkdir -p /upx/lib/ && cd /usr/lib/ && cp -d ./libgcc_s.so* ./libstdc++.so* ./libucl.so* /upx/lib/

FROM ${GOLANG_IMG} AS adguardhome
ENV ADGUARD_VER="v0.107.12"
RUN apk add git make npm yarn && git clone https://github.com/AdguardTeam/AdGuardHome.git
WORKDIR ./AdGuardHome/
RUN git checkout ${ADGUARD_VER} && make js-deps
RUN make js-build
RUN make CHANNEL="release" VERSION=${ADGUARD_VER} VERBOSE=1 go-build && mv ./AdGuardHome /tmp/

FROM ${GOLANG_IMG} AS dnsproxy
ENV DNSPROXY_VER="0.44.0"
RUN wget https://github.com/AdguardTeam/dnsproxy/archive/refs/tags/v${DNSPROXY_VER}.tar.gz && tar xf v${DNSPROXY_VER}.tar.gz
WORKDIR ./dnsproxy-${DNSPROXY_VER}/
RUN env CGO_ENABLED=0 go build -v -trimpath -ldflags "-X main.VersionString=${DNSPROXY_VER} -s -w" && mv ./dnsproxy /tmp/

FROM ${GOLANG_IMG} AS overture
ENV OVERTURE_VER="1.8"
RUN wget https://github.com/shawn1m/overture/archive/refs/tags/v${OVERTURE_VER}.tar.gz && tar xf ./v${OVERTURE_VER}.tar.gz
WORKDIR ./overture-${OVERTURE_VER}/main/
RUN env CGO_ENABLED=0 go build -o overture -trimpath -ldflags "-X main.version=v${OVERTURE_VER} -s -w" ./main.go && mv ./overture /tmp/

#FROM ${ALPINE_IMG} AS cleardns
#COPY . /ClearDNS
#RUN apk add build-base cmake glib-dev
#RUN cd ./ClearDNS/ && mkdir ./build/ && \
#    cd ./build/ && cmake -DCMAKE_BUILD_TYPE=Release .. && make && \
#    mv ../bin/cleardns /tmp/ && strip /tmp/cleardns

FROM ${ALPINE_IMG} AS asset
RUN apk add xz && mkdir -p /asset/ && \
    wget https://res.dnomd343.top/Share/gfwlist/gfwlist.txt && \
    wget https://res.dnomd343.top/Share/chinalist/china-ip.txt && \
    wget https://res.dnomd343.top/Share/chinalist/chinalist.txt && \
    tar cJf /asset/assets.tar.xz ./*.txt
COPY --from=dnsproxy /tmp/dnsproxy /asset/usr/bin/
COPY --from=overture /tmp/overture /asset/usr/bin/
COPY --from=adguardhome /tmp/AdGuardHome /asset/usr/bin/
COPY --from=upx /upx/ /usr/
RUN ls /asset/usr/bin/* | xargs -P0 -n1 upx -9
#COPY --from=cleardns /tmp/cleardns /asset/usr/bin/

FROM ${ALPINE_IMG}
COPY --from=asset /asset/ /
#ENTRYPOINT ["cleardns"]
