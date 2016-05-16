FROM coreboot/coreboot-sdk:1.42
MAINTAINER Martin Roth <gaumless@gmail.com>
USER root

# Check to make sure /dev is a tmpfs file system
RUN mount | grep "on /dev type tmpfs" > /dev/null || exit 1

RUN apt-get -y update && \
	apt-get -y install \
	lua5.3 liblua5.3-dev openjdk-8-jre-headless openssh-server && \
	apt-get clean

COPY authorized_keys /home/coreboot/.ssh/authorized_keys
RUN chown -R coreboot /home/coreboot/.ssh && \
	chmod 0700 /home/coreboot/.ssh && \
	chmod 0600 /home/coreboot/.ssh/authorized_keys

RUN mkdir /var/run/sshd && \
	chmod 0755 /var/run/sshd

# Build encapsulate tool
ADD https://raw.githubusercontent.com/pgeorgi/encapsulate/master/encapsulate.c /tmp/encapsulate.c
RUN gcc -o /usr/sbin/encapsulate /tmp/encapsulate.c && \
	chown root /usr/sbin/encapsulate && \
	chmod +s /usr/sbin/encapsulate


VOLUME /data/cache
ENTRYPOINT mkdir /dev/cb-build && chown coreboot /dev/cb-build && /usr/sbin/sshd -p 49151 -D
EXPOSE 49151
ENV PATH $PATH:/usr/sbin
