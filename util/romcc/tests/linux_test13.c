#include "linux_syscall.h"
#include "linux_console.h"

struct mem_controller {
	unsigned short channel0[4];
};

static unsigned int spd_detect_dimms(const struct mem_controller *ctrl)
{
        unsigned dimm_mask;
        int i;

	print_debug("1\n");
        dimm_mask = 0;
        for(i = 0; i < 4; i++) {
                int byte;
                unsigned device;

		print_debug("2\n");
                device = ctrl->channel0[i];
                if (device) {
			print_debug("3\n");
                        byte = ctrl->channel0[i] + 2;
                        if (byte == 7) {
                                dimm_mask |= (1 << i);
                        }
                }
		print_debug("4\n");
        }
	print_debug("5\n");
        return dimm_mask;
}


static void main(void)
{
        static const struct mem_controller cpu[] = {
                {
                        .channel0 = { (0xa<<3)|0, (0xa<<3)|2, 0, 0 },
                },
        };
	long dimm_mask;
	print_debug("A\n");
	dimm_mask = spd_detect_dimms(cpu);
	print_debug("B\n");
	_exit(0);
}
