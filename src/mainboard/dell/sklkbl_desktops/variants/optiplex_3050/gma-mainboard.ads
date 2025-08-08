-- SPDX-License-Identifier: GPL-2.0-or-later

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is

   ports : constant Port_List :=
      (HDMI1,  -- External HDMI
       DP2,    -- External DP (native)
       HDMI2,  -- External DP (DP++)
       DP3,    -- Video I/O card: VGA (0PKGGG), DP (H64DC)
       HDMI3,  -- Video I/O card: VGA (0PKGGG), DP (H64DC)
       others => Disabled);

end GMA.Mainboard;
