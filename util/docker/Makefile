##
## This file is part of the coreboot project.
##
## Copyright (C) 2016 Google, Inc.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
export top=$(abspath $(CURDIR)/../..)
export crossgcc_version=$(shell $(top)/util/crossgcc/buildgcc --version | grep 'cross toolchain' | sed 's/^.*\sv//' | sed 's/\s.*$$//')
export DOCKER:=$(shell env sh -c "command -v docker")

test_docker:
	$(if $(DOCKER),,\
		$(warning Docker command not found.  Please install docker) \
		$(warning https://docs.docker.com/engine/installation ) \
		$(error halting))

test_docker_login: test_docker
	$(if $(shell if [ ! -f ~/.docker/config.json ]; then \
		echo "docker authentication file not found"; fi), \
		$(error Docker authentication file not found.  Run 'docker login'))

coreboot-sdk: test_docker
	$(DOCKER) build -t coreboot/coreboot-sdk:$(crossgcc_version) coreboot-sdk

upload-coreboot-sdk: test_docker_login
	$(DOCKER) push coreboot/coreboot-sdk:$(crossgcc_version)

coreboot-jenkins-node: test_docker
	$(DOCKER) build -t coreboot/coreboot-jenkins-node:$(crossgcc_version) coreboot-jenkins-node

upload-coreboot-jenkins-node: test_docker_login
	$(DOCKER) push coreboot/coreboot-jenkins-node:$(crossgcc_version)

clean_coreboot_containers: test_docker
	$(DOCKER) rm $(docker ps -a | grep 'coreboot' | sed 's|\s.*$||')

clean_coreboot_images: test_docker
	$(DOCKER) rmi $(docker images | grep coreboot | sed 's|^\S\+\s\+\S\+\s\+||' | sed 's|\s.*$||')

docker_build_coreboot: test_docker

	$(DOCKER) run -u root -it -v $(top):/home/coreboot/coreboot \
		--rm coreboot/coreboot-sdk:$(crossgcc_version) \
		/bin/bash -c "cd /home/coreboot/coreboot && \
		make clean && \
		make $(BUILD_CMD)"
	@echo "Enter root password to chown files to $$(whoami):$$(id -gn $$(whoami))"
	@echo "Exiting now will leave built files owned by root"
	sudo chown -R $$(whoami):$$(id -gn $$(whoami)) $(top)

docker_abuild: test_docker
	$(DOCKER) run -u root -it -v $(top):/home/coreboot/coreboot \
		--rm coreboot/coreboot-sdk:$(crossgcc_version) \
		/bin/bash -c "cd /home/coreboot/coreboot && \
		make clean && \
		util/abuild/abuild $(ABUILD_ARGS)"
	@echo "Enter root password to chown files to $$(whoami):$$(id -gn $$(whoami))"
	@echo "Exiting now will leave built files owned by root"
	sudo chown -R $$(whoami):$$(id -gn $$(whoami)) $(top)

help:
	@echo "Commands for working with docker images:"
	@echo "  build-coreboot-sdk           - Build coreboot-sdk container"
	@echo "  upload-coreboot-sdk          - Upload coreboot-sdk to hub.docker.com"
	@echo "  build-coreboot-jenkins-node  - Build coreboot-jenkins-node container"
	@echo "  upload-coreboot-jenkins-node - Upload coreboot-jenkins-node to hub.docker.com"
	@echo "  clean_coreboot_containers    - remove all docker coreboot containers"
	@echo "  clean_coreboot_images        - remove all docker coreboot images"
	@echo
	@echo "Commands for using docker images"
	@echo "  docker_build_coreboot <BUILD_CMD=target>  - Build coreboot under coreboot-sdk"
	@echo "  docker_abuild <ABUILD_ARGS='-a -B'>       - Run abuild under coreboot-sdk"

.PHONY: test_docker test_docker_login help
.PHONY: build-coreboot-jenkins-node upload-coreboot-jenkins-node
.PHONY: build-coreboot-sdk upload-coreboot-sdk
.PHONY: clean_coreboot_containers clean_coreboot_images
.PHONY: docker_build_coreboot docker_abuild
.PHONY: help