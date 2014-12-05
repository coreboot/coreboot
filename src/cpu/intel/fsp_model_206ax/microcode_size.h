/* Maximum size of the area that the FSP will search for the correct microcode */

#if IS_ENABLED(CONFIG_CPU_INTEL_FSP_MODEL_306AX)
	#define MICROCODE_REGION_LENGTH 0xC000
#elif IS_ENABLED(CONFIG_CPU_INTEL_FSP_MODEL_206AX)
	#define MICROCODE_REGION_LENGTH 0x2800
#endif
