ARG ALPINE="alpine:3.16"
ARG GOLANG="golang:1.18-alpine3.16"

FROM ${ALPINE} AS apk
WORKDIR /apk/
RUN echo -e "cd \`dirname \$0\`\napk add --no-network \\" > setup && chmod +x setup && \
    apk update && apk fetch -R build-base cmake | grep -oE '\S+$' | awk '{print "./"$0".apk \\"}' >> setup

FROM ${ALPINE} AS upx
COPY --from=apk /apk/ /apk/
RUN apk add git && /apk/setup
RUN git clone https://github.com/dnomd343/upx.git
WORKDIR ./upx/
RUN git submodule update --init && rm -rf ./.git/
RUN make UPX_CMAKE_CONFIG_FLAGS=-DCMAKE_EXE_LINKER_FLAGS=-static && \
    mv ./build/release/upx /tmp/ && strip /tmp/upx

FROM ${GOLANG} AS adguard
ENV ADGUARD="v0.107.15"
RUN apk add git make npm yarn && git clone https://github.com/AdguardTeam/AdGuardHome.git
WORKDIR ./AdGuardHome/
RUN git checkout ${ADGUARD} && make js-deps
RUN make js-build
RUN make CHANNEL="release" VERSION=${ADGUARD} VERBOSE=1 go-build && mv AdGuardHome /tmp/
COPY --from=upx /tmp/upx /usr/bin/
RUN upx -9 /tmp/AdGuardHome

FROM ${GOLANG} AS overture
ENV OVERTURE="1.8"
RUN wget https://github.com/shawn1m/overture/archive/refs/tags/v${OVERTURE}.tar.gz && tar xf ./v${OVERTURE}.tar.gz
WORKDIR ./overture-${OVERTURE}/main/
RUN env CGO_ENABLED=0 go build -v -trimpath -ldflags "-X main.version=v${OVERTURE} -s -w" && mv main /tmp/overture
COPY --from=upx /tmp/upx /usr/bin/
RUN upx -9 /tmp/overture

FROM ${GOLANG} AS dnsproxy
ENV DNSPROXY="0.45.2"
RUN wget https://github.com/AdguardTeam/dnsproxy/archive/refs/tags/v${DNSPROXY}.tar.gz && tar xf v${DNSPROXY}.tar.gz
WORKDIR ./dnsproxy-${DNSPROXY}/
RUN env CGO_ENABLED=0 go build -v -trimpath -ldflags "-X main.VersionString=${DNSPROXY} -s -w" && mv dnsproxy /tmp/
COPY --from=upx /tmp/upx /usr/bin/
RUN upx -9 /tmp/dnsproxy

FROM ${GOLANG} AS toJSON
COPY ./toJSON/ /toJSON/
WORKDIR /toJSON/
RUN env CGO_ENABLED=0 go build -v -trimpath -ldflags "-s -w" && mv toJSON /tmp/
COPY --from=upx /tmp/upx /usr/bin/
RUN upx -9 /tmp/toJSON

FROM ${ALPINE} AS cleardns
COPY --from=apk /apk/ /apk/
RUN /apk/setup
COPY ./ /ClearDNS/
WORKDIR /ClearDNS/bin/
RUN cmake -DCMAKE_EXE_LINKER_FLAGS=-static -DCMAKE_BUILD_TYPE=Release .. && \
    make && strip cleardns && mv cleardns /tmp/

FROM ${ALPINE} AS build
RUN apk add xz
WORKDIR /release/
RUN wget https://res.dnomd343.top/Share/cleardns/gfwlist.txt.xz && \
    wget https://res.dnomd343.top/Share/cleardns/china-ip.txt.xz && \
    wget https://res.dnomd343.top/Share/cleardns/chinalist.txt.xz && \
    xz -d *.xz && tar cJf assets.tar.xz *.txt && rm *.txt
COPY --from=adguard /tmp/AdGuardHome /release/usr/bin/
COPY --from=overture /tmp/overture /release/usr/bin/
COPY --from=dnsproxy /tmp/dnsproxy /release/usr/bin/
COPY --from=cleardns /tmp/cleardns /release/usr/bin/
COPY --from=toJSON /tmp/toJSON /release/usr/bin/

FROM ${ALPINE}
COPY --from=build /release/ /
WORKDIR /cleardns/
ENTRYPOINT ["cleardns"]
