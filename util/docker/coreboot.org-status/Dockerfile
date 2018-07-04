FROM debian:sid

RUN apt-get update && apt-get install -y python git bc && apt-get clean

ADD board-status.html kconfig2html run.sh /opt/tools/

ENTRYPOINT /opt/tools/run.sh
