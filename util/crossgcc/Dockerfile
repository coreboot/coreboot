FROM debian:sid
MAINTAINER Martin Roth <gaumless@gmail.com>

RUN \
	useradd -p locked -m coreboot && \
	apt-get -qq update && \
	apt-get -qq upgrade && \
	apt-get -qqy install gcc g++ gnat-6 make patch python diffutils bison \
		flex git doxygen ccache subversion p7zip-full unrar-free \
		m4 wget curl bzip2 vim-common cmake xz-utils pkg-config \
		dh-autoreconf unifont \
		libssl-dev libgmp-dev zlib1g-dev libpci-dev liblzma-dev \
		libyaml-dev libncurses5-dev uuid-dev libusb-dev libftdi-dev \
		libusb-1.0-0-dev libreadline-dev libglib2.0-dev libgmp-dev \
		libelf-dev libxml2-dev libfreetype6-dev && \
	apt-get clean

COPY buildgcc Makefile* root/coreboot/util/crossgcc/
COPY patches/ /root/coreboot/util/crossgcc/patches/
COPY sum/ /root/coreboot/util/crossgcc/sum/
COPY tarballs/ /root/coreboot/util/crossgcc/tarballs/

RUN \
	cd /root/coreboot/util/crossgcc && \
	make all_without_gdb \
        	BUILD_LANGUAGES=c,ada CPUS=$(nproc) DEST=/opt/xgcc && \
	cd /root && \
	rm -rf coreboot

RUN mkdir /home/coreboot/.ccache && \
	chown coreboot:coreboot /home/coreboot/.ccache && \
	mkdir /home/coreboot/cb_build && \
	chown coreboot:coreboot /home/coreboot/cb_build
VOLUME /home/coreboot/.ccache

ENV PATH $PATH:/opt/xgcc/bin
USER coreboot
