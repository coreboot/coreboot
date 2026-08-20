## SPDX-License-Identifier: GPL-2.0-only

BIN_DIR := bin
SPD_GEN = $(BIN_DIR)/spd_gen
PART_ID_GEN = $(BIN_DIR)/part_id_gen

all: $(SPD_GEN) $(PART_ID_GEN)

$(SPD_GEN): src/spd_gen/*.go
	mkdir -p $(dir $@)
	go build -o $@.$$$$.tmp $^ && mv -f $@.$$$$.tmp $@

$(PART_ID_GEN): src/part_id_gen/*.go
	mkdir -p $(dir $@)
	go build -o $@.$$$$.tmp $^ && mv -f $@.$$$$.tmp $@

clean:
	rm -rf $(BIN_DIR)

.PHONY: all
