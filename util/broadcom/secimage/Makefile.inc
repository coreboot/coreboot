secimageobj :=
secimageobj += crypto.o
secimageobj += io.o
secimageobj += misc.o
secimageobj += sbi.o

LIBS = -lssl -lcrypto -lz

additional-dirs += $(objutil)/broadcom/secimage

$(objutil)/broadcom/secimage/%.o: $(top)/util/broadcom/secimage/%.c
	printf "    HOSTCC     $(subst $(objutil)/,,$(@))\n"
	$(HOSTCC) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/broadcom/secimage/secimage: \
		$(addprefix $(objutil)/broadcom/secimage/,$(secimageobj))
	printf "    HOSTCC     $(subst $(objutil)/,,$(@)) (link)\n"
	$(HOSTCC) -o $@ $^ $(LIBS)
