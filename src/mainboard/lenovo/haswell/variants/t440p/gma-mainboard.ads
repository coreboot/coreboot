-- SPDX-License-Identifier: GPL-2.0-or-later

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is

   ports : constant Port_List :=
     (DP1, -- MiniDP
      DP2, -- dock, DP2-1 (DP/HDMI) and DP2-2 (DP/DVI)
      Analog,
      eDP,
      others => Disabled);

end GMA.Mainboard;
