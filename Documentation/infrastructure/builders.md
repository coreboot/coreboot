# Jenkins builder setup and configuration

## How to set up a new jenkins builder

### Contact a jenkins admin

Let a jenkins admin know that you’re interested in setting up a jenkins
build system.

For a permanent build system, this should generally be a dedicated
machine workstation or server class machine that is not generally being
used for other purposes.  The coreboot builds are very intensive.

It's also best to be aware that although we don't know of any security
issues, the jenkins-node image is run with the privileged flag which
gives the container root access to the build machine.  See
[this article](https://blog.trendmicro.com/trendlabs-security-intelligence/why-running-a-privileged-container-in-docker-is-a-bad-idea/)
about why this is discouraged.

It's recommended that you give an admin root access on your machine so
that they can reset it in case of a failure.  This is not a requirement,
as the system can just be disabled until someone is available to fix any
issues.

Currently active Jenkins admins:
*   Patrick Georgi:
    *   Email: [patrick@georgi-clan.de](mailto:patrick@georgi-clan.de)
    *   IRC: pgeorgi
*   Martin Roth:
    *   Email: [gaumless@gmail.com](mailto:gaumless@gmail.com)
    *   IRC: martinr

### Build Machine requirements

For a builder, we need a very fast system with lots of threads and
plenty of RAM.  The builder builds and stores the git repos and output
in tmpfs along with the ccache save area, so if there isn't enough
memory, the builds will slow down because of smaller ccache areas and
can run into "out of storage space" errors.

#### Current Build Machines

To give an idea of what a suitable build machine might be, currently the
coreboot project has 6 active jenkins build machines.

These times are taken from the week of Feb 21 - Feb 28, 2022

* Congenialbuilder - 128 threads, 256GiB RAM
  * Coverity Builds, Toolchain builds, Scanbuild-builds
  * Fastest Passing coreboot gerrit build: 6 min, 47 sec
  * Slowest Passing coreboot gerrit build: 14 min

* Gleefulbuilder - 64 threads, 64GiB RAM
  * Fastest Passing coreboot gerrit build: 10 min
  * Slowest Passing coreboot gerrit build: 46 min

* Fabulousbuilder - 64 threads, 64GiB RAM
  * Fastest Passing coreboot gerrit build: 7 min, 56 sec
  * Slowest Passing coreboot gerrit build: 56 min (No ccache)

* Ultron (9elements) - 48 threads, 128GiB RAM
  * Fastest Passing coreboot gerrit build: 12 min
  * Slowest Passing coreboot gerrit build: 58 min

* Bob - 64 threads, 128GiB RAM
  * Fastest Passing coreboot gerrit build: 7 min
  * Slowest Passing coreboot gerrit build: 34 min

* Pokeybuilder - 32 Threads, 96GiB RAM
  * Runs coreboot-checkpatch and other lighter builds


### Jenkins Builds

There are a number of builds handled by the coreboot jenkins builders,
for a number of different projects - coreboot, flashrom, memtest86+,
em100, etc.  Many of these have builders for their current master branch
as well as Gerrit and [Coverity](coverity.md) builds.


#### Long builds - over 90 minutes on congenialbuilder
There are a few builds that take a long time even on the fastest
machines.  These tasks run overnight in the US timezones.
* coreboot_coverity - 9 to 12 hours
* coreboot_scanbuild - ~3 hours
* coreboot_toolchain - ~1 hour 45 minutes


#### All builds

You can see all the builds in the main jenkins interface:
[https://qa.coreboot.org/](https://qa.coreboot.org/)

Most of the time on the builders is taken up by the coreboot master and
coreboot gerrit builds.

* [coreboot gerrit build](https://qa.coreboot.org/job/coreboot-gerrit/)
([Time trend](https://qa.coreboot.org/job/coreboot-gerrit/buildTimeTrend))


* [coreboot master build](https://qa.coreboot.org/job/coreboot/)
 ([Time trend](https://qa.coreboot.org/job/coreboot/buildTimeTrend))


### Stress test the machine

Test the machine to make sure that building won't stress the hardware
too much.  Install stress-ng, then run the stress test for at least an
hour.

On a system with 32 cores, it was tested with this command:

```sh
stress-ng --cpu 20 --io 6 --vm 6 --vm-bytes 1G --verify --metrics-brief -t 60m
```

You can watch the temperature with the sensors package or with ‘acpi -t’
if your machine supports that.

You can check for thermal throttling by running this command and seeing
if the values go down on any of the cores after it's been running for a
while.

```sh
while [ true ]; do clear; cat /proc/cpuinfo | grep 'cpu MHz' ; sleep 1; done
```

If the machine throttles or resets, you probably need to upgrade the
cooling system.


## jenkins-server docker installation


### Manual Installation

If you’ve met all the above requirements, and an admin has agreed to set
up the builder in jenkins, you’re ready to go on to the next steps.


### Set up your network so jenkins can talk to the container

Expose a local port through any firewalls you might have on your router.
This would generally be in the port forwarding section, and you'd just
forward a port (typically 49151) from the internet directly to the
builder’s IP address.

You might also want to set up a port to forward to port 22 on your
machine and set up openssh so you or the jenkins admins can manage
the machine remotely (if you allow them).


### Install and set up docker

Install docker by following [the
directions](https://docs.docker.com/engine/install/) on the docker site.
These instructions keep changing, so just check the latest information.


### Set up the system for the jenkins builder

As a regular user - *Not root*, run:

```sh
sudo mkdir -p ${COREBOOT_JENKINS_CACHE_DIR}
sudo mkdir -p ${COREBOOT_JENKINS_CCACHE_DIR}
sudo chown $(whoami):$(whoami) ${COREBOOT_JENKINS_CCACHE_DIR}
sudo chown $(whoami):$(whoami) ${COREBOOT_JENKINS_CACHE_DIR}
wget http://www.dediprog.com/save/78.rar/to/EM100Pro.rar
mv EM100Pro.rar ${COREBOOT_JENKINS_CACHE_DIR}
```


#### Set up environment variables

To make configuration and the later commands easier, these should go in
your shell's .rc file.  Note that you only need to set them if you're
using something other than the default.

```sh
# Set the port used on your machine to connect to jenkins.
export COREBOOT_JENKINS_PORT=49151

# Set the revision of the container from [docker hub](https://hub.docker.com/repository/docker/coreboot/coreboot-sdk)
export DOCKER_COMMIT=2021-09-23_b0d87f753c

# Set the location of where the jenkins cache directory will be.
export COREBOOT_JENKINS_CACHE_DIR="/srv/docker/coreboot-builder/cache"

# Set the name of the container
export COREBOOT_JENKINS_CONTAINER="coreboot_jenkins"
```

Make sure any variables needed are set in your environment before
continuing to the next step.


### Using the Makefile for docker installation

From the coreboot directory, run

```sh
make -C util/docker help
```

This will show you the available targets and variables needed:

```text
Commands for working with docker images:
  coreboot-sdk                 - Build coreboot-sdk container
  upload-coreboot-sdk          - Upload coreboot-sdk to hub.docker.com
  coreboot-jenkins-node        - Build coreboot-jenkins-node container
  upload-coreboot-jenkins-node - Upload coreboot-jenkins-node to hub.docker.com
  doc.coreboot.org             - Build doc.coreboot.org container
  clean-coreboot-containers    - Remove all docker coreboot containers
  clean-coreboot-images        - Remove all docker coreboot images
  docker-clean                 - Remove docker coreboot containers & images

Commands for using docker images
  docker-build-coreboot        - Build coreboot under coreboot-sdk
      <BUILD_CMD=target>
  docker-abuild                - Run abuild under coreboot-sdk
      <ABUILD_ARGS='-a -B'>
  docker-what-jenkins-does     - Run 'what-jenkins-does' target
  docker-shell                 - Bash prompt in coreboot-jenkins-node
      <USER=root or USER=coreboot>
  docker-jenkins-server        - Run coreboot-jenkins-node image (for server)
  docker-jenkins-attach        - Open shell in running jenkins server
  docker-build-docs            - Build the documentation
  docker-livehtml-docs         - Run sphinx-autobuild

Variables:
  COREBOOT_JENKINS_PORT=49151
  COREBOOT_JENKINS_CACHE_DIR=/srv/docker/coreboot-builder/cache
  COREBOOT_JENKINS_CONTAINER=coreboot_jenkins
  COREBOOT_IMAGE_TAG=f2741aa632f
  DOCKER_COMMIT=65718760fa
```


### Install the coreboot jenkins builder

```sh
make -C util/docker docker-jenkins-server
```

Your installation is complete on your side.

### Tell the Admins that the machine is set up
Let the admins know that the builder is set up so they can set up the
machine profile on qa.coreboot.org.

They need to know:
*   Your external IP address or domain name.  If you don’t have a static
    IP, make sure you have a dynamic dns hostname configured.
*   The port on your machine and firewall that’s exposed for jenkins:
    `$COREBOOT_JENKINS_PORT`
*   The core count of the machine.
*   How much memory is available on the machine.  This helps determine
    the amount of memory used for ccache.


### First build
On the first build after a machine is reset, it will frequently take
an hour to do the entire what-jenkins-does build while the ccache
is getting filled up and the entire coreboot repo gets downloaded.  As
the ccache gets populated, the build time will drop.


## Additional Information


### How to log in to the docker instance for debugging

```sh
make -C util/docker docker-jenkins-attach
su coreboot
cd ~/slave-root/workspace
bash
```


WARNING: This should not be used to make changes to the build system,
but just to debug issues. Changes to the build system image are highly
discouraged as it leads to situations where patches can pass the build
testing on one builder and fail on another builder. Any changes that are
made in the image will be lost on the next update, so if you
accidentally change something, you can remove the containers and images,
then update to get a fresh installation.


### How to download containers/images for a fresh installation and remove old containers

To delete the old containers & images:

```sh
docker stop $COREBOOT_JENKINS_CONTAINER
docker rm $COREBOOT_JENKINS_CONTAINER
docker images # lists all existing images
docker rmi XXXX # Use the image ID found in the above command.
```

To get and run the new coreboot-jenkins image, change the value in the
`DOCKER_COMMIT` variable to the new image value.

```sh
make -C util/docker docker-jenkins-server
```

#### Getting ready to push the docker images

Set up an account on hub.docker.com

Get an admin to add the account to the coreboot team on hub.docker.com

[https://hub.docker.com/u/coreboot/dashboard/teams/?team=owners](https://hub.docker.com/u/coreboot/dashboard/teams/?team=owners)

Make sure your credentials are configured on your host machine by
running

```sh
docker login
```

This will prompt you for your docker username, password, and your email
address, and write out to ~/.docker/config.json.  Without this file, you
won’t be able to push the images.

#### Updating the Dockerfiles

The coreboot-sdk Dockerfile will need to be updated when any additional
dependencies are added.  Both the coreboot-sdk and the
coreboot-jenkins-node Dockerfiles will need to be updated to the new
version number and git commit id anytime the toolchain is updated. Both
files are stored in the coreboot repo under coreboot/util/docker.

Read the [dockerfile best practices](https://docs.docker.com/v1.8/articles/dockerfile_best-practices/)
page before updating the files.

#### Rebuilding the coreboot-sdk docker image to update the toolchain

```sh
make -C util/docker coreboot-sdk
```

This takes a relatively long time.

#### Test the coreboot-sdk docker image

There are two methods of running the docker image - interactively as a
shell, or doing the build directly.  Running interactively as a shell is
useful for early testing, because it allows you to update the image
(without any changes getting saved) and re-test builds.  This saves the
time of having to rebuild the image for every issue you find.

#### Running the docker image interactively

Run:

```sh
make -C util/docker docker-jenkins-server
make -C util/docker docker-jenkins-attach
```

#### Running the build directly

From the coreboot directory:

```sh
make -C util/docker docker-build-coreboot
```

You’ll also want to test building the other projects and payloads:
ChromeEC, flashrom, memtest86+, em100, Grub2, SeaBIOS, iPXE, coreinfo,
nvramcui, tint...

#### Pushing the coreboot-sdk image to hub.docker.com for use

When you’re satisfied with the testing, push the coreboot-sdk image to
the hub.docker.com

```sh
make -C util/docker upload-coreboot-sdk
```

#### Building and pushing the coreboot-jenkins-node docker image

This docker image is pretty simple, so there’s not really any testing
that needs to be done.

```sh
make -C util/docker coreboot-jenkins-node
make -C util/docker upload-coreboot-jenkins-node
```

### Coverity Setup

To run coverity jobs, the builder needs to have the tools available, and
to be marked as a coverity builder.


#### Set up the Coverity tools

Download the Linux-64 coverity build tool and decompress it into your
cache directory as defined by the `$COREBOOT_JENKINS_CACHE_DIR` variable
on the jenkins server.

[https://scan.coverity.com/download](https://scan.coverity.com/download)

Rename the directory from its original name
(cov-analysis-linux64-7.7.0.4) to ‘coverity’, or better, create a
symlink:

```sh
ln -s cov-analysis-linux64-7.7.0.4 coverity
```


Let the admins know that the ‘coverity’ label can be added to the
builder.
