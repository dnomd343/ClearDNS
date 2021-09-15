FROM alpine as asset
COPY ./asset.sh /
RUN apk --update add --no-cache curl wget && \
    sh /asset.sh

FROM alpine
COPY ["./overture/", "./init.sh", "/etc/overture/"]
COPY --from=asset /tmp/asset/ /usr/bin/
RUN mv /etc/overture/init.sh / && \
    mkdir -p /etc/cleardns && \
    sed -i '$i\0\t4\t*\t*\t*\t/etc/overture/update.sh' /var/spool/cron/crontabs/root
CMD ["sh","/init.sh"]
