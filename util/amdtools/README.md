# amdtools
Various tools for AMD platforms

## A tool to update the SPI speed set in the EFS table
This works for Stoney Ridge and Zen class AMD processors.
 - update_efs_spi_speed

## A set of tools to compare (extended) K8 memory settings.
 - k8-compare-pci-space.pl
 - k8-interpret-extended-memory-settings.pl
 - k8-read-mem-settings.sh
 - parse-bkdg.pl

Before you can use them, you need to massage the relevant BKDG
sections into useable data. Here's how.

  1. First, you need to acquire a copy of the K8 BKDG. Go here:
     Rev F: https://www.amd.com/content/dam/amd/en/documents/archived-tech-docs/programmer-references/32559.pdf
  2. Make sure pdftotext is installed (it's in the poppler-utils
     package on Debian/Ubuntu).
  3. Run the bkdg through pdftotext:
     `pdftotext -layout 32559.pdf 32559.txt`
  4. Extract sections 4.5.15 - 4.5.19 from the file, and save it
     separately, say as bkdg-raw.data.
  5. Finally run the txt file through the parse-bkdg.pl script like so:
     `parse-bkdg.pl < bkdg-raw.data > bkdg.data`

Now we have the bkdg.data file that is used by the other scripts.

If you want to test the scripts without doing all this work, you
can use some sample input files from the 'example_input/' directory.
