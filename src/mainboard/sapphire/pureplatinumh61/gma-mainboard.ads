-- SPDX-License-Identifier: GPL-2.0-or-later

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is

   -- For a three-pipe setup, bandwidth is shared between the 2nd and
   -- the 3rd pipe. Thus, probe ports that likely have a high-resolution
   -- display attached first.

   -- FIXME: Only one of the digital ports is implemented as DP
   --        but it's unknown which.
   ports : constant Port_List :=
     (DP1,
      DP2,
      DP3,
      HDMI1,
      HDMI2,
      HDMI3,
      Analog,
      others => Disabled);

end GMA.Mainboard;
