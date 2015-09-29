------------------------------------------------------------------------------
--                                                                          --
--                         GNAT COMPILER COMPONENTS                         --
--                                                                          --
--                         I N T E R F A C E S . C                          --
--                                                                          --
--                                 B o d y                                  --
--                                                                          --
--          Copyright (C) 1992-2009, Free Software Foundation, Inc.         --
--                                                                          --
-- GNAT is free software;  you can  redistribute it  and/or modify it under --
-- terms of the  GNU General Public License as published  by the Free Soft- --
-- ware  Foundation;  either version 3,  or (at your option) any later ver- --
-- sion.  GNAT is distributed in the hope that it will be useful, but WITH- --
-- OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY --
-- or FITNESS FOR A PARTICULAR PURPOSE.                                     --
--                                                                          --
-- As a special exception under Section 7 of GPL version 3, you are granted --
-- additional permissions described in the GCC Runtime Library Exception,   --
-- version 3.1, as published by the Free Software Foundation.               --
--                                                                          --
-- You should have received a copy of the GNU General Public License and    --
-- a copy of the GCC Runtime Library Exception along with this program;     --
-- see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see    --
-- <http://www.gnu.org/licenses/>.                                          --
--                                                                          --
-- GNAT was originally developed  by the GNAT team at  New York University. --
-- Extensive contributions were provided by Ada Core Technologies Inc.      --
--                                                                          --
------------------------------------------------------------------------------

package body Interfaces.C is

   -----------------------
   -- Is_Nul_Terminated --
   -----------------------

   --  Case of char_array

   function Is_Nul_Terminated (Item : char_array) return Boolean is
   begin
      for J in Item'Range loop
         if Item (J) = nul then
            return True;
         end if;
      end loop;

      return False;
   end Is_Nul_Terminated;

   ------------
   -- To_Ada --
   ------------

   --  Convert char to Character

   function To_Ada (Item : char) return Character is
   begin
      return Character'Val (char'Pos (Item));
   end To_Ada;

   ----------
   -- To_C --
   ----------

   --  Convert Character to char

   function To_C (Item : Character) return char is
   begin
      return char'Val (Character'Pos (Item));
   end To_C;

end Interfaces.C;
