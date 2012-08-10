 ##
 ## This file is part of the vx900_ephy utlity.
 ##
 ##
 ## Copyright (C) 2011-2012 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 ##
 ## This program is free software: you can redistribute it and/or modify
 ## it under the terms of the GNU General Public License as published by
 ## the Free Software Foundation, either version 2 of the License, or
 ## (at your option) any later version.
 ##
 ## This program is distributed in the hope that it will be useful,
 ## but WITHOUT ANY WARRANTY; without even the implied warranty of
 ## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ## GNU General Public License for more details.
 ##
 ## You should have received a copy of the GNU General Public License
 ## along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ##

CC=gcc

SRCS = vx900_ephy.c
OBJS = $(sort ${SRCS:.c=.o})

all: vx900_ephy

vx900_ephy: $(OBJS)
	$(CC) $< -o $@ -lpci

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJS)