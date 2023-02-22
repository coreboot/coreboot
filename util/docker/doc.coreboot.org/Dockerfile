FROM alpine:3.18.3

COPY makeSphinx.sh /makeSphinx.sh

RUN apk add --no-cache python3 py3-pip make bash git openjdk8-jre ttf-dejavu fontconfig \
 && pip3 install --upgrade --no-cache-dir pip \
 && pip3 install --no-cache-dir  \
  jinja2==3.1.2 \
  recommonmark===0.7.1\
  sphinx===6.2.1 \
  sphinxcontrib-ditaa===1.0.2 \
  sphinx_autobuild===2021.3.14 \
  sphinx_rtd_theme===1.2.2 \
 && chmod 755 /makeSphinx.sh

ADD https://github.com/stathissideris/ditaa/releases/download/v0.11.0/ditaa-0.11.0-standalone.jar \
    /usr/lib/ditaa-0.11.0-standalone.jar

ADD ditaa.sh /usr/bin/ditaa

VOLUME /data-in /data-out

# For Sphinx-autobuild
# Port 8000 - HTTP server
# Port 35729 - websockets connection to allow automatic browser reloads after each build
EXPOSE 8000 35729

ENTRYPOINT ["/bin/bash", "/makeSphinx.sh"]
CMD []
