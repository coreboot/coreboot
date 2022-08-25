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
		graphviz \
		lcov \
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
	&& update-alternatives --install /usr/bin/python python /usr/bin/python2 1 \
	&& ln -s /usr/bin/automake /usr/bin/automake-1.15 \
	&& ln -s /usr/bin/aclocal /usr/bin/aclocal-1.15

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
# Test the built image
RUN mkdir -p /tmp/work && \
  cd /tmp/work && \
  git clone https://review.coreboot.org/bios_extract.git && \
  make -C bios_extract && \
  git clone https://review.coreboot.org/memtest86plus.git && \
  make -C memtest86plus && \
  git clone https://review.coreboot.org/flashrom.git && \
  CONFIG_EVERYTHING=yes make -C flashrom && \
  git clone https://review.coreboot.org/em100.git && \
  make -C em100 && \
  git clone https://review.coreboot.org/coreboot.git && \
  (cd coreboot && git submodule update --init --checkout ) && \
  make -C coreboot CPUS=$(nproc) test-abuild

RUN \
  cd /tmp/work && \
  make -C coreboot olddefconfig && \
  make -C coreboot all -j && \
  make -C coreboot printall && \
  make -C coreboot filelist && \
  make -C coreboot ctags-project && \
  make -C coreboot cscope-project

RUN \
  cd /tmp/work && \
  make -C coreboot test-payloads&& \
  make -C coreboot test-tools -j && \
  make -C coreboot test-lint -j && \
  make -C coreboot test-cleanup -j && \
  cd && \
  rm -rf /tmp/work/

FROM coreboot-sdk
VOLUME /home/coreboot/.ccache
