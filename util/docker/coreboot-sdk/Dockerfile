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

FROM debian:sid AS coreboot-sdk

RUN \
	useradd -p locked -m coreboot && \
	apt-get -qq update && \
	apt-get -qqy install \
		bc \
		bison \
		bsdextrautils \
		bzip2 \
		ccache \
		cmake \
		cscope \
		curl \
		device-tree-compiler \
		dh-autoreconf \
		diffutils \
		exuberant-ctags \
		flex \
		g++ \
		gawk \
		gcc \
		git \
		gnat \
		golang \
		graphicsmagick-imagemagick-compat \
		graphviz \
		lcov \
		libcrypto++-dev \
		libcurl4 \
		libcurl4-openssl-dev \
		libelf-dev \
		libfreetype6-dev \
		libftdi1-dev \
		libglib2.0-dev \
		libgmp-dev \
		libgpiod-dev \
		libjaylink-dev \
		liblzma-dev \
		libncurses-dev \
		libpci-dev \
		libreadline-dev \
		libssl-dev \
		libusb-1.0-0-dev \
		libxml2-dev \
		libyaml-dev \
		m4 \
		make \
		msitools \
		nasm \
		openssl \
		parted \
		patch \
		pbzip2 \
		pkg-config \
		python3 \
		python-is-python3 \
		qemu-system-arm \
		qemu-system-misc \
		qemu-system-ppc \
		qemu-system-x86 \
		rsync \
		sharutils \
		shellcheck \
		unifont \
		unzip \
		uuid-dev \
		vim-common \
		wget \
		xz-utils \
		zlib1g-dev \
	&& apt-get clean

RUN \
	cd /tmp && \
	git clone https://review.coreboot.org/coreboot && \
	cd coreboot && \
	git checkout {{DOCKER_COMMIT}}; \
	if echo {{CROSSGCC_PARAM}} | grep -q ^all; then \
		make -C /tmp/coreboot/util/crossgcc/ build_clang \
			BUILD_LANGUAGES=c,ada CPUS=$(nproc) DEST=/opt/xgcc; \
	fi; \
	make -C /tmp/coreboot/util/crossgcc/ {{CROSSGCC_PARAM}} \
		BUILD_LANGUAGES=c,ada CPUS=$(nproc) DEST=/opt/xgcc && \
	rm -rf /tmp/coreboot

RUN mkdir /home/coreboot/.ccache && \
	chown coreboot:coreboot /home/coreboot/.ccache && \
	mkdir /home/coreboot/cb_build && \
	chown coreboot:coreboot /home/coreboot/cb_build && \
	echo "export PATH=$PATH:/opt/xgcc/bin" >> /home/coreboot/.bashrc && \
	echo "export SDK_VERSION={{SDK_VERSION}}" >> /home/coreboot/.bashrc && \
	echo "export SDK_COMMIT={{DOCKER_COMMIT}}" >> /home/coreboot/.bashrc

ENV PATH $PATH:/opt/xgcc/bin
ENV SDK_VERSION={{SDK_VERSION}}
ENV SDK_COMMIT={{DOCKER_COMMIT}}
USER coreboot

FROM coreboot-sdk
VOLUME /home/coreboot/.ccache
