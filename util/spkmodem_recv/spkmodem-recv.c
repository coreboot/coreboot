/* spkmodem-recv.c - decode spkmodem signals */
/*
 *  Copyright (C) 2013  Vladimir 'phcoder' Serbinenko
 *
 *  spkmodem-recv is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  spkmodem-recv is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Compilation:  gcc -o spkmodem-recv spkmodem-recv  */
/* Usage: parec --channels=1 --rate=48000 --format=s16le | ./spkmodem-recv */

#define SAMPLES_PER_TRAME 240
#define FREQ_SEP_MIN 5
#define FREQ_SEP_MAX 15
#define FREQ_DATA_MIN 15
#define FREQ_DATA_THRESHOLD 25
#define FREQ_DATA_MAX 60
#define THRESHOLD 500

#define DEBUG 0
#define FLUSH_TIMEOUT 1

static signed short trame[2 * SAMPLES_PER_TRAME];
static signed short pulse[2 * SAMPLES_PER_TRAME];
static int ringpos = 0;
static int pos, f1, f2;
static int amplitude = 0;
static int lp = 0;

static void
read_sample (void)
{
  amplitude -= abs (trame[ringpos]);
  f1 -= pulse[ringpos];
  f1 += pulse[(ringpos + SAMPLES_PER_TRAME) % (2 * SAMPLES_PER_TRAME)];
  f2 -= pulse[(ringpos + SAMPLES_PER_TRAME) % (2 * SAMPLES_PER_TRAME)];
  fread (trame + ringpos, 1, sizeof (trame[0]), stdin);
  amplitude += abs (trame[ringpos]);

  if (pos ? (trame[ringpos] < -THRESHOLD)
      : (trame[ringpos] > +THRESHOLD))
    {
      pulse[ringpos] = 1;
      pos = !pos;
      f2++;
    }
  else
    pulse[ringpos] = 0;
  ringpos++;
  ringpos %= 2 * SAMPLES_PER_TRAME;
  lp++;
}

int
main ()
{
  int bitn = 7;
  char c = 0;
  int i;
  int llp = 0;
  while (!feof (stdin))
    {
      if (lp > 3 * SAMPLES_PER_TRAME)
	{
	  bitn = 7;
	  c = 0;
	  lp = 0;
	  llp++;
	}
      if (llp == FLUSH_TIMEOUT)
	fflush (stdout);
      if (f2 > FREQ_SEP_MIN && f2 < FREQ_SEP_MAX
	  && f1 > FREQ_DATA_MIN && f1 < FREQ_DATA_MAX)
	{
#if DEBUG
	  printf ("%d %d %d @%d\n", f1, f2, FREQ_DATA_THRESHOLD,
		  ftell (stdin) - sizeof (trame));
#endif
	  if (f1 < FREQ_DATA_THRESHOLD)
	    c |= (1 << bitn);
	  bitn--;
	  if (bitn < 0)
	    {
#if DEBUG
	      printf ("<%c, %x>", c, c);
#else
	      printf ("%c", c);
#endif
	      bitn = 7;
	      c = 0;
	    }
	  lp = 0;
	  llp = 0;
	  for (i = 0; i < SAMPLES_PER_TRAME; i++)
	    read_sample ();
	  continue;
	}
      read_sample ();
    }
  return 0;
}
