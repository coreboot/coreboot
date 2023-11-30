FROM archlinux:latest

RUN \
	echo "export EDITOR=nvim" > /etc/profile.d/cbsettings.sh && \
	chmod 755 /etc/profile.d/cbsettings.sh

RUN pacman-key --init && \
	pacman -Sy --noconfirm archlinux-keyring && \
	pacman-key --populate && \
	pacman-key --refresh-keys && \
	pacman -Syu --noconfirm

RUN pacman -S --noconfirm \
	base-devel \
	bash-completion \
	bc \
	bzip2 \
	ccache \
	cmake \
	cscope \
	dtc \
	gcc-ada \
	git \
	go \
	htop \
	lcov \
	less \
	meson \
	msitools \
	neovim \
	nss \
	openssh \
	openssl \
	python3 \
	shellcheck \
	tmux \
	unzip \
	xz && \
	rm -r /var/cache/pacman/pkg/*
