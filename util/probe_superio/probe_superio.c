#include <stdio.h>
#include <sys/io.h>

/* well, they really thought this throught, eh? Family is 8 bits!!!! */
char *familyid[] = {
	[0xf1] = "pc8374 (winbond, was NS)"
};

/* eventually, if you care, break this out into a file. For now, I don't know 
  * if we need this. 
  */

unsigned char regval(unsigned short port, unsigned short reg) {
	outb(reg, port);
	return inb(port+1);
}

void
dump_ns8374(unsigned short port) {
	printf("Enables: 21=%02x, 22=%02x, 23=%02x, 24=%02x, 26=%02x\n", 
			regval(port,0x21), regval(port,0x22), 
			regval(port,0x23), regval(port,0x24), regval(port,0x26));
	/* check COM1. This is all we care about at present. */
	printf("COM 1 is Globally %s\n", regval(port,0x26)&8 ? "disabled" : "enabled");
	/* select com1 */
	outb(0x7, port);
	outb(3, port+1);
	printf("COM 1 is locally %s\n", regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf("COM1 60=%02x, 61=%02x, 70=%02x, 71=%02x, 74=%02x, 75=%02x, f0=%02x\n", 
		regval(port, 0x60), regval(port, 0x61), regval(port, 0x70), regval(port, 0x71),
		regval(port, 0x74), regval(port, 0x75), regval(port, 0xf0));

}

void
probe_idregs(unsigned short port){
	unsigned char id;
	int i;
	outb(0x20, port);
	if (inb(port) != 0x20)  {
		printf("probing 0x%04x, failed (0x%02x), data returns 0x%02x\n", 
					port, inb(port), inb(port+1));
		return;
	}
	id = inb(port+1);
	printf("Probe of id returns %02x\n", id);
	if (id == 0xff)
		return;

	printf("%s\n", familyid[id]);
	switch(id) {
		case 0xf1:
			dump_ns8374(port);
			break;
		default:
			printf("no dump for 0x%02x\n", id);
			break;
	}
}

void
probe_superio(unsigned short port) {
	probe_idregs(port);
}

int
main(int argc, char *argv[]){
	unsigned short port;

	if (iopl(3) < 0) {
		perror("iopl");
		exit(1);
	}

	/* try the 2e */
	probe_superio(0x2e);
	/* now try the 4e */
	probe_superio(0x4e);
}
