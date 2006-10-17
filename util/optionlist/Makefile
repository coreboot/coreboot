all: html wiki

html:
	./mkOptionList.py
	@#saxon  Options.xml Options.xsl >Options.html
	xsltproc Options.xsl Options.xml > Options.html
	
wiki:
	./mkOptionList.py
	xsltproc Options-wiki.xsl Options.xml > Options.wiki

clean:
	rm -rf Options.xml Options.html Options.wiki
