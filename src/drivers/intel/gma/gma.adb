-- SPDX-License-Identifier: GPL-2.0-only

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;

package body GMA is

   function read_edid
     (raw_edid :    out HW.GFX.EDID.Raw_EDID_Data;
      port     : in     Interfaces.C.int)
      return Interfaces.C.int
   is
      use type Interfaces.C.int;
      success : Boolean := true;
   begin
      if port not in Active_Port_Type'Pos (Active_Port_Type'First)
                     .. Active_Port_Type'Pos (Active_Port_Type'Last)
      then
         raw_edid := (others => 0);
         return -2;
      else
         if not HW.GFX.GMA.Is_Initialized then
            HW.GFX.GMA.Initialize (Success => success);
         end if;
         if success then
            HW.GFX.GMA.Display_Probing.Read_EDID
              (raw_edid, Active_Port_Type'Val (port), success);
         end if;
         if success then
            return 0;
         else
            return -1;
         end if;
      end if;
   end read_edid;

end GMA;
