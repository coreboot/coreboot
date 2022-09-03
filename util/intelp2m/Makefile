## SPDX-License-Identifier: GPL-2.0-only

PROJECT_NAME := intelp2m
THIS_FILE := $(lastword $(MAKEFILE_LIST))
THIS_DIR  := $(abspath $(dir $(THIS_FILE)))
SCRIPTS_DIR := $(THIS_DIR)/scripts/linux

OUTPUT_DIR := $(THIS_DIR)/generate
VERSION ?= $(shell $(SCRIPTS_DIR)/version.sh)
LDFLAGS = "-X main.Version=$(VERSION)"

default: version
	go version
	go build -ldflags=$(LDFLAGS) -v

test:
	go test ./... -v -count=1

clean:
	rm -Rf $(THIS_DIR)/$(PROJECT_NAME) $(OUTPUT_DIR)

version:
	@echo $(VERSION)
