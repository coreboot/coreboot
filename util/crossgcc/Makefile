all: build

build:
	bash ./buildgcc

.PHONY: build-without-gdb
build-without-gdb:
	bash ./buildgcc --skip-gdb

clean:
	rm -rf xgcc

.PHONY: all build clean
