/*
Test if this is the main cpu BSP
*/
logical_cpu_start:
        movl    $0x1B , %ecx
        rdmsr
        testl   $(1 << 8) , %eax
        jnz     logical_cpu_end

	/* This is the 2nd virtual cpu, so enable cache and sleep */
        movl    %cr0, %eax
        andl    $0x9fffffff, %eax
        movl    %eax, %cr0

        /* Signal the BSP */
        movl    $0x20f, %ecx    /* mtrr[7] physical mask register */
        xorl    %edx, %edx
        movl    $0x01000, %eax
        wrmsr

1:      hlt   
	jmp     1b 

logical_cpu_end:

        /* Test if this physical cpu has virtual cpu's.
           If it does start the second virtual cpu so that it can
           enable cache in cr0.  Wait for it to finish, then
           continue. */

        movl    $0x20f, %ecx    /* mtrr[7] physical mask register */
        xorl    %eax,%eax       /* clear out the mttr used as a flag */
        xorl    %edx, %edx
        wrmsr


        /* test if this cpu has hyper-threading, & the # of virtual cpus */
        movl    $1, %eax
        cpuid
        andl    $(1 << 28), %edx
        jz      enable_cache_cr0_end    /* no hyper threading, so jump */
        shr     $16, %ebx
        andl    $0x0ff, %ebx
        cmpl    $1, %ebx
        jle     enable_cache_cr0_end    /* only 1 logical cpu, so jump */
        subl    $1, %ebx

cache_enable_logical_cpus:

        /* start logical cpu */
        movl    (APIC_ID  + APIC_DEFAULT_BASE ), %edi
        movl    %ebx, %eax
        shl     $24, %eax       /* add the logical cpu # for the apic addr */
        addl    %eax, %edi
        movl    %edi,( APIC_ICR2  + APIC_DEFAULT_BASE )
        movl    $_start, %edi
        shrl    $12, %edi
        andl    $0x0ff, %edi
        orl     $(APIC_DM_STARTUP | APIC_INT_ASSERT), %edi
        movl    %edi,( APIC_ICR  + APIC_DEFAULT_BASE )

        /* wait for message to be received */

1:      movl    ( APIC_ICR  + APIC_DEFAULT_BASE ), %edi
        andl    $APIC_ICR_BUSY, %edi
        jnz     1b

        /* delay waiting for virtual cpu to enable cache */

2:      rep; nop
        movl    $0x20f, %ecx    /* mtrr[7] physical mask register */
        rdmsr
        andl    %eax,%eax
        jz      2b
        xorl    %eax,%eax       /* clear out the mttr used as a flag */
        xorl    %edx, %edx
        wrmsr

        subl    $1, %ebx
        jnz     cache_enable_logical_cpus

enable_cache_cr0_end:

        /* delay loop to give logical cpu time to halt */
        /* test showed 400 cycles needed, so use 1000 to be safe*/
        xorl    %ecx, %ecx
1:      rdtsc
        movl    %eax, %ebx
2:      rep; nop
        rdtsc
        movl    %eax, %edx
        subl    %ebx, %eax
        jc      1b
        addl    %eax, %ecx
        movl    %edx, %ebx
        cmpl    $1000, %ecx
        jc      2b


