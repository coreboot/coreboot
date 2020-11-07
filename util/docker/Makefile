##
## SPDX-License-Identifier: GPL-2.0-only

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
export COREBOOT_IMAGE_TAG?=$(crossgcc_version)

# Commit id to build from
export DOCKER_COMMIT?=$(shell git log -n 1 --pretty=%h)

# .ccache dir to use
export DOCKER_CCACHE?=$(HOME)/.ccache

# SDK architecture
export COREBOOT_CROSSGCC_PARAM?=all_without_gdb

UID ?= $(shell id -u)
GID ?= $(shell id -g)

test-docker:
	$(if $(DOCKER),,\
		$(warning Error: Docker command not found.  Please install docker) \
		$(warning Instructions: https://docs.docker.com/engine/install/ ) \
		$(error halting))

test-docker-login: test-docker
	$(if $(shell if [ ! -f ~/.docker/config.json ]; then \
		echo "docker authentication file not found"; fi), \
		$(error Docker authentication file not found.  Run 'docker login'))

coreboot-sdk: test-docker
	@echo "Building coreboot SDK $(crossgcc_version) from commit $(DOCKER_COMMIT)"
	cat coreboot-sdk/Dockerfile | \
		sed "s/{{DOCKER_COMMIT}}/$(DOCKER_COMMIT)/" | \
		sed "s/{{SDK_VERSION}}/$(COREBOOT_IMAGE_TAG)/" | \
		sed "s/{{CROSSGCC_PARAM}}/$(COREBOOT_CROSSGCC_PARAM)/" | \
		$(DOCKER) build -t coreboot/coreboot-sdk:$(COREBOOT_IMAGE_TAG) -

upload-coreboot-sdk: test-docker-login
	$(DOCKER) push coreboot/coreboot-sdk:$(COREBOOT_IMAGE_TAG)

coreboot-jenkins-node: test-docker
	cat coreboot-jenkins-node/Dockerfile | \
		sed "s/{{SDK_VERSION}}/$(COREBOOT_IMAGE_TAG)/" | \
		sed "s|{{SSH_KEY}}|$$(cat coreboot-jenkins-node/authorized_keys)|" | \
		$(DOCKER) build -t coreboot/coreboot-jenkins-node:$(COREBOOT_IMAGE_TAG) -

upload-coreboot-jenkins-node: test-docker-login
	$(DOCKER) push coreboot/coreboot-jenkins-node:$(COREBOOT_IMAGE_TAG)

doc.coreboot.org: test-docker
	$(DOCKER) build --force-rm -t doc.coreboot.org \
	$(top)/util/docker/doc.coreboot.org/

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

docker-cleanall: test-docker
ifeq($(Y),1)
	@if [ -n "$$($(DOCKER) ps -a | grep -v "CONTAINER")" ]; then \
		$(DOCKER) kill $$($(DOCKER) ps | grep -v "CONTAINER" | sed 's|\s.*$$||') ; \
		$(DOCKER) rm $$($(DOCKER) ps -a | grep -v "CONTAINER" | sed 's|\s.*$$||'); \
	fi
	@if [ -n "$$($(DOCKER) images | grep -v "REPOSITORY")" ]; then \
		$(DOCKER) rmi $$($(DOCKER) images | grep -v "REPOSITORY" | tr -s ' ' | cut -f3 -d ' '); \
	fi
else
	echo "This will remove *ALL* docker containers from your machine."
	echo "If this is what you want, run 'make docker-cleanall Y=1'"
endif

$(DOCKER_CCACHE):
	@mkdir -p $@

docker-run-local: test-docker $(DOCKER_CCACHE)
	$(DOCKER) run -it --rm \
		--volume $(DOCKER_CCACHE):/home/coreboot/.ccache \
		--volume $(top):/home/coreboot/coreboot \
		--env HOME=/home/coreboot \
		--user $(UID):$(GID) \
		coreboot/coreboot-sdk:$(COREBOOT_IMAGE_TAG) \
		/bin/bash -c 'cd $${HOME}/coreboot && $(DOCKER_RUN_LOCAL)'

docker-build-coreboot: docker-run-local
docker-build-coreboot: override DOCKER_RUN_LOCAL := \
	make clean && make $(BUILD_CMD); rm -f .xcompile

docker-abuild: docker-run-local
docker-abuild: override DOCKER_RUN_LOCAL := \
	make clean && util/abuild/abuild $(ABUILD_ARGS); rm -f .xcompile

docker-what-jenkins-does: docker-run-local
docker-what-jenkins-does: override DOCKER_RUN_LOCAL := \
	make clean && make what-jenkins-does CPUS=$(CPUS)

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
		coreboot/coreboot-jenkins-node:$(COREBOOT_IMAGE_TAG)

docker-jenkins-shell: test-docker
	@if [ ! -d $(COREBOOT_JENKINS_CACHE_DIR) ]; then printf "\nError: %s does not exist.\nPlease run:\nsudo mkdir -p %s\n\n" "$(COREBOOT_JENKINS_CACHE_DIR)" "$(COREBOOT_JENKINS_CACHE_DIR)"; false; fi
	$(DOCKER) run -u root -it --privileged \
		--entrypoint="/bin/bash" \
		-p $(COREBOOT_JENKINS_PORT):49151 \
		-v $(top):/home/coreboot/coreboot \
		-v $(COREBOOT_JENKINS_CACHE_DIR):/data/cache \
		--name=$(COREBOOT_JENKINS_CONTAINER) \
		--rm coreboot/coreboot-jenkins-node:$(COREBOOT_IMAGE_TAG)

docker-shell: USER=coreboot
docker-shell: test-docker
	$(DOCKER) run -u $(USER) -it \
		-e COLUMNS=$(shell tput cols) -e LINES=$(shell tput lines) -e TERM=$(TERM) \
		-w /home/coreboot/coreboot \
		-v $(top):/home/coreboot/coreboot \
		--rm coreboot/coreboot-sdk:$(COREBOOT_IMAGE_TAG) \
		/bin/bash -l

docker-jenkins-attach: USER=root
docker-jenkins-attach: test-docker
	$(DOCKER)  exec --user $(USER) \
		-e COLUMNS=$(shell tput cols) -e LINES=$(shell tput lines) -e TERM=$(TERM) \
		-it "$$(docker ps | grep coreboot-jenkins-node | cut -f1 -d' ')" \
		/bin/bash -l

docker-build-docs: test-docker
docker-build-docs:
	$(DOCKER) run -it --rm \
		--user $(UID):$(GID) \
		-v "$(top)/:/data-in/:ro" \
		-v "$(top)/Documentation/_build/:/data-out/" \
		doc.coreboot.org

docker-livehtml-docs: test-docker
docker-livehtml-docs:
	$(DOCKER) run -it --rm \
		--net=host -v "$(top)/:/data-in/:ro" \
		doc.coreboot.org livehtml

help:
	@echo "Commands for working with docker images:"
	@echo "  coreboot-sdk                 - Build coreboot-sdk container"
	@echo "  upload-coreboot-sdk          - Upload coreboot-sdk to hub.docker.com"
	@echo "  coreboot-jenkins-node        - Build coreboot-jenkins-node container"
	@echo "  upload-coreboot-jenkins-node - Upload coreboot-jenkins-node to hub.docker.com"
	@echo "  doc.coreboot.org             - Build doc.coreboot.org container"
	@echo "  clean-coreboot-containers    - Remove all docker coreboot containers"
	@echo "  clean-coreboot-images        - Remove all docker coreboot images"
	@echo "  docker-clean                 - Remove docker coreboot containers & images"
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
	@echo "  docker-build-docs            - Build the documentation"
	@echo "  docker-livehtml-docs         - Run sphinx-autobuild"
	@echo
	@echo "Variables:"
	@echo "  COREBOOT_JENKINS_PORT=$(COREBOOT_JENKINS_PORT)"
	@echo "  COREBOOT_JENKINS_CACHE_DIR=$(COREBOOT_JENKINS_CACHE_DIR)"
	@echo "  COREBOOT_JENKINS_CONTAINER=$(COREBOOT_JENKINS_CONTAINER)"
	@echo "  COREBOOT_IMAGE_TAG=$(COREBOOT_IMAGE_TAG)"
	@echo "  DOCKER_COMMIT=$(DOCKER_COMMIT)"

.PHONY: test-docker test-docker-login
.PHONY: coreboot-jenkins-node upload-coreboot-jenkins-node
.PHONY: coreboot-sdk upload-coreboot-sdk
.PHONY: doc.coreboot.org
.PHONY: clean-coreboot-containers clean-coreboot-images
.PHONY: docker-abuild
.PHONY: docker-what-jenkins-does docker-shell docker-jenkins-server docker-jenkins-attach
.PHONY: docker-build-docs docker-livehtml-docs
.PHONY: help
