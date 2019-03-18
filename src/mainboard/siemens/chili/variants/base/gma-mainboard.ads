-- SPDX-License-Identifier: GPL-2.0-or-later

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is

   ports : constant Port_List :=
     (DP1,	-- VGA (DVI-I)
      HDMI2,	-- DVI-I
      DP3,	-- DP++
      HDMI3,	-- DP++
      eDP,
      others => Disabled);

end GMA.Mainboard;
