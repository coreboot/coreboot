	movw	$0xf000, %ax
	movw	%ax, %ds 
	xorw	%si, %si
	xorw	%di, %di
	movw	$0xffff, %cx
	rep
	movsb

	movw	$DOC_WIN_SEG, %ax
	movw	%ax, %ds
