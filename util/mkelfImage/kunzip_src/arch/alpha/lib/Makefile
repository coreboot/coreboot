ARCH_OPTIONS=

OBJECTS += __divqu.o __remqu.o __divlu.o __remlu.o

$(OBJDIR)/__divqu.o: $(SRC)/arch/alpha/lib/divide.S
	$(CC) $(CFLAGS) -DDIV -c -o $@ $^

$(OBJDIR)/__remqu.o: $(SRC)/arch/alpha/lib/divide.S
	$(CC) $(CFLAGS) -DREM -c -o $@ $^

$(OBJDIR)/__divlu.o: $(SRC)/arch/alpha/lib/divide.S
	$(CC) $(CFLAGS) -DDIV -DINTSIZE -c -o $@ $^

$(OBJDIR)/__remlu.o: $(SRC)/arch/alpha/lib/divide.S
	$(CC) $(CFLAGS) -DREM -DINTSIZE -c -o $@ $^


