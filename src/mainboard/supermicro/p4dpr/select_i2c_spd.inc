	SIO_ENTER_PNP_MODE()

	SIO_READ_CONFIG($0x2b)
	movb	%al, %bl
	orb	$0x30, %bl
	SIO_WRITE_CONFIG(%bl, $0x2b)

	/* Enable the GPIO device */
	SIO_SET_LOGICAL_DEVICE($GPIO_PORT2_DEVICE)
	SIO_WRITE_CONFIG($1, $0x30) 

	SIO_READ_CONFIG($0xf0)
	movb	%al, %bl
	andb	$~(1 << 3), %bl
	SIO_WRITE_CONFIG(%bl, $0xf0)

	SIO_EXIT_PNP_MODE()
