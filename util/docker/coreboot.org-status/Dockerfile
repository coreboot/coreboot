FROM debian:sid AS builder

RUN apt-get update && apt-get install -y golang

ADD board-status.html/ /tmp/board-status.html
RUN cd /tmp/board-status.html && \
	go build

FROM debian:sid

RUN apt-get update && apt-get install -y python3 git bc && apt-get clean

ADD kconfig2html run.sh /opt/tools/
COPY --from=builder /tmp/board-status.html/board-status.html /opt/tools/
RUN git config --global --add safe.directory /data-in/coreboot.git
RUN git config --global --add safe.directory /data-in/board-status.git

ENTRYPOINT /opt/tools/run.sh
