ARG ALPINE="alpine:3.19"
ARG NODE="node:16-alpine3.18"
ARG RUST="rust:1.78-alpine3.19"
ARG GOLANG="golang:1.22-alpine3.19"

FROM ${GOLANG} AS dnsproxy
ENV DNSPROXY="0.71.1"
RUN wget https://github.com/AdguardTeam/dnsproxy/archive/v${DNSPROXY}.tar.gz -O- | tar xz
WORKDIR ./dnsproxy-${DNSPROXY}/
RUN go get
RUN env CGO_ENABLED=0 go build -v -trimpath -ldflags "-X main.VersionString=${DNSPROXY} -s -w"
RUN mv dnsproxy /tmp/

FROM ${GOLANG} AS overture
ENV OVERTURE="1.8"
RUN wget https://github.com/shawn1m/overture/archive/v${OVERTURE}.tar.gz -O- | tar xz
WORKDIR ./overture-${OVERTURE}/main/
RUN go get
RUN env CGO_ENABLED=0 go build -v -trimpath -ldflags "-X main.version=v${OVERTURE} -s -w"
RUN mv main /tmp/overture

FROM ${ALPINE} AS adguard-src
RUN apk add git
ENV ADGUARD="0.107.52"
RUN git clone https://github.com/AdguardTeam/AdGuardHome.git -b v${ADGUARD} --depth=1

FROM ${NODE} AS adguard-web
RUN apk add make
COPY --from=adguard-src /AdGuardHome/ /AdGuardHome/
WORKDIR /AdGuardHome/
RUN echo '.nav-item .order-4 {display: none;}' >> ./client/src/components/Header/Header.css
RUN make js-deps
RUN make js-build
RUN mv ./build/static/ /tmp/

FROM ${GOLANG} AS adguard
RUN apk add git make
COPY --from=adguard-src /AdGuardHome/ /AdGuardHome/
WORKDIR /AdGuardHome/
RUN go get
COPY --from=adguard-web /tmp/static/ ./build/static/
RUN make CHANNEL="release" VERBOSE=1 go-build
RUN mv AdGuardHome /tmp/

FROM ${RUST} AS rust-mods
RUN apk add musl-dev
COPY ./src/ /cleardns/
WORKDIR /cleardns/
RUN cargo fetch
RUN cargo build --release
RUN mv ./target/release/*.a /tmp/

FROM ${ALPINE} AS cleardns
RUN apk add gcc git make cmake musl-dev
COPY ./ /cleardns/
COPY --from=rust-mods /tmp/libassets.a /cleardns/src/target/release/
COPY --from=rust-mods /tmp/libto_json.a /cleardns/src/target/release/
WORKDIR /cleardns/bin/
RUN cmake -DCMAKE_EXE_LINKER_FLAGS=-static .. && make && strip cleardns
RUN mv cleardns /tmp/

FROM ${ALPINE} AS assets
RUN apk add xz
RUN wget https://cdn.dnomd343.top/cleardns/gfwlist.txt.xz
RUN wget https://cdn.dnomd343.top/cleardns/china-ip.txt.xz
RUN wget https://cdn.dnomd343.top/cleardns/chinalist.txt.xz
RUN xz -d *.xz && tar cJf /tmp/assets.tar.xz gfwlist.txt china-ip.txt chinalist.txt

FROM ${ALPINE} AS release
RUN apk add upx xz
COPY --from=assets /tmp/assets.tar.xz /release/
COPY --from=cleardns /tmp/cleardns /release/usr/bin/
COPY --from=dnsproxy /tmp/dnsproxy /release/usr/bin/
COPY --from=overture /tmp/overture /release/usr/bin/
COPY --from=adguard /tmp/AdGuardHome /release/usr/bin/
WORKDIR /release/usr/bin/
RUN ls | xargs -n1 -P0 upx -9

FROM ${ALPINE}
COPY --from=release /release/ /
WORKDIR /cleardns/
ENTRYPOINT ["cleardns"]
