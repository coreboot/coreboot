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
		liblua5.4-dev \
		linkchecker \
		lua5.4 \
		openssh-server \
		parallel \
		ruby-full \
		sdcc \
		python3-pip \
		pykwalify \
		python3-venv \
		python3-yaml \
		python3-pyelftools \
		python3-jsonschema \
		python3-colorama \
		python3-pyrsistent \
		swig \
	&& apt-get clean \
	&& gem install mdl

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
ADD https://raw.githubusercontent.com/coreboot/encapsulate/main/encapsulate.c /tmp/encapsulate.c
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

USER coreboot
ENV VIRTUAL_ENV="/home/coreboot/python3"
ENV PATH=$VIRTUAL_ENV/bin:$PATH:/home/coreboot/.local/bin
RUN echo 'export PATH=$PATH:/opt/xgcc/bin' >> /home/coreboot/.bashrc && \
	echo "source ${VIRTUAL_ENV}/bin/activate"  >> /home/coreboot/.bashrc && \
	python3 -m venv /home/coreboot/python3 && \
	pip3 install --upgrade --no-cache-dir pip && \
	pip3 install --no-cache-dir \
		setuptools==58.2.0 \
		jinja2==3.1.3 \
		myst-parser===2.0.0 \
		sphinx===7.2.6 \
		sphinxcontrib-ditaa===1.0.2 \
		sphinx_autobuild===2024.2.4 \
		sphinx_rtd_theme===2.0.0 \
	&& mkdir -p /home/coreboot/.ssh && \
	echo "{{SSH_KEY}}" > /home/coreboot/.ssh/authorized_keys && \
	chmod 0700 /home/coreboot/.ssh && \
	chmod 0600 /home/coreboot/.ssh/authorized_keys
USER root
