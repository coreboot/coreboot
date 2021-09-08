SPD_GEN = bin/spd_gen

all: $(SPD_GEN)

$(SPD_GEN):
	go build -o $(SPD_GEN) src/spd_gen/*.go

clean:
	rm -rf bin/

.PHONY: all
