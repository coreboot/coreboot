FROM coreboot/coreboot-sdk:1.37
MAINTAINER Patrick Georgi <patrick@georgi-clan.de>
USER root
ADD https://raw.githubusercontent.com/pgeorgi/encapsulate/master/encapsulate.c /tmp/encapsulate.c
COPY authorized_keys /home/coreboot/.ssh/authorized_keys
RUN mount |grep "on /dev type tmpfs" > /dev/null || exit 1
RUN apt-get -y update && \
	apt-get -y install \
	curl p7zip-full unrar-free libusb-1.0-0-dev pkg-config libreadline-dev libglib2.0-dev \
	subversion libpci-dev libusb-dev libftdi-dev \
	lua5.3 liblua5.3-dev \
	ccache openjdk-8-jre-headless openssh-server libssl-dev vim-common libgmp-dev && \
	apt-get clean && \
	mkdir /home/coreboot/.ccache && chown coreboot /home/coreboot/.ccache && \
	mkdir -p /home/coreboot/.ssh && chown -R coreboot /home/coreboot/.ssh && chmod 0700 /home/coreboot/.ssh && chmod 0600 /home/coreboot/.ssh/authorized_keys && \
	gcc -o /usr/sbin/encapsulate /tmp/encapsulate.c && chown root /usr/sbin/encapsulate && chmod +s /usr/sbin/encapsulate && \
	mkdir /var/run/sshd && chmod 0755 /var/run/sshd
VOLUME /home/coreboot/.ccache
VOLUME /data/cache
ENTRYPOINT mkdir /dev/cb-build && chown coreboot /dev/cb-build && /usr/sbin/sshd -p 49151 -D
EXPOSE 49151
ENV PATH $PATH:/usr/sbin
