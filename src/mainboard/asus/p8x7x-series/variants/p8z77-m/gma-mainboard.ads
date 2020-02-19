-- SPDX-License-Identifier: GPL-2.0-or-later

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is

   ports : constant Port_List :=
     (HDMI1,	-- DVI-D port
      HDMI3,	-- HDMI port
      Analog,	-- VGA port
      others => Disabled);

end GMA.Mainboard;
