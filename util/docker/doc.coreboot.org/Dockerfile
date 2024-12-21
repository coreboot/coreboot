FROM alpine:3.19

COPY makeSphinx.sh /makeSphinx.sh

RUN \
	apk update && \
	apk upgrade --no-cache && \
	apk add --no-cache \
		python3 \
		py3-pip \
		make \
		git \
		ttf-dejavu \
		fontconfig \
	&& chmod 755 /makeSphinx.sh

RUN adduser -D coreboot
RUN \
	mkdir /data-in /data-out && \
	chown -R coreboot:coreboot /data-in /data-out

USER coreboot

ENV VIRTUAL_ENV="/home/coreboot/python3"

RUN \
	python3 -m venv $VIRTUAL_ENV && \
	source ${VIRTUAL_ENV}/bin/activate && \
	pip3 install --upgrade --no-cache-dir pip && \
	pip3 install --no-cache-dir \
		jinja2==3.1.3 \
		myst-parser==2.0.0 \
		sphinx===7.2.6 \
		sphinx_autobuild===2024.2.4 \
		sphinx_rtd_theme===2.0.0

RUN git config --global --add safe.directory /data-in

# For Sphinx-autobuild
# Port 8000 - HTTP server
# Port 35729 - websockets connection to allow automatic browser reloads after each build
EXPOSE 8000 35729

ENTRYPOINT [ "/makeSphinx.sh" ]
CMD []
