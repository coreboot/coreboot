	movw	$sis950_init_table, %si		# unlock SiS 950 LPC
	movw	$0x05, %cx			# select Clock Selection
	movw	$0x2e, %dx			# and Flash ROM I/F 
	rep
	outsb
	movb	$0xfd, %al			# enable write, CLKIN = 24 MHZ
	outb	%al, $0x2f
