	xorw	%sp, %sp			# clear %sp
	xorw	%dx, %dx			# clear %dx, start of RAM
	movw	$0x800, %si			# point %si to CDSN Data area
	movw	$0x1000, %di			# point %di to CDSN Control area
	movw	$DOC_SPL_START_PAGE, %bp	# start page of LinuxBIOS

	movb	$0x84, %al			# Reset DOC Millennium
	call	doc_reset

	movb	$0x85, %al			# Turn into Normal Mode
	call	doc_reset

read_next_page:
	movw	$0x1000, %di			# point %di to CDSN Control area

flash_command:
	movb	$0x03, 0x04(%di)		# start command cycle
	movb	$0x00, (%si)			# issue flash command Read00
	call	doc_cycle_end

	movw	%bp, %bx			# %bp is current page number

flash_address:
#ifndef SIS550_BUG
	shll	$0x08, %ebx
	movb	$0x03, %cl			# this one is DANGEROUS but I am
						# sure the upper 3 bytes == 0x00

	movb	$0x05, 0x04(%di)		# start address cycle
0:
	movb	%bl, (%si)			# write address to CDSNIO
	shrw	$0x08, %bx			# shift next address byte
	loop	0b
#else /* SIS550_BUG */

	movb	$0x05, 0x04(%di)		# start address cycle

	movb	$0x00, (%si)
	movw	%bp, %bx
	movb	%bl, (%si)
	movb	%bh, (%si)
#endif

	call	doc_cycle_end

wait_for_flash_ready:
	call	doc_delay

	testb	$0x80, 0x04(%di)		# is flash ready ?
	jz	wait_for_flash_ready

	movb	0x1d(%di), %al			# init read pipeline
	movw	$0x100, %cx			# 1 page = 512 bytes
	movw	$0x800, %si			# point %si to CDSN Data area
	movw	%dx, %di			# restore saved current destination
	rep
	movsw

	movw	%di, %dx			# save current destination
	incw	%bp				# increse current page number
	cmpw	$(DOC_SPL_START_PAGE + DOC_SPL_SIZE_IN_PAGE), %bp
						# moved enough data ??
	jl	read_next_page			# no, read next page

	jmp	doc_mil_end			# jump to SPL vector

doc_reset:
	/* Input: AL = value write to DOC_CONTROL register
	   Clobberd: CX */
	movb	%al, 0x02(%di)			# write DoC Control retister
	movb	%al, 0x02(%di)			# twice
	ret					# End of doc_reset

doc_cycle_end:
	movb	$0x00, 0x1e(%di)		# flush write pepeline
	movb	$0x01, 0x04(%di)		# end command cycle
	ret

	/* delay by reding NOP register before polling the FLASH READY bit,
	   this is inlined to save a call/ret pair */
doc_delay:
	movb	$0x04, %cl			# this one is DANGEROUS but I am
						# sure the upper 3 bytes == 0x00
0:
	movb	0x20(%di), %al			# read DOC NOP retisger
	loop	0b				# four times
	ret

doc_mil_end:

