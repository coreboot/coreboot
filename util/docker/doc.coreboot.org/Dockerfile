FROM debian:stable
RUN apt-get update && apt-get install -y make python-sphinx python-recommonmark python-sphinx-rtd-theme && apt-get clean
USER nobody
VOLUME /data-in /data-out
ENTRYPOINT bash -c "cd /data-in/Documentation && make sphinx BUILDDIR=/tmp/build && rm -rf /data-out/* && mv /tmp/build/html/* /data-out/"
