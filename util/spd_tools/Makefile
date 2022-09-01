SPD_GEN = bin/spd_gen
PART_ID_GEN = bin/part_id_gen

all: $(SPD_GEN) $(PART_ID_GEN)

$(SPD_GEN): src/spd_gen/*.go
	go build -o $@ $^

$(PART_ID_GEN): src/part_id_gen/*.go
	go build -o $@ $^

clean:
	rm -rf bin/

.PHONY: all
