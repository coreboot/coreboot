#include <stdio.h>
#include <fcntl.h>

#include <unistd.h>
#include <sys/mman.h>

main(int argc, char *argv[])
{
  int i;
  volatile unsigned char *cp;
  int fd;
  void *v;
  off_t nvram = 0xffe00000;
  size_t length = 0x200000;

  if ((argc > 1)  && (! strncmp(argv[1], "--help", 6))) {
    fprintf(stderr, 
	    "Usage: %s [high 16 bits of base address [size]]\n", argv[0]);
    fprintf(stderr, 
	    "Dumps dev mem, starting at either argv[1]<<16 or (default) ");
    fprintf(stderr, 
	    "0xffe00000 for 0x200000 bytes (i.e. last 2MB of memory\n");
    fprintf(stderr, "Example: %s 0xfff00000 0x100000\n");
    fprintf(stderr, "Dumps last 1M of memory to stdout\n");
    exit(0);
  }
  if (argc > 1)
    nvram = (strtol(argv[1], 0, 0)) << 16;
  if (argc > 2)
    length = (strtol(argv[2], 0, 0)) ;
  if((fd = open("/dev/mem",O_RDWR)) != -1)
    {
      v = mmap(0, length, PROT_READ | PROT_WRITE, MAP_SHARED,fd,nvram);
      fprintf(stderr, "mmap returns %p\n", v);
      
      if ( (int)v == -1)
	{
	  perror("mmap");
	  exit(1);
	}
    } else {
      perror("open /dev/mem");
      exit(1);
    }
  write(fileno(stdout), v, length);
}
