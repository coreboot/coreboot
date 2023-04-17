FROM archlinux:latest

RUN pacman-key --init && \
	pacman -Sy --noconfirm archlinux-keyring && \
	pacman-key --populate && \
	pacman-key --refresh-keys && \
	pacman -Syu --noconfirm

RUN pacman -S --noconfirm \
	base-devel \
	bash-completion \
	gcc-ada \
	git \
	go \
	htop \
	neovim \
	python3 \
	tmux && \
	rm -r /var/cache/pacman/pkg/*
