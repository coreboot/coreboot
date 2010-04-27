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
  off_t nvram;
  size_t length = 0x1000;

  fd = open("/proc/bus/pci/00/0a.1",O_RDONLY);
  lseek(fd,0x10,0);
  read(fd,&nvram,sizeof(nvram));
  close(fd);
  //printf("Star %x\n",nvram);

  if((fd = open("/dev/mem",O_RDWR)) != -1)
    {
      v = mmap(0, length, PROT_READ | PROT_WRITE, MAP_SHARED,fd,nvram);
      fprintf(stderr, "mmap returns %p\n", v);

      if ( v == (void *) -1)
	{
	  perror("mmap");
	  exit(1);
	}
    } else {
      perror("open /dev/mem");
      exit(1);
    }

    for( i = 0x836 ; i < 0x840 ; i++){
	*(unsigned char *)(v+i) = 0;
    }


}
