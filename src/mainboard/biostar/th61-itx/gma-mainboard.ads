-- SPDX-License-Identifier: GPL-2.0-or-later

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is
   -- TODO: verify this with a CPU that has an IGP
   ports : constant Port_List :=
     (HDMI1, -- DVI
      HDMI2, -- HDMI
      HDMI3,
      Analog,
      others => Disabled);

end GMA.Mainboard;
