-- SPDX-License-Identifier: GPL-2.0-or-later

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is

   -- DDI C drives the HDMI port. The USB-C ports are left out, as
   -- the Wilco EC provides no way to wait for DP alt mode entry.
   ports : constant Port_List :=
     (eDP,
      DP2,
      others => Disabled);

end GMA.Mainboard;
