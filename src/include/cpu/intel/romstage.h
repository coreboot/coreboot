#ifndef _CPU_INTEL_ROMSTAGE_H
#define _CPU_INTEL_ROMSTAGE_H

#include <arch/cpu.h>

void mainboard_romstage_entry(unsigned long bist);

/* fill_postcar_frame() is called after raminit completes and right before
 * calling run_postcar_phase(). Implementation should call postcar_frame_add_mtrr()
 * to tag memory ranges as cacheable to speed up execution of postcar and
 * early ramstage. */
void fill_postcar_frame(struct postcar_frame *pcf);

#endif /* _CPU_INTEL_ROMSTAGE_H */
