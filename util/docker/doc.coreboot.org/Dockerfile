FROM alpine:3.8

COPY makeSphinx.sh /makeSphinx.sh

ADD https://sourceforge.net/projects/ditaa/files/ditaa/0.9/ditaa0_9.zip/download /tmp/ditaa.zip

RUN apk add --no-cache python3 make bash git openjdk8-jre ttf-dejavu fontconfig \
 && pip3 install --upgrade --no-cache-dir pip \
 && pip3 install --no-cache-dir  \
    sphinx===1.8.3 \
    sphinx_rtd_theme===0.4.2 \
    recommonmark===0.5.0 \
    sphinx_autobuild===0.7.1 \
    sphinxcontrib-ditaa===0.6 \
 && chmod 755 /makeSphinx.sh
RUN cd /tmp \
 && unzip ditaa.zip \
 && mv ditaa0_9.jar /usr/lib
ADD ditaa.sh /usr/bin/ditaa

VOLUME /data-in /data-out

# For Sphinx-autobuild
# Port 8000 - HTTP server
# Port 35729 - websockets connection to allow automatic browser reloads after each build
EXPOSE 8000 35729

ENTRYPOINT ["/bin/bash", "/makeSphinx.sh"]
CMD []
