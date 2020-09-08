-- SPDX-License-Identifier: GPL-2.0-or-later

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is

   ports : constant Port_List :=
     (DP1, -- DP1/HDMI1: DisplayPorts on board and dock
      HDMI1,
      DP2, -- DP2: VGA ports on board and dock
      eDP,
      others => Disabled);

end GMA.Mainboard;
