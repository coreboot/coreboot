#include <stdio.h>
#include <uthash.h>
#include <sys/gmon_out.h>
#include <stdlib.h>

#define GMON_SEC "seconds        s"
uint32_t mineip = 0xffffffff;
uint32_t maxeip = 0;

/* a hash structure to hold the arc */
struct arec {
	uint32_t eip;
	uint32_t from;
	uint32_t count;
	UT_hash_handle hh;
};

struct arec *arc = NULL;

void note_arc(uint32_t eip, uint32_t from)
{
	struct arec *s;

	HASH_FIND_INT(arc, &eip, s);
	if (s == NULL) {
		s = malloc(sizeof(struct arec));
		s->eip = eip;
		s->from = from;
		s->count = 1;
		if (eip > maxeip)
			maxeip = eip;
		if (eip < mineip)
			maxeip = eip;

		HASH_ADD_INT(arc, eip, s);
	} else {
		s->count++;
	}
}

int main(int argc, char* argv[])
{
	FILE *f, *fo;
	struct arec *s;
	uint32_t eip, from, tmp;
	uint8_t tag;
	uint16_t hit;

	if ( argc < 2 )
	{
		fprintf(stderr, "Please specify the coreboot trace log as parameter\n");
		return 1;
	}

	f = fopen(argv[1], "r");
	fo = fopen("gmon.out", "w+");

	if ((f == NULL) || (fo == NULL)) {
		fprintf(stderr, "Unable to manipulate with the input file\n");
		return 1;
	}

	while (!feof(f)) {
		if (fscanf(f, "~%x(%x)%*[^\n]\n", &eip, &from) == 2) {
			note_arc(eip, from);
		} else if (fscanf(f, "%*c~%x(%x)%*[^\n]\n", &eip, &from) == 2) {
			note_arc(eip, from);
		} else {
			/* just drop a line */
			tmp = fscanf(f, "%*[^\n]\n");
		}
	}

	/* write gprof header */
	fwrite(GMON_MAGIC, 1, sizeof(GMON_MAGIC) - 1, fo);
	tmp = GMON_VERSION;
	fwrite(&tmp, 1, sizeof(tmp), fo);
	tmp = 0;
	fwrite(&tmp, 1, sizeof(tmp), fo);
	fwrite(&tmp, 1, sizeof(tmp), fo);
	fwrite(&tmp, 1, sizeof(tmp), fo);
	/* write fake histogram */
	tag = GMON_TAG_TIME_HIST;
	fwrite(&tag, 1, sizeof(tag), fo);
	fwrite(&mineip, 1, sizeof(mineip), fo);
	fwrite(&maxeip, 1, sizeof(maxeip), fo);
	/* size of histogram */
	tmp = 1;
	fwrite(&tmp, 1, sizeof(tmp), fo);
	/* prof rate */
	tmp = 1000;
	fwrite(&tmp, 1, sizeof(tmp), fo);
	fwrite(GMON_SEC, 1, sizeof(GMON_SEC) - 1, fo);
	hit = 1;
	fwrite(&hit, 1, sizeof(hit), fo);

	/* write call graph data */
	tag = GMON_TAG_CG_ARC;
	for (s = arc; s != NULL; s = s->hh.next) {
		fwrite(&tag, 1, sizeof(tag), fo);
		fwrite(&s->from, 1, sizeof(s->from), fo);
		fwrite(&s->eip, 1, sizeof(s->eip), fo);
		fwrite(&s->count, 1, sizeof(s->count), fo);
	}

	fclose(fo);
	return 0;
}
