/* Maximum size of the area that the FSP will search for the correct microcode */

#if IS_ENABLED(CONFIG_FSP_MODEL_406DX_A1)
	#define MICROCODE_REGION_LENGTH 0x14400
#elif IS_ENABLED(CONFIG_FSP_MODEL_406DX_B0)
	#define MICROCODE_REGION_LENGTH 0x14800
#endif
