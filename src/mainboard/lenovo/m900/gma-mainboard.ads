-- SPDX-License-Identifier: GPL-2.0-or-later

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is

	ports : constant Port_List :=
		(DP1, -- mainboard VGA port
		 DP2, -- DP++ port 1
		 DP3, -- DP++ port 2
		 HDMI2, -- DP++ port 1
		 HDMI3, -- DP++ port 2
		 others => Disabled);

end GMA.Mainboard;
