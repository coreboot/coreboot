#ifndef PART_FRAMEBUFFER_H
#define PART_FRAMEBUFFER_H

#ifdef HAVE_FRAMEBUFFER
void framebuffer_on(void)
#else
#  define framebuffer_on() do {} while(0)
#endif /* HAVE_FRAMEBUFFER */

#endif /* PART_FRAMEBUFFER_H */
