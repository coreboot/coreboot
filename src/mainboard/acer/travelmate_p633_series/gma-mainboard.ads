-- SPDX-License-Identifier: GPL-2.0-or-later

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is

   ports : constant Port_List :=
     (Analog, -- laptop VGA (shared with ProDock via PI3V712)
      HDMI1,  -- laptop HDMI (shared with ProDock via PS8122)
      LVDS,   -- laptop display
      others => Disabled);

end GMA.Mainboard;
