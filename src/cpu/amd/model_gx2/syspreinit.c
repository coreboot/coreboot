/* ***************************************************************************/
/* **/
/* *	StartTimer1*/
/* **/
/* *	Entry: none*/
/* *	Exit: Starts Timer 1 for port 61 use*/
/* *	Destroys: Al,*/
/* **/
/* ***************************************************************************/
void
StartTimer1(void){
	outb(0x56, 0x43);
	outb(0x12, 0x41);
}

void
SystemPreInit(void){

	/* they want a jump ... */
	__asm__("jmp .+2\ninvd\njmp.+2\n");
	StartTimer1();
}
