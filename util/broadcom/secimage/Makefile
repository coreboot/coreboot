#
# Copyright (C) 2015 Broadcom Corporation
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation version 2.
#
# This program is distributed "as is" WITHOUT ANY WARRANTY of any
# kind, whether express or implied; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

TARGET = secimage
OBJS = crypto.o io.o misc.o sbi.o
CC = gcc
RM = rm
CFLAGS += -Wall -g

LIBS = -lssl -lcrypto -lz

%.o : %.c
	$(CC) -c $(CFLAGS) -o $@ $<

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

install:
	install -d $(DESTDIR)/usr/bin
	install $(TARGET) $(DESTDIR)/usr/bin

.PHONY: clean

clean:
	$(RM) -f $(TARGET) $(OBJS)
