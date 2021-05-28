FROM alpine as asset
COPY ./asset.sh /
RUN apk --update add --no-cache curl wget && \
    sh /asset.sh

FROM alpine
COPY ./init.sh /
COPY ./overture /etc/overture/
COPY --from=asset /tmp/asset/ /usr/bin/
RUN mkdir -p /etc/cleardns && \
    sed -i '$i\0\t2\t*\t*\t*\t/etc/overture/update.sh' /var/spool/cron/crontabs/root
CMD ["sh","/init.sh"]
