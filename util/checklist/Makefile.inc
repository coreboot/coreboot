#
# This file is part of the coreboot project.
#
# Copyright (C) 2016 Intel Corporation.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

###########################################################################
# Instructions
###########################################################################
#
# Create new control files for checklist:
#
#     1. Remove any selection for CREATE_BOARD_CHECKLIST
#     2. Remove any selection for MAKE_CHECKLIST_PUBLIC
#     3. make
#     4. nm build/cbfs/fallback/<stage>.debug > <stage>_symbols.txt
#     6. sed 's/^...........//' <stage>_symbols.txt > <stage>_complete.dat
#     7. grep -F " W " <stage>_symbols.txt | sed 's/^...........//' \
#             > <stage>_optional.dat
#     8. Edit <stage>_complete.dat to remove any symbols that are not
#        desired in the report
#     9. Edit <stage>_optional.dat to remove any symbols that are
#        required to be implemented
#
# Create a board checklist:
#
#     1. select CREATE_BOARD_CHECKLIST
#     2. Optionally: select MAKE_CHECKLIST_PUBLIC
#     3. Specify CONFIG_CHECKLIST_DATA_FILE_LOCATION
#     4. make
#
# Build Errors:
#     * No checklist built - verify CREATE_BOARD_CHECKLIST is selected in
#           board Kconfig file.  Do a make clean
#     * <stage>_complete.dat not found - verify that
#           CONFIG_CHECKLIST_DATA_FILE_LOCATION points to the directory
#           containing the checklist data files.  Build the checklist
#           data files if necessary.
#     * Segmentation fault - most likely caused by $(NM_$(class)) not being
#           set.
#
###########################################################################
# Build the board implementation checklist
###########################################################################

# Only build the checklist for boards under development
ifeq ($(CONFIG_CREATE_BOARD_CHECKLIST),y)

#
# Extract the symbol table from the image
#
%.symbol_table: %.elf %.debug
	$(NM_$(class)) $(*D)/$(*F).debug > $@
	$(NM_$(class)) $< >> $@

#
# All symbols in the image
#
# 1. Remove the address and symbol type
# 2. Sort the table into alphabetical order
# 3. Remove any duplicates
#
%.symbols: %.symbol_table
	sed 's/^...........//' $< > $@.tmp
	sort $@.tmp > $@.tmp2
	uniq $@.tmp2 > $@
	rm $@.tmp $@.tmp2

#
# Weak symbols in the image
#
# 1. Find the weak symbols
# 2. Remove the address and symbol type
# 3. Sort the table into alphabetical order
# 4. Remove any duplicates
#
%.weak: %.symbol_table
	grep -F " W " $< | sed 's/^...........//' > $@.tmp
	sort $@.tmp > $@.tmp2
	uniq $@.tmp2 > $@
	rm $@.tmp $@.tmp2

#
# Expected symbols in the image
#
# 1. Get the complete list of expected symbols in the image
# 2. Sort the table into alphabetical order
# 3. Remove any duplicates
#
%.expected: %.symbol_table
	cp $(CONFIG_CHECKLIST_DATA_FILE_LOCATION)/$(basename $(*F))_complete.dat $@.tmp
	cat $(CONFIG_CHECKLIST_DATA_FILE_LOCATION)/$(basename $(*F))_optional.dat >> $@.tmp
	# If no separate verstage, combine verstage and romstage routines into a single list
	if [ "$(*F)" = "romstage" ]; then \
	  if [ ! -e $(*D)/verstage.elf ]; then \
	    if [ ! -e $(*D)/postcar.elf ]; then \
	      cat $(CONFIG_CHECKLIST_DATA_FILE_LOCATION)/verstage_complete.dat >> $@.tmp; \
	      cat $(CONFIG_CHECKLIST_DATA_FILE_LOCATION)/verstage_optional.dat >> $@.tmp; \
	    fi; \
	  fi; \
	fi
	sort $@.tmp > $@.tmp2
	uniq $@.tmp2 > $@
	rm $@.tmp $@.tmp2

