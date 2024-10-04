-- SPDX-License-Identifier: GPL-2.0-or-later

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is

   ports : constant Port_List :=
     (DP1,      -- DP
      HDMI1,	-- DVI-D
      HDMI2,	-- DP
      HDMI3,	-- HDMI
      Analog,	-- D-SUB
      others => Disabled);

end GMA.Mainboard;
