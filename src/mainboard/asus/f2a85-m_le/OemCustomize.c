#include "../f2a85-m/OemCustomize.c"
/*----------------------------------------------------------------------------------------
 *                        CUSTOMER OVERIDES MEMORY TABLE
 *----------------------------------------------------------------------------------------
 */

#if IS_ENABLED(CONFIG_BOARD_ASUS_F2A85_M_LE)
/*
 *  Platform Specific Overriding Table allows IBV/OEM to pass in platform information to AGESA
 *  (e.g. MemClk routing, the number of DIMM slots per channel,...). If PlatformSpecificTable
 *  is populated, AGESA will base its settings on the data from the table. Otherwise, it will
 *  use its default conservative settings.
 */
CONST PSO_ENTRY ROMDATA DefaultPlatformMemoryConfiguration[] = {

  NUMBER_OF_DIMMS_SUPPORTED (ANY_SOCKET, ANY_CHANNEL, 1),
  NUMBER_OF_CHANNELS_SUPPORTED (ANY_SOCKET, 2),

/*
  TODO: is this OK for DDR3 socket FM2?
  MEMCLK_DIS_MAP (ANY_SOCKET, ANY_CHANNEL, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),
  CKE_TRI_MAP (ANY_SOCKET, ANY_CHANNEL, 0x05, 0x0A),
  ODT_TRI_MAP (ANY_SOCKET, ANY_CHANNEL, 0x01, 0x02, 0x00, 0x00),
  CS_TRI_MAP (ANY_SOCKET, ANY_CHANNEL, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),
  */
  PSO_END
};
#endif /* CONFIG_BOARD_ASUS_F2A85M_LE */