#
# Optional symbols in the image
#
# 1. Get the list of optional symbols in the image
# 2. Sort the table into alphabetical order
# 3. Remove any duplicates
#
%.optional: %.symbol_table
	cp $(CONFIG_CHECKLIST_DATA_FILE_LOCATION)/$(basename $(*F))_optional.dat $@.tmp
	# If no separate verstage, combine verstage and romstage routines into a single list
	if [ "$(*F)" = "romstage" ]; then \
	  if [ ! -e $(*D)/verstage.elf ]; then \
	    if [ ! -e $(*D)/postcar.elf ]; then \
	      cat $(CONFIG_CHECKLIST_DATA_FILE_LOCATION)/verstage_optional.dat >> $@.tmp; \
	    fi; \
	  fi; \
	fi
	sort $@.tmp > $@.tmp2
	uniq $@.tmp2 > $@
	rm $@.tmp $@.tmp2

#
#   Expected Symbols Optional	Weak	Done	Type
#	no	yes	no	d/c	yes	Don't display
#	yes	no	no	no	no	Required - not implemented
#	yes	no	yes	no	no	Optional - not implemented
#	yes	yes	yes	yes	no	Optional - not implemented
#	yes	yes	no	no	yes	Required - implemented
#	yes	yes	yes	no	yes	Required - implemented
#
# Implemented routines are in the symbol table and are not weak
#
# 1. Remove expected symbols which are not in the image (not implemented yet)
# 2. Remove weak symbols from the list (not implemented yet)
#
%.done: %.symbols %.expected %.weak %.optional
	comm -12 $(*D)/$(*F).expected $(*D)/$(*F).symbols | sed "s/^[ \t]*//" > $@.tmp
	comm -23 $@.tmp $(*D)/$(*F).weak | sed "s/^[ \t]*//" > $@
	rm $@.tmp

#
# Remove any routines that are implemented
#
%.optional2: %.optional %.done
	comm -23 $^ | sed "s/^[ \t]*//" > $@

#
# Remove any implemented or optional routines
#
%.tbd: %.expected %.done %.optional2
	comm -23 $(*D)/$(*F).expected $(*D)/$(*F).done | sed "s/^[ \t]*//" > $@.tmp
	comm -23 $@.tmp $(*D)/$(*F).optional2 | sed "s/^[ \t]*//" > $@
	rm $@.tmp

#
# Build the implementation table for each stage
# 1. Color code the rows
#    *  Done table rows are in green
#    *  Optional table rows are in yellow
#    *  TBD table rows are in red
# 2. Add the row termination
# 3. Sort the rows into alphabetical order
#
%.table_rows: %.optional2 %.done %.expected %.tbd
	sed -e 's/^/<tr bgcolor=#c0ffc0><td>Required<\/td><td>/' $(*D)/$(basename $(*F)).done > $@.tmp
	sed -e 's/^/<tr bgcolor=#ffffc0><td>Optional<\/td><td>/' $(*D)/$(basename $(*F)).optional2 >> $@.tmp
	if [ -s $(*D)/$(basename $(*F)).tbd ]; then \
	  sed -e 's/^/<tr bgcolor=#ffc0c0><td>Required<\/td><td>/' $(*D)/$(basename $(*F)).tbd >> $@.tmp; \
	fi
	sed -e 's/$$/<\/td><\/tr>/' -i $@.tmp
	sort -t ">" -k4 $@.tmp > $@
	rm $@.tmp

#
# Count the lines in the done file
#
done_lines = $$(wc -l $(*D)/$(basename $(*F)).done | sed 's/ .*//')

#
# Count the lines in the optional file
#
optional_lines = $$(wc -l $(*D)/$(basename $(*F)).optional2 | sed 's/ .*//')

#
# Count the lines in the expected file
#
expected_lines = $$(wc -l $(*D)/$(basename $(*F)).expected | sed 's/ .*//')

# Compute the percentage done by routine count
percent_complete = $$(($(done_lines) * 100 / ($(expected_lines) - $(optional_lines))))

