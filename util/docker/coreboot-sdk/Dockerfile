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
		cscope \
		curl \
		device-tree-compiler \
		dh-autoreconf \
		diffutils \
		doxygen \
		exuberant-ctags \
		flex \
		g++ \
		gawk \
		gcc \
		git \
		gnat \
		golang \
		graphviz \
		libcrypto++-dev \
		libcurl4 \
		libcurl4-openssl-dev \
		libelf-dev \
		libfreetype6-dev \
		libftdi-dev \
		libftdi1-dev \
		libglib2.0-dev \
		libgmp-dev \
		libjaylink-dev \
		liblzma-dev \
		libncurses5-dev \
		libpci-dev \
		libreadline-dev \
		libssl-dev \
		libusb-1.0-0-dev \
		libusb-dev \
		libxml2-dev \
		libyaml-dev \
		m4 \
		make \
		msitools \
		nasm \
		openssl \
		patch \
		pbzip2 \
		pkg-config \
		python2 \
		python3 \
		qemu \
		rsync \
		shellcheck \
		subversion \
		unifont \
		uuid-dev \
		vim-common \
		wget \
		xz-utils \
		zlib1g-dev \
	&& apt-get clean \
	&& ln -s /usr/bin/automake /usr/bin/automake-1.15 \
	&& ln -s /usr/bin/aclocal /usr/bin/aclocal-1.15

RUN \
	cd /root && \
	git clone https://review.coreboot.org/coreboot && \
	cd coreboot && \
	git checkout {{DOCKER_COMMIT}}

RUN \
	if echo {{CROSSGCC_PARAM}} | grep -q ^all; then \
		make -C/root/coreboot/util/crossgcc/ build_clang \
			BUILD_LANGUAGES=c,ada CPUS=$(nproc) DEST=/opt/xgcc; \
	fi

RUN \
	make -C/root/coreboot/util/crossgcc/ {{CROSSGCC_PARAM}} \
		BUILD_LANGUAGES=c,ada CPUS=$(nproc) DEST=/opt/xgcc && \
	rm -rf /root/coreboot

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
