ALL: $(shell echo *.g | sed s/\\.g/\\.py/g )

%.py: %.g yapps2.py yappsrt.py Makefile
	python yapps2.py $<

DOC:	yapps2.ps yapps2.pdf manual/index.html

yapps2.ps: yapps2.dvi
	dvips -q yapps2.dvi -o yapps2.ps

yapps2.pdf: yapps2.ps
	ps2pdf yapps2.ps

yapps2.dvi: yapps2.tex
	latex yapps2.tex

manual/index.html: yapps2.aux yapps2.tex
	rm manual/yapps2.css
	latex2html -dir 'manual' -mkdir -lcase_tags -font_size 12pt -split 4 -toc_depth 4 -html_version 4.0,unicode,table -t 'Yapps 2.0 Manual' -address 'Amit J Patel, amitp@cs.stanford.edu' -info 0 -show_section_numbers -up_title 'Yapps Page' -up_url 'http://theory.stanford.edu/~amitp/yapps/' -strict -image_type png yapps2.tex
	echo '@import url("http://www-cs-students.stanford.edu/~amitp/amitp.css");' > manual/yapps2-new.css
	echo 'hr { display:none; }' >> manual/yapps2-new.css
	echo 'h1 br, h2 br { display:none; }' >>manual/yapps2-new.css
	cat manual/yapps2.css >> manual/yapps2-new.css
	rm manual/yapps2.css
	mv manual/yapps2-new.css manual/yapps2.css

DISTRIB:
	cd ..; zip -u yapps2.zip yapps2/{LICENSE,yapps2.py,yappsrt.py,parsedesc.g,examples/*.g,NOTES,yapps2.tex,Makefile,manual/*.html,manual/*.css,manual/*.png}

clean:
	rm -f config.py yappsrt.pyc parsedesc.py