#
# Build the table
# 1. Add the table header
# 2. Add the table rows
# 3. Add the table trailer
#
%.html: %.table_rows
	echo "<table border=1>" > $@
	echo "<tr><th colspan=2>$(basename $(*F)): $(percent_complete)% Done</th></tr>" >> $@
	echo "<tr><th>Type</th><th>Routine</td></tr>" >> $@
	cat $< >> $@
	echo "</table>" >> $@

#
# Determine which HTML files to include into the webpage
#
ifeq ($(CONFIG_C_ENVIRONMENT_BOOTBLOCK),y)
html_table_files += $(objcbfs)/bootblock.html
endif
ifeq ($(CONFIG_SEPARATE_VERSTAGE),y)
html_table_files += $(objcbfs)/verstage.html
endif
html_table_files += $(objcbfs)/romstage.html
ifeq ($(CONFIG_POSTCAR_STAGE),y)
html_table_files += $(objcbfs)/postcar.html
endif
html_table_files += $(objcbfs)/ramstage.html

#
# Create a list with each file on a separate line
#
list_of_html_files = $(subst _NEWLINE_,${\n},${html_table_files})

#
# Get the date for the webpage
#
current_date_time = $$(date +"%Y/%m/%d %T %Z")

#
# Build the webpage from the implementation tables
# 1. Add the header to the webpage
# 2. Add the legend to the webpage
# 3. Use a table to place stage tables side-by-side
# 4. Add the stage tables to the webpage
# 5. Separate the stage tables
# 6. Terminate the outer table
# 7. Add the trailer to the webpage
#
$(obj)/$(CONFIG_MAINBOARD_PART_NUMBER)_checklist.html: $(html_table_files)
	echo "<html>" > $@
	echo "<head>" >> $@
	echo "<title>$(CONFIG_MAINBOARD_PART_NUMBER) Implementation Status</title>" >> $@
	echo "</title>" >> $@
	echo "<body>" >> $@
	echo "<h1>$(CONFIG_MAINBOARD_PART_NUMBER) Implementation Status<br>$(current_date_time)</h1>" >> $@
	echo "<table>" >> $@
	echo "  <tr><td colspan=2><b>Legend</b></td></tr>" >> $@
	echo "  <tr><td bgcolor=\"#ffc0c0\">Red</td><td>Required - To-be-implemented</td></tr>" >> $@
	echo "  <tr><td bgcolor=\"#ffffc0\">Yellow</td><td>Optional</td></tr>" >> $@
	echo "  <tr><td bgcolor=\"#c0ffc0\">Green</td><td>Implemented</td></tr>" >> $@
	echo "</table>" >> $@
	echo "<table>" >> $@
	echo "  <tr valign=\"top\">" >> $@
	for table in $(list_of_html_files); do \
	  echo "    <td>" >> $@; \
	  cat $$table >> $@; \
	  echo "    </td>" >> $@; \
	  echo "    <td width=5>&nbsp;</td>" >> $@; \
	done
	echo "  </tr>" >> $@
	echo "</table>" >> $@
	echo "</body>" >> $@
	echo "</html>" >> $@

#
# Copy the output file into the Documentation directory
#
Documentation/$(CONFIG_MAINBOARD_VENDOR)/Board/$(CONFIG_MAINBOARD_PART_NUMBER)_checklist.html: $(obj)/$(CONFIG_MAINBOARD_PART_NUMBER)_checklist.html
	if [ ! -d Documentation/$(CONFIG_MAINBOARD_VENDOR) ]; then \
	  mkdir Documentation/$(CONFIG_MAINBOARD_VENDOR); \
	fi
	if [ ! -d Documentation/$(CONFIG_MAINBOARD_VENDOR)/Board ]; then \
	  mkdir Documentation/$(CONFIG_MAINBOARD_VENDOR)/Board; \
	fi
	cp $< $@

#
# Determine where to place the output file
#
ifeq ($(CONFIG_MAKE_CHECKLIST_PUBLIC),y)
INTERMEDIATE+=Documentation/$(CONFIG_MAINBOARD_VENDOR)/Board/$(CONFIG_MAINBOARD_PART_NUMBER)_checklist.html
else
INTERMEDIATE+=$(obj)/$(CONFIG_MAINBOARD_PART_NUMBER)_checklist.html
endif

endif
