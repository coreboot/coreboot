FROM debian:sid
MAINTAINER Patrick Georgi <patrick@georgi-clan.de>
RUN \
	useradd -p locked -m coreboot && \
	apt-get -qq update && \
	apt-get -qqy install git gcc g++ make patch python diffutils bison flex m4 wget bzip2 libssl-dev libgmp-dev vim-common cmake xz-utils zlib1g-dev && \
	apt-get clean && \
	cd /root && \
	git clone http://review.coreboot.org/coreboot && \
	cd coreboot/util/crossgcc && \
	make all_without_gdb CPUS=$(grep -c "^processor\>" /proc/cpuinfo) DEST=/opt/xgcc && \
	cd /root && \
	rm -rf coreboot
ENV PATH $PATH:/opt/xgcc/bin
USER coreboot
