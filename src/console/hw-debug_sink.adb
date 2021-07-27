-- SPDX-License-Identifier: GPL-2.0-only

with Interfaces.C;
with CB.Config;

use CB;
use type Interfaces.C.int;

package body HW.Debug_Sink is

   function console_log_level
     (msg_level : Interfaces.C.int)
      return Interfaces.C.int;
   pragma Import (C, console_log_level, "console_log_level");

   Msg_Level_BIOS_DEBUG : constant := 7;

   CONSOLE_LOG_FAST  : constant := 1;
   CONSOLE_LOG_ALL   : constant := 2;

   procedure cbmemc_tx_byte (chr : Interfaces.C.char);
   pragma Import (C, cbmemc_tx_byte, "cbmemc_tx_byte");

   procedure console_tx_byte (chr : Interfaces.C.char);
   pragma Import (C, console_tx_byte, "console_tx_byte");

   procedure Put (Item : String)
   is
      console_log : constant Interfaces.C.int :=
         console_log_level (Msg_Level_BIOS_DEBUG);
   begin
      if console_log = CONSOLE_LOG_FAST then
         if Config.CONSOLE_CBMEM then
            for Idx in Item'Range loop
               cbmemc_tx_byte (Interfaces.C.To_C (Item (Idx)));
            end loop;
         end if;
      elsif console_log = CONSOLE_LOG_ALL then
         for Idx in Item'Range loop
            console_tx_byte (Interfaces.C.To_C (Item (Idx)));
         end loop;
      end if;
   end Put;

   procedure Put_Char (Item : Character)
   is
      console_log : constant Interfaces.C.int :=
         console_log_level (Msg_Level_BIOS_DEBUG);
   begin
      if console_log = CONSOLE_LOG_FAST then
         if Config.CONSOLE_CBMEM then
            cbmemc_tx_byte (Interfaces.C.To_C (Item));
         end if;
      elsif console_log = CONSOLE_LOG_ALL then
         console_tx_byte (Interfaces.C.To_C (Item));
      end if;
   end Put_Char;

   procedure New_Line is
   begin
      Put_Char (Character'Val (16#0a#));
   end New_Line;

end HW.Debug_Sink;
