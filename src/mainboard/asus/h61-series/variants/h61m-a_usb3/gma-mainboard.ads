-- SPDX-License-Identifier: GPL-2.0-or-later

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is

   -- For a three-pipe setup, bandwidth is shared between the 2nd and
   -- the 3rd pipe. Thus, probe ports that likely have a high-resolution
   -- display attached first.

   ports : constant Port_List :=
     (HDMI2, -- mainboard HDMI port
      HDMI1, -- mainboard DVI-D port
      Analog,
      others => Disabled);

end GMA.Mainboard;
