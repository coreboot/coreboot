-- SPDX-License-Identifier: GPL-2.0-or-later

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is
  -- Two onboard DP++, one header for optional DP, HDMI or VGA port
   ports : constant Port_List :=
     (DP1,
      DP2,
      DP3,
      HDMI1,
      HDMI2,
      HDMI3,
      others => Disabled);

end GMA.Mainboard;
