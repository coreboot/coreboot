#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/mount.h>

#include <linux/mtd/mtd.h>
int region_erase(int Fd, int start, int count, int unlock, int regcount)
{
	int i, j;
	region_info_t * reginfo;

	reginfo = calloc(regcount, sizeof(region_info_t));

	for(i = 0; i < regcount; i++)
	{
		reginfo[i].regionindex = i;
		if(ioctl(Fd,MEMGETREGIONINFO,&(reginfo[i])) != 0)
			return 8;
		else
			printf("Region %d is at %d of %d sector and with sector "
					"size %x\n", i, reginfo[i].offset, reginfo[i].numblocks,
					reginfo[i].erasesize);
	}

	// We have all the information about the chip we need.

	for(i = 0; i < regcount; i++)
	{ //Loop through the regions
		region_info_t * r = &(reginfo[i]);

		if((start >= reginfo[i].offset) &&
				(start < (r->offset + r->numblocks*r->erasesize)))
			break;
	}

	if(i >= regcount)
	{
		printf("Starting offset %x not within chip.\n", start);
		return 8;
	}

	//We are now positioned within region i of the chip, so start erasing
	//count sectors from there.

	for(j = 0; (j < count)&&(i < regcount); j++)
	{
		erase_info_t erase;
		region_info_t * r = &(reginfo[i]);

		erase.start = start;
		erase.length = r->erasesize;

		if(unlock != 0)
		{ //Unlock the sector first.
			if(ioctl(Fd, MEMUNLOCK, &erase) != 0)
			{
				perror("\nMTD Unlock failure");
				close(Fd);
				return 8;
			}
		}
		printf("\rPerforming Flash Erase of length %lu at offset 0x%lx",
				erase.length, erase.start);
		fflush(stdout);
		if(ioctl(Fd, MEMERASE, &erase) != 0)
		{
			perror("\nMTD Erase failure");
			close(Fd);
			return 8;
		}


		start += erase.length;
		if(start >= (r->offset + r->numblocks*r->erasesize))
		{ //We finished region i so move to region i+1
			printf("\nMoving to region %d\n", i+1);
			i++;
		}
	}

	printf(" done\n");

	return 0;
}

int non_region_erase(int Fd, int start, int count, int unlock)
{
	mtd_info_t meminfo;

	if (ioctl(Fd,MEMGETINFO,&meminfo) == 0)
	{
		erase_info_t erase;

		erase.start = start;

		erase.length = meminfo.erasesize;
		printf("Erase Unit Size 0x%lx, ", meminfo.erasesize);

		for (; count > 0; count--) {
			printf("\rPerforming Flash Erase of length %lu at offset 0x%lx",
					erase.length, erase.start);
			fflush(stdout);

			if(unlock != 0)
			{
				//Unlock the sector first.
				printf("\rPerforming Flash unlock at offset 0x%lx",erase.start);
				if(ioctl(Fd, MEMUNLOCK, &erase) != 0)
				{
					perror("\nMTD Unlock failure");
					close(Fd);
					return 8;
				}
			}

			if (ioctl(Fd,MEMERASE,&erase) != 0)
			{      
				perror("\nMTD Erase failure");
				close(Fd);
				return 8;
			}
			erase.start += meminfo.erasesize;
		}
		printf(" done\n");
	}
	return 0;
}

int main(int argc,char *argv[])
{
	int regcount;
	int Fd;
	int start;
	int count;
	int unlock;
	int res = 0; 

	if (1 >= argc)
	{
		fprintf(stderr,"You must specify a device\n");
		return 16;
	}

	if (argc > 2)
		start = strtol(argv[2], NULL, 0);
	else
		start = 0;

	if (argc > 3)
		count = strtol(argv[3], NULL, 0);
	else
		count = 1;

	if(argc > 4)
		unlock = strtol(argv[4], NULL, 0);
	else
		unlock = 0;


	// Open and size the device
	if ((Fd = open(argv[1],O_RDWR)) < 0)
	{
		fprintf(stderr,"File open error\n");
		return 8;
	}

	printf("Erase Total %d Units\n", count);

	if (ioctl(Fd,MEMGETREGIONCOUNT,&regcount) == 0)
	{
		if(regcount == 0)
		{
			res = non_region_erase(Fd, start, count, unlock);
		}
		else
		{
			res = region_erase(Fd, start, count, unlock, regcount);
		}
	}
			
	return res;
}
