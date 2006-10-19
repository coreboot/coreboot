# 
# Copyright (C) 2005 Stefan Reinauer <stepan@coresystems.de>
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
# 

all: html wiki

html:
	./mkOptionList.py
	@#saxon  Options.xml Options.xsl > Options.html
	xsltproc Options.xsl Options.xml > Options.html
	
wiki:
	./mkOptionList.py
	xsltproc Options-wiki.xsl Options.xml > Options.wiki

clean:
	rm -rf Options.xml Options.html Options.wiki

