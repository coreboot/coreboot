# This dockerfile is not meant to be used directly by docker.  The
# {{}} varibles are replaced with values by the makefile.  Please generate
# the docker image for this file by running:
#
#   make coreboot-sdk
#
# Variables can be updated on the make command line or left blank to use
# the default values set by the makefile.
#
#  SDK_VERSION is used to name the version of the coreboot sdk to use.
#              Typically, this corresponds to the toolchain version.  This
#              is used to identify this docker image.
#  DOCKER_COMMIT is the coreboot Commit-ID to build the toolchain from.

FROM debian:sid
MAINTAINER Martin Roth <martin@coreboot.org>

RUN \
	useradd -p locked -m coreboot && \
	apt-get -qq update && \
	apt-get -qqy install gcc g++ make patch python diffutils bison flex \
		git doxygen ccache subversion p7zip-full unrar-free \
		m4 wget curl bzip2 vim-common cmake xz-utils pkg-config \
		dh-autoreconf unifont \
		libssl1.0-dev libgmp-dev zlib1g-dev libpci-dev liblzma-dev \
		libyaml-dev libncurses5-dev uuid-dev libusb-dev libftdi-dev \
		libusb-1.0-0-dev libreadline-dev libglib2.0-dev libgmp-dev \
		libelf-dev libxml2-dev libfreetype6-dev libisl-dev && \
	apt-get clean

RUN \
	cd /root && \
	git clone http://review.coreboot.org/coreboot && \
	cd coreboot/util/crossgcc && \
	git checkout {{DOCKER_COMMIT}} && \
	make all_without_gdb CPUS=$(nproc) DEST=/opt/xgcc BUILDGCC_OPTIONS=-b && \
	cd /root && \
	rm -rf coreboot

RUN mkdir /home/coreboot/.ccache && \
	chown coreboot:coreboot /home/coreboot/.ccache && \
	mkdir /home/coreboot/cb_build && \
	chown coreboot:coreboot /home/coreboot/cb_build
VOLUME /home/coreboot/.ccache

ENV PATH $PATH:/opt/xgcc/bin
ENV SDK_VERSION={{SDK_VERSION}}
ENV SDK_COMMIT={{DOCKER_COMMIT}}
USER coreboot
