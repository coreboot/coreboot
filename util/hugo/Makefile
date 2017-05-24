.SILENT:
.PHONY: error public

error:
	echo please state a valid target
	exit 1

themes/hugo-material-docs:
	mkdir -p themes
	git clone https://github.com/digitalcraftsman/hugo-material-docs.git themes/hugo-material-docs

public: themes/hugo-material-docs
	docker run --rm -it -v $$PWD:/src -v $$PWD/../../Documentation:/src/content -v /srv/www/vhosts/www.coreboot.org/Documentation:/src/public jguyomard/hugo-builder hugo --forceSyncStatic --cleanDestinationDir
