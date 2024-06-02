-- SPDX-License-Identifier: GPL-2.0-or-later

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is

    ports : constant Port_List :=
    (DP2,     -- DP
     HDMI1,   -- DVI-I
     HDMI2,   -- DP
     HDMI3,   -- HDMI
     Analog,  -- DVI-I
     others => Disabled);

end GMA.Mainboard;
