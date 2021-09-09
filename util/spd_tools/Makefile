SPD_GEN = bin/spd_gen
PART_ID_GEN = bin/part_id_gen

all: $(SPD_GEN) $(PART_ID_GEN)

$(SPD_GEN):
	go build -o $(SPD_GEN) src/spd_gen/*.go

$(PART_ID_GEN):
	go build -o $(PART_ID_GEN) src/part_id_gen/*.go

clean:
	rm -rf bin/

.PHONY: all
