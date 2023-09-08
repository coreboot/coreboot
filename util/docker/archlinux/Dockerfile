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
	bzip2 \
	ccache \
	cscope \
	gcc-ada \
	git \
	go \
	htop \
	lcov \
	less \
	msitools \
	neovim \
	openssl \
	python3 \
	shellcheck \
	tmux \
	unzip \
	xz && \
	rm -r /var/cache/pacman/pkg/*
