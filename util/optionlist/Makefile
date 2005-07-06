all:
	./mkOptionList.py
	@#saxon  Options.xml Options.xsl >Options.html
	xsltproc Options.xsl Options.xml > Options.html
	
clean:
	rm Options.xml Options.html
