## SPDX-License-Identifier: GPL-2.0-only
# simple makefile for the project

OUTPUT_DIR = generate
PROJECT_NAME = intelp2m

default:
	go version
	go build -v -o $(PROJECT_NAME)

test:
	go test ./... -v -count=1

clean:
	rm -Rf $(PROJECT_NAME) $(OUTPUT_DIR)
