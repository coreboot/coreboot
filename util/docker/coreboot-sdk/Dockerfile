# This dockerfile is not meant to be used directly by docker.  The
# {{}} variables are replaced with values by the makefile.  Please generate
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
	apt-get -qqy install \
		bc \
		bison \
		bzip2 \
		ccache \
		cmake \
		curl \
		device-tree-compiler \
		dh-autoreconf \
		diffutils \
		doxygen \
		flex \
		g++ \
		gawk \
		gcc \
		git \
		gnat-6 \
		libelf-dev \
		libfreetype6-dev \
		libftdi-dev \
		libftdi1-dev \
		libglib2.0-dev \
		libgmp-dev \
		libgmp-dev \
		libisl-dev \
		liblzma-dev \
		libncurses5-dev \
		libpci-dev \
		libreadline-dev \
		libssl1.0-dev \
		libusb-1.0-0-dev \
		libusb-dev \
		libxml2-dev \
		libyaml-dev \
		m4 \
		make \
		msitools \
		nasm \
		openssl \
		p7zip-full \
		patch \
		pkg-config \
		python \
		qemu \
		rsync \
		shellcheck \
		subversion \
		unifont \
		unrar-free \
		uuid-dev \
		vim-common \
		wget \
		xz-utils \
		zlib1g-dev \
	&& apt-get clean

RUN \
	cd /root && \
	git clone https://review.coreboot.org/coreboot && \
	cd coreboot/util/crossgcc && \
	git checkout {{DOCKER_COMMIT}} && \
	make {{CROSSGCC_PARAM}} \
		BUILD_LANGUAGES=c,ada CPUS=$(nproc) DEST=/opt/xgcc && \
	cd /root && \
	rm -rf coreboot

RUN mkdir /home/coreboot/.ccache && \
	chown coreboot:coreboot /home/coreboot/.ccache && \
	mkdir /home/coreboot/cb_build && \
	chown coreboot:coreboot /home/coreboot/cb_build && \
	echo "export PATH=$PATH:/opt/xgcc/bin" >> /home/coreboot/.bashrc && \
	echo "export SDK_VERSION={{SDK_VERSION}}" >> /home/coreboot/.bashrc && \
	echo "export SDK_COMMIT={{DOCKER_COMMIT}}" >> /home/coreboot/.bashrc

VOLUME /home/coreboot/.ccache

ENV PATH $PATH:/opt/xgcc/bin
ENV SDK_VERSION={{SDK_VERSION}}
ENV SDK_COMMIT={{DOCKER_COMMIT}}
USER coreboot
