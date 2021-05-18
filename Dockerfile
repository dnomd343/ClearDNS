FROM alpine as tools
COPY . /tmp/cleardns
RUN apk --update add --no-cache curl wget && \
    sh /tmp/cleardns/tools.sh

FROM alpine
COPY . /tmp/cleardns
COPY --from=tools /tmp/tools/ /usr/bin/
RUN mv /tmp/cleardns/overture/ /etc/ && \
    mv /tmp/cleardns/smartdns/ /etc/ && \
    mkdir -p /opt/AdGuardHome && \
    mkdir -p /etc/smartdns/expose && \
    mkdir /etc/cleardns && \
    mv /tmp/cleardns/init.sh / && \
    rm -rf /tmp/cleardns && \
    sed -i '$i\0\t0\t*\t*\t*\t/etc/overture/update.sh' /var/spool/cron/crontabs/root
CMD ["sh","/init.sh"]
