# This dockerfile is not meant to be used directly by docker.  The
# {{}} varibles are replaced with values by the makefile.  Please generate
# the docker image for this file by running:
#
#   make coreboot-jenkins-node
#
# Variables can be updated on the make command line or left blank to use
# the default values set by the makefile.
#
#  SDK_VERSION is used to name the version of the coreboot sdk to use.
#              Typically, this corresponds to the toolchain version.
#  SSH_KEY is the contents of the file coreboot-jenkins-node/authorized_keys
#          Because we're piping the contents of the dockerfile into the
#          docker build command, the 'COPY' keyword isn't valid.

FROM coreboot/coreboot-sdk:{{SDK_VERSION}}
MAINTAINER Martin Roth <martin@coreboot.org>
USER root

# Check to make sure /dev is a tmpfs file system
RUN mount | grep "on /dev type tmpfs" > /dev/null || exit 1

RUN apt-get -y update && \
	apt-get -y install \
	lua5.3 liblua5.3-dev openjdk-8-jre-headless openssh-server && \
	apt-get clean

# Because of the way that the variables are being replaced, docker's 'COPY'
# command does not work
RUN mkdir -p /home/coreboot/.ssh && \
	echo "{{SSH_KEY}}" > /home/coreboot/.ssh/authorized_keys && \
	chown -R coreboot:coreboot /home/coreboot/.ssh && \
	chmod 0700 /home/coreboot/.ssh && \
	chmod 0600 /home/coreboot/.ssh/authorized_keys

RUN mkdir /var/run/sshd && \
	chmod 0755 /var/run/sshd && \
	/usr/bin/ssh-keygen -A

# Build encapsulate tool
ADD https://raw.githubusercontent.com/pgeorgi/encapsulate/master/encapsulate.c /tmp/encapsulate.c
RUN gcc -o /usr/sbin/encapsulate /tmp/encapsulate.c && \
	chown root /usr/sbin/encapsulate && \
	chmod +s /usr/sbin/encapsulate

VOLUME /data/cache
ENTRYPOINT mkdir /dev/cb-build && chown coreboot /dev/cb-build && /usr/sbin/sshd -p 49151 -D
EXPOSE 49151
ENV PATH $PATH:/usr/sbin
