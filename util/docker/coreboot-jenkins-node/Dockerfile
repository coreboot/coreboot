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
USER root

RUN apt-get -y update && \
	apt-get -y install \
		default-jre-headless \
		libcmocka-dev \
		liblua5.3-dev \
		linkchecker \
		lua5.3 \
		meson \
		ninja-build \
		openssh-server \
		parallel \
		ruby-full \
		sdcc \
		python3-pip \
		pykwalify \
		python3-yaml \
		python3-pyelftools \
		python3-jsonschema \
		python3-colorama \
		python3-pyrsistent \
		swig \
	&& apt-get clean \
	&& pip3 install --upgrade --no-cache-dir --break-system-packages pip \
	&& pip3 install --no-cache-dir --break-system-packages \
		setuptools==58.2.0 \
		jinja2==3.0.3 \
		recommonmark===0.5.0 \
		sphinx===1.8.3 \
		sphinxcontrib-ditaa===0.6 \
		sphinx_autobuild===0.7.1 \
		sphinx_rtd_theme===0.4.2 \
	&& gem install mdl

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

# Create tmpfs directories to build in
RUN mkdir /cb-build && \
	chown coreboot:coreboot /cb-build && \
	echo "tmpfs /cb-build tmpfs rw,mode=1777,noatime 0 0" > /etc/fstab && \
	mkdir -p /home/coreboot/node-root/workspace && \
	chown -R coreboot:coreboot /home/coreboot/node-root && \
	echo "tmpfs /home/coreboot/node-root/workspace tmpfs rw,mode=1777,strictatime,atime 0 0" >> /etc/fstab && \
	chown coreboot:coreboot /home/coreboot/.ccache && \
	echo "tmpfs /home/coreboot/.ccache tmpfs rw,mode=1777 0 0" >> /etc/fstab

# Build encapsulate tool
ADD https://raw.githubusercontent.com/coreboot/encapsulate/master/encapsulate.c /tmp/encapsulate.c
RUN gcc -o /usr/sbin/encapsulate /tmp/encapsulate.c && \
	chown root /usr/sbin/encapsulate && \
	chmod +s /usr/sbin/encapsulate

VOLUME /data/cache
ENTRYPOINT mount /cb-build && \
	mount /home/coreboot/node-root/workspace && \
	chown -R coreboot:coreboot /home/coreboot/node-root && \
	mount /home/coreboot/.ccache && \
	chown coreboot:coreboot /home/coreboot/.ccache && \
	/usr/sbin/sshd -p 49151 -D
EXPOSE 49151
ENV PATH $PATH:/usr/sbin
