CC=gcc
CPPFLAGS=
CFLAGS= -g -Wall -Werror $(CPPFLAGS)
CPROF_FLAGS=-pg -fprofile-arcs
BUILD_DIR=build

default: $(BUILD_DIR)/romcc

$(BUILD_DIR)/romcc: romcc.c $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/romcc_pg: romcc.c $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPROF_FLAGS) -o $@ $<

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

test: $(BUILD_DIR)/romcc
	./test.sh all

test-simple: $(BUILD_DIR)/romcc
	./test.sh simple

test-linux: $(BUILD_DIR)/romcc
	./test.sh linux

clean distclean:
	rm -rf $(BUILD_DIR)

.PHONY: all test test-simple test-linux clean distclean
