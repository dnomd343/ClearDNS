ARG ALPINE="alpine:3.16"
ARG GOLANG="golang:1.18-alpine3.16"

FROM ${ALPINE} AS upx
ENV UPX="3.96"
RUN sed -i 's/v3.\d\d/v3.15/' /etc/apk/repositories && apk add bash build-base perl ucl-dev zlib-dev
RUN wget https://github.com/upx/upx/releases/download/v${UPX}/upx-${UPX}-src.tar.xz && tar xf upx-${UPX}-src.tar.xz
WORKDIR ./upx-${UPX}-src/
RUN make -C ./src/ && mkdir -p /upx/bin/ && mv ./src/upx.out /upx/bin/upx && \
    mkdir -p /upx/lib/ && cd /usr/lib/ && cp -d libgcc_s.so* libstdc++.so* libucl.so* /upx/lib/

FROM ${GOLANG} AS adguard
ENV ADGUARD="v0.107.14"
RUN apk add git make npm yarn && git clone https://github.com/AdguardTeam/AdGuardHome.git
WORKDIR ./AdGuardHome/
RUN git checkout ${ADGUARD} && make js-deps
RUN make js-build
RUN make CHANNEL="release" VERSION=${ADGUARD} VERBOSE=1 go-build && mv AdGuardHome /tmp/
COPY --from=upx /upx/ /usr/
RUN upx -9 /tmp/AdGuardHome

FROM ${GOLANG} AS overture
ENV OVERTURE="1.8"
RUN wget https://github.com/shawn1m/overture/archive/refs/tags/v${OVERTURE}.tar.gz && tar xf ./v${OVERTURE}.tar.gz
WORKDIR ./overture-${OVERTURE}/main/
RUN env CGO_ENABLED=0 go build -v -trimpath -ldflags "-X main.version=v${OVERTURE} -s -w" && mv main /tmp/overture
COPY --from=upx /upx/ /usr/
RUN upx -9 /tmp/overture

FROM ${GOLANG} AS dnsproxy
ENV DNSPROXY="0.45.1"
RUN wget https://github.com/AdguardTeam/dnsproxy/archive/refs/tags/v${DNSPROXY}.tar.gz && tar xf v${DNSPROXY}.tar.gz
WORKDIR ./dnsproxy-${DNSPROXY}/
RUN env CGO_ENABLED=0 go build -v -trimpath -ldflags "-X main.VersionString=${DNSPROXY} -s -w" && mv dnsproxy /tmp/
COPY --from=upx /upx/ /usr/
RUN upx -9 /tmp/dnsproxy

FROM ${GOLANG} AS toJSON
COPY ./toJSON/ /toJSON/
WORKDIR /toJSON/
RUN env CGO_ENABLED=0 go build -v -trimpath -ldflags "-s -w" && mv toJSON /tmp/
COPY --from=upx /upx/ /usr/
RUN upx -9 /tmp/toJSON

FROM ${ALPINE} AS cleardns
RUN apk add build-base cmake
COPY ./ /ClearDNS/
WORKDIR /ClearDNS/build/
RUN cmake -DCMAKE_BUILD_TYPE=Release .. && make && mv ../bin/cleardns /tmp/ && strip /tmp/cleardns

FROM ${ALPINE} AS build
RUN apk add xz
WORKDIR /release/
RUN wget https://res.dnomd343.top/Share/cleardns/gfwlist.txt.xz && \
    wget https://res.dnomd343.top/Share/cleardns/china-ip.txt.xz && \
    wget https://res.dnomd343.top/Share/cleardns/chinalist.txt.xz && \
    xz -d *.xz && tar cJf assets.tar.xz *.txt && rm *.txt
COPY --from=dnsproxy /tmp/dnsproxy /release/usr/bin/
COPY --from=overture /tmp/overture /release/usr/bin/
COPY --from=adguard /tmp/AdGuardHome /release/usr/bin/
COPY --from=cleardns /tmp/cleardns /release/usr/bin/
COPY --from=toJSON /tmp/toJSON /release/usr/bin/

FROM ${ALPINE}
COPY --from=build /release/ /
WORKDIR /cleardns/
ENTRYPOINT ["cleardns"]
