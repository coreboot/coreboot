## SPDX-License-Identifier: GPL-2.0-only
sort: sort-spelling.txt

sort-%: %
	csplit --prefix $<- $< '/^$$/'
	LC_ALL="en_US.UTF-8" LANG= sort -u $<-01 | cat $<-00 - > $<
	rm -f $<-0[01]
