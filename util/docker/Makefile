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
export DOCKER:=$(shell $(SHELL) -c "command -v docker")

# Local port to forward to the jenkins image for the test server
export COREBOOT_JENKINS_PORT?=49151

# Local cache directory - for storing files shared with the docker image
export COREBOOT_JENKINS_CACHE_DIR?=/srv/docker/coreboot-builder/cache

# Name of the jenkins container
export COREBOOT_JENKINS_CONTAINER?=coreboot_jenkins

# Version of the jenkins / sdk container
export COREBOOT_CONTAINER_VERSION?=$(crossgcc_version)

# Commit id to build from
export DOCKER_COMMIT?=$(shell git log -n 1 --pretty=%h)

# SDK architecture
export COREBOOT_CROSSGCC_PARAM?=all_without_gdb

UID ?= $(shell id -u)
GID ?= $(shell id -g)

test-docker:
	$(if $(DOCKER),,\
		$(warning Docker command not found.  Please install docker) \
		$(warning https://docs.docker.com/engine/installation ) \
		$(error halting))

test-docker-login: test-docker
	$(if $(shell if [ ! -f ~/.docker/config.json ]; then \
		echo "docker authentication file not found"; fi), \
		$(error Docker authentication file not found.  Run 'docker login'))

coreboot-sdk: test-docker
	@echo "Building coreboot SDK $(crossgcc_version) from commit $(DOCKER_COMMIT)"
	cat coreboot-sdk/Dockerfile | \
		sed "s/{{DOCKER_COMMIT}}/$(DOCKER_COMMIT)/" | \
		sed "s/{{SDK_VERSION}}/$(COREBOOT_CONTAINER_VERSION)/" | \
		sed "s/{{CROSSGCC_PARAM}}/$(COREBOOT_CROSSGCC_PARAM)/" | \
		$(DOCKER) build -t coreboot/coreboot-sdk:$(COREBOOT_CONTAINER_VERSION) -

upload-coreboot-sdk: test-docker-login
	$(DOCKER) push coreboot/coreboot-sdk:$(COREBOOT_CONTAINER_VERSION)

coreboot-jenkins-node: test-docker
	cat coreboot-jenkins-node/Dockerfile | \
		sed "s/{{SDK_VERSION}}/$(COREBOOT_CONTAINER_VERSION)/" | \
		sed "s|{{SSH_KEY}}|$$(cat coreboot-jenkins-node/authorized_keys)|" | \
		$(DOCKER) build -t coreboot/coreboot-jenkins-node:$(COREBOOT_CONTAINER_VERSION) -

upload-coreboot-jenkins-node: test-docker-login
	$(DOCKER) push coreboot/coreboot-jenkins-node:$(COREBOOT_CONTAINER_VERSION)

docker-killall: test-docker
	@if [ -n "$$($(DOCKER) ps | grep 'coreboot')" ]; then \
		$(DOCKER) kill $$($(DOCKER) ps | grep 'coreboot' | cut -f1 -d ' '); \
	fi

clean-coreboot-containers: docker-killall
	@if [ -n "$$($(DOCKER) ps -a | grep 'coreboot')" ]; then \
		$(DOCKER) rm $$($(DOCKER) ps -a | grep 'coreboot' | sed 's|\s.*$$||'); \
	fi

clean-coreboot-images: docker-killall
	@if [ -n "$$($(DOCKER) images | grep 'coreboot')" ]; then \
		$(DOCKER) rmi $$($(DOCKER) images | grep coreboot | sed 's|^\S\+\s\+\S\+\s\+||' | sed 's|\s.*$$||'); \
	fi

docker-clean: clean-coreboot-containers
	@$(MAKE) clean-coreboot-images

docker-cleanall:
	@if [ -n "$$($(DOCKER) ps -a | grep -v "CONTAINER")" ]; then \
		$(DOCKER) kill $$($(DOCKER) ps -a | grep -v "CONTAINER" | sed 's|\s.*$$||'); \
		$(DOCKER) rm $$($(DOCKER) ps -a | grep -v "CONTAINER" | sed 's|\s.*$$||'); \
	fi
	@if [ -n "$$($(DOCKER) images | grep -v "REPOSITORY")" ]; then \
		$(DOCKER) rmi $$($(DOCKER) images | grep -v "REPOSITORY" | tr -s ' ' | cut -f3 -d ' '); \
	fi

docker-build-coreboot: test-docker
	$(DOCKER) run -u root -it -v $(top):/home/coreboot/coreboot \
		--user $(UID):$(GID) \
		--rm coreboot/coreboot-sdk:$(COREBOOT_CONTAINER_VERSION) \
		/bin/bash -c "cd /home/coreboot/coreboot && \
		make clean && \
		make $(BUILD_CMD); \
		rm .xcompile"

docker-abuild: test-docker
	$(DOCKER) run -u root -it -v $(top):/home/coreboot/coreboot \
		--user $(UID):$(GID) \
		--rm coreboot/coreboot-sdk:$(COREBOOT_CONTAINER_VERSION) \
		/bin/bash -c "cd /home/coreboot/coreboot && \
		make clean && \
		util/abuild/abuild $(ABUILD_ARGS); \
		rm -f .xcompile"

docker-what-jenkins-does: test-docker
	$(DOCKER) run -u root -it -v $(top):/home/coreboot/coreboot \
		--rm coreboot/coreboot-sdk:$(COREBOOT_CONTAINER_VERSION) \
		/bin/bash -c "cd /home/coreboot/coreboot && \
		make clean && make what-jenkins-does CPUS=$(CPUS)"

docker-jenkins-server: test-docker
	@if [ ! -d "$(COREBOOT_JENKINS_CACHE_DIR)" ]; then \
		printf "\nError: %s does not exist.\n" "$(COREBOOT_JENKINS_CACHE_DIR)"; \
		printf "Please run:\nsudo mkdir -p %s\n\n" "$(COREBOOT_JENKINS_CACHE_DIR)"; \
		false; \
	fi
	@if ! touch "$(COREBOOT_JENKINS_CACHE_DIR)/testfile" 2>/dev/null; then \
		printf "\nError: Cannot create file in %s." "$(COREBOOT_JENKINS_CACHE_DIR)"; \
		printf "Please run:\nsudo chown -R $(whoami):$(whoami) %s\n\n" "$(COREBOOT_JENKINS_CACHE_DIR)"; \
	else \
		 rm -f "$(COREBOOT_JENKINS_CACHE_DIR)/testfile"; \
	fi
	$(DOCKER) run -d --privileged --restart=always \
		-p $(COREBOOT_JENKINS_PORT):49151 \
		-v $(COREBOOT_JENKINS_CACHE_DIR):/data/cache \
		--name=$(COREBOOT_JENKINS_CONTAINER) \
		coreboot/coreboot-jenkins-node:$(COREBOOT_CONTAINER_VERSION)

docker-jenkins-shell: test-docker
	@if [ ! -d $(COREBOOT_JENKINS_CACHE_DIR) ]; then printf "\nError: %s does not exist.\nPlease run:\nsudo mkdir -p %s\n\n" "$(COREBOOT_JENKINS_CACHE_DIR)" "$(COREBOOT_JENKINS_CACHE_DIR)"; false; fi
	$(DOCKER) run -u root -it --privileged \
		--entrypoint="/bin/bash" \
		-p $(COREBOOT_JENKINS_PORT):49151 \
		-v $(top):/home/coreboot/coreboot \
		-v $(COREBOOT_JENKINS_CACHE_DIR):/data/cache \
		--name=$(COREBOOT_JENKINS_CONTAINER) \
		--rm coreboot/coreboot-jenkins-node:$(COREBOOT_CONTAINER_VERSION)

docker-shell: USER=coreboot
docker-shell: test-docker
	$(DOCKER) run -u $(USER) -it \
		-e COLUMNS=$(shell tput cols) -e LINES=$(shell tput lines) -e TERM=$(TERM) \
		-w /home/coreboot/coreboot \
		-v $(top):/home/coreboot/coreboot \
		--rm coreboot/coreboot-sdk:$(COREBOOT_CONTAINER_VERSION) \
		/bin/bash -l

docker-jenkins-attach: USER=root
docker-jenkins-attach:
	docker exec --user $(USER) \
		-e COLUMNS=$(shell tput cols) -e LINES=$(shell tput lines) -e TERM=$(TERM) \
		-it "$$(docker ps | grep coreboot-jenkins-node | cut -f1 -d' ')" \
		/bin/bash -l

help:
	@echo "Commands for working with docker images:"
	@echo "  coreboot-sdk                 - Build coreboot-sdk container"
	@echo "  upload-coreboot-sdk          - Upload coreboot-sdk to hub.docker.com"
	@echo "  coreboot-jenkins-node        - Build coreboot-jenkins-node container"
	@echo "  upload-coreboot-jenkins-node - Upload coreboot-jenkins-node to hub.docker.com"
	@echo "  clean-coreboot-containers    - Remove all docker coreboot containers"
	@echo "  clean-coreboot-images        - Remove all docker coreboot images"
	@echo "  clean-docker                 - Remove docker coreboot containers & images"
	@echo
	@echo "Commands for using docker images"
	@echo "  docker-build-coreboot        - Build coreboot under coreboot-sdk"
	@echo "      <BUILD_CMD=target> "
	@echo "  docker-abuild                - Run abuild under coreboot-sdk"
	@echo "      <ABUILD_ARGS='-a -B'>"
	@echo "  docker-what-jenkins-does     - Run 'what-jenkins-does' target"
	@echo "  docker-shell                 - Bash prompt in coreboot-jenkins-node"
	@echo "      <USER=root or USER=coreboot>"
	@echo "  docker-jenkins-server        - Run coreboot-jenkins-node image (for server)"
	@echo "  docker-jenkins-attach        - Open shell in running jenkins server"
	@echo
	@echo "Variables:"
	@echo "  COREBOOT_JENKINS_PORT=$(COREBOOT_JENKINS_PORT)"
	@echo "  COREBOOT_JENKINS_CACHE_DIR=$(COREBOOT_JENKINS_CACHE_DIR)"
	@echo "  COREBOOT_JENKINS_CONTAINER=$(COREBOOT_JENKINS_CONTAINER)"
	@echo "  COREBOOT_CONTAINER_VERSION=$(COREBOOT_CONTAINER_VERSION)"
	@echo "  DOCKER_COMMIT=$(DOCKER_COMMIT)"

.PHONY: test-docker test-docker-login
.PHONY: coreboot-jenkins-node upload-coreboot-jenkins-node
.PHONY: coreboot-sdk upload-coreboot-sdk
.PHONY: clean-coreboot-containers clean-coreboot-images
.PHONY: docker-abuild
.PHONY: docker-what-jenkins-does docker-shell docker-jenkins-server docker-jenkins-attach
.PHONY: help
