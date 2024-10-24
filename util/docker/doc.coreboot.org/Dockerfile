FROM alpine:3.19.1

COPY makeSphinx.sh /makeSphinx.sh

RUN \
	apk add --no-cache \
		python3 \
		py3-pip \
		make \
		bash \
		git \
		openjdk8-jre \
		ttf-dejavu \
		fontconfig \
	&& chmod 755 /makeSphinx.sh

RUN adduser -D coreboot

USER coreboot

ENV VIRTUAL_ENV="/home/coreboot/python3"
ENV PATH=$VIRTUAL_ENV/bin:$PATH:/home/coreboot/.local/bin

RUN \
	python3 -m venv $VIRTUAL_ENV && \
	echo "source ${VIRTUAL_ENV}/bin/activate"  >> /home/coreboot/.bashrc && \
	pip3 install --upgrade --no-cache-dir pip && \
	pip3 install --no-cache-dir \
		jinja2==3.1.3 \
		myst-parser==2.0.0 \
		sphinx===7.2.6 \
		sphinxcontrib-ditaa===1.0.2 \
		sphinx_autobuild===2024.2.4 \
		sphinx_rtd_theme===2.0.0

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
