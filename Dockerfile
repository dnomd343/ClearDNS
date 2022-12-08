ARG ALPINE="alpine:3.16"
ARG RUST="rust:1.64-alpine3.16"
ARG GOLANG="golang:1.18-alpine3.16"

FROM ${ALPINE} AS upx
RUN apk add build-base cmake
ENV UPX="4.0.1"
RUN wget https://github.com/upx/upx/releases/download/v${UPX}/upx-${UPX}-src.tar.xz && tar xf upx-${UPX}-src.tar.xz
WORKDIR ./upx-${UPX}-src/
RUN make UPX_CMAKE_CONFIG_FLAGS=-DCMAKE_EXE_LINKER_FLAGS=-static
WORKDIR ./build/release/
RUN strip upx && mv upx /tmp/

FROM ${GOLANG} AS adguard
ENV ADGUARD="0.107.20"
RUN apk add git make npm yarn
RUN git clone https://github.com/AdguardTeam/AdGuardHome.git -b v${ADGUARD} --depth=1
WORKDIR ./AdGuardHome/
RUN make js-deps
RUN make js-build
RUN make CHANNEL="release" VERBOSE=1 go-build && mv AdGuardHome /tmp/
COPY --from=upx /tmp/upx /usr/bin/
RUN upx -9 /tmp/AdGuardHome

FROM ${GOLANG} AS overture
ENV OVERTURE="1.8"
RUN wget https://github.com/shawn1m/overture/archive/refs/tags/v${OVERTURE}.tar.gz && tar xf v${OVERTURE}.tar.gz
WORKDIR ./overture-${OVERTURE}/main/
RUN env CGO_ENABLED=0 go build -v -trimpath -ldflags "-X main.version=v${OVERTURE} -s -w" && mv main /tmp/overture
COPY --from=upx /tmp/upx /usr/bin/
RUN upx -9 /tmp/overture

FROM ${GOLANG} AS dnsproxy
ENV DNSPROXY="0.46.4"
RUN wget https://github.com/AdguardTeam/dnsproxy/archive/refs/tags/v${DNSPROXY}.tar.gz && tar xf v${DNSPROXY}.tar.gz
WORKDIR ./dnsproxy-${DNSPROXY}/
RUN env CGO_ENABLED=0 go build -v -trimpath -ldflags "-X main.VersionString=${DNSPROXY} -s -w" && mv dnsproxy /tmp/
COPY --from=upx /tmp/upx /usr/bin/
RUN upx -9 /tmp/dnsproxy

FROM ${RUST} AS to-json
COPY ./src/to-json/ /to-json/
WORKDIR /to-json/
RUN cargo build --release
RUN cp ./target/release/libto_json.a /

FROM ${ALPINE} AS cleardns
RUN apk add build-base cmake
COPY ./ /cleardns/
COPY --from=to-json /libto_json.a /cleardns/src/to-json/target/release/
WORKDIR /cleardns/bin/
RUN cmake -DCMAKE_EXE_LINKER_FLAGS=-static -DCMAKE_BUILD_TYPE=Release .. && make
RUN strip cleardns && mv cleardns /tmp/
COPY --from=upx /tmp/upx /usr/bin/
RUN upx -9 /tmp/cleardns

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

FROM ${ALPINE}
COPY --from=build /release/ /
WORKDIR /cleardns/
ENTRYPOINT ["cleardns"]
