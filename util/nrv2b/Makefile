GCC=gcc

nrv2b:	nrv2b.c
	$(GCC) -O2 -DENCODE -DDECODE -DMAIN -DVERBOSE -DNDEBUG -DBITSIZE=32 -DENDIAN=0 -o $@ $<

clean:
	rm -f nrv2b
