--
-- This file is part of the coreboot project.
--
-- Copyright (C) 2018 Angel Pons <th3fanbus@gmail.com>
--
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; version 2 of the License.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is

   -- For a three-pipe setup, bandwidth is shared between the 2nd and
   -- the 3rd pipe. Thus, probe ports that likely have a high-resolution
   -- display attached first.

   ports : constant Port_List :=
     (HDMI3, -- mainboard HDMI port
      HDMI1, -- mainboard DVI-D port
      Analog,
      others => Disabled);

end GMA.Mainboard;
