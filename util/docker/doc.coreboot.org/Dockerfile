FROM alpine:3.8

COPY makeSphinx.sh /makeSphinx.sh

RUN apk add --no-cache python3 make bash git \
 && pip3 install --upgrade --no-cache-dir pip \
 && pip3 install --no-cache-dir  \
    sphinx===1.7.7 \
    sphinx_rtd_theme===0.4.1 \
    recommonmark===0.4.0 \
    sphinx_autobuild===0.7.1 \
 && chmod 755 /makeSphinx.sh

VOLUME /data-in /data-out

# For Sphinx-autobuild
# Port 8000 - HTTP server
# Port 35729 - websockets connection to allow automatic browser reloads after each build
EXPOSE 8000 35729

ENTRYPOINT ["/bin/bash", "/makeSphinx.sh"]
CMD []
