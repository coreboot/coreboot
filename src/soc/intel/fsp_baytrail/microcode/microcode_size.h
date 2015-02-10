/* Maximum size of the area that the FSP will search for the correct microcode */
#if !IS_ENABLED(CONFIG_SOC_INTEL_FSP_BAYTRAIL_MD)
	#define MICROCODE_REGION_LENGTH 0x30000
#else
	#define MICROCODE_REGION_LENGTH 0x10000
#endif
