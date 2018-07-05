--
-- This file is part of the coreboot project.
--
-- Copyright (C) 2015 secunet Security Networks AG
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

with Interfaces.C;

use type Interfaces.C.int;

package body HW.Debug_Sink is

   function console_log_level
     (msg_level : Interfaces.C.int)
      return Interfaces.C.int;
   pragma Import (C, console_log_level, "console_log_level");

   Msg_Level_BIOS_DEBUG : constant := 7;

   procedure console_tx_byte (chr : Interfaces.C.char);
   pragma Import (C, console_tx_byte, "console_tx_byte");

   procedure Put (Item : String) is
   begin
      if console_log_level (Msg_Level_BIOS_DEBUG) /= 0 then
         for Idx in Item'Range loop
            console_tx_byte (Interfaces.C.To_C (Item (Idx)));
         end loop;
      end if;
   end Put;

   procedure Put_Char (Item : Character) is
   begin
      if console_log_level (Msg_Level_BIOS_DEBUG) /= 0 then
         console_tx_byte (Interfaces.C.To_C (Item));
      end if;
   end Put_Char;

   procedure New_Line is
   begin
      Put_Char (Character'Val (16#0a#));
   end New_Line;

end HW.Debug_Sink;
