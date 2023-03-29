ARG ALPINE="alpine:3.17"
ARG RUST="rust:1.67-alpine3.17"
ARG GOLANG="golang:1.19-alpine3.17"

FROM ${GOLANG} AS dnsproxy
ENV DNSPROXY="0.48.0"
RUN wget https://github.com/AdguardTeam/dnsproxy/archive/refs/tags/v${DNSPROXY}.tar.gz && tar xf v${DNSPROXY}.tar.gz
WORKDIR ./dnsproxy-${DNSPROXY}/
RUN go get
RUN env CGO_ENABLED=0 go build -v -trimpath -ldflags "-X main.VersionString=${DNSPROXY} -s -w" && mv dnsproxy /tmp/

FROM ${GOLANG} AS overture
ENV OVERTURE="1.8"
RUN wget https://github.com/shawn1m/overture/archive/refs/tags/v${OVERTURE}.tar.gz && tar xf v${OVERTURE}.tar.gz
WORKDIR ./overture-${OVERTURE}/main/
RUN go get
RUN env CGO_ENABLED=0 go build -v -trimpath -ldflags "-X main.version=v${OVERTURE} -s -w" && mv main /tmp/overture

FROM ${ALPINE} AS adguard-src
RUN apk add git
ENV ADGUARD="0.107.26"
RUN git clone https://github.com/AdguardTeam/AdGuardHome.git -b v${ADGUARD} --depth=1

FROM ${ALPINE} AS adguard-web
RUN apk add make npm
COPY --from=adguard-src /AdGuardHome/ /AdGuardHome/
WORKDIR /AdGuardHome/
RUN make js-deps
# TODO: for node18, remove the OpenSSL compatibility option after AdGuardHome project upgrade
RUN env NODE_OPTIONS="--openssl-legacy-provider" make js-build
RUN mv ./build/static/ /tmp/

FROM ${GOLANG} AS adguard
RUN apk add git make
COPY --from=adguard-src /AdGuardHome/ /AdGuardHome/
WORKDIR /AdGuardHome/
RUN go get
COPY --from=adguard-web /tmp/static/ ./build/static/
RUN make CHANNEL="release" VERBOSE=1 go-build && mv AdGuardHome /tmp/

FROM ${RUST} AS rust-mods
RUN apk add musl-dev
COPY ./src/ /cleardns/
WORKDIR /cleardns/
RUN cargo fetch
RUN cargo build --release && mv ./target/release/*.a /tmp/

FROM ${ALPINE} AS cleardns
RUN apk add gcc git make cmake musl-dev
COPY ./ /cleardns/
COPY --from=rust-mods /tmp/libassets.a /cleardns/src/target/release/
COPY --from=rust-mods /tmp/libto_json.a /cleardns/src/target/release/
WORKDIR /cleardns/bin/
RUN cmake -DCMAKE_EXE_LINKER_FLAGS=-static .. && make && strip cleardns && mv cleardns /tmp/

FROM ${ALPINE} AS build
RUN apk add upx xz
WORKDIR /release/
RUN wget https://cdn.dnomd343.top/cleardns/gfwlist.txt.xz && \
    wget https://cdn.dnomd343.top/cleardns/china-ip.txt.xz && \
    wget https://cdn.dnomd343.top/cleardns/chinalist.txt.xz && \
    xz -d *.xz && tar cJf assets.tar.xz *.txt && rm *.txt
COPY --from=cleardns /tmp/cleardns /release/usr/bin/
COPY --from=dnsproxy /tmp/dnsproxy /release/usr/bin/
COPY --from=overture /tmp/overture /release/usr/bin/
COPY --from=adguard /tmp/AdGuardHome /release/usr/bin/
WORKDIR /release/usr/bin/
RUN ls | xargs -n1 -P0 upx -9

FROM ${ALPINE}
COPY --from=build /release/ /
WORKDIR /cleardns/
ENTRYPOINT ["cleardns"]
