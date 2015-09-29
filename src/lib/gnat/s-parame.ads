------------------------------------------------------------------------------
--                                                                          --
--                         GNAT COMPILER COMPONENTS                         --
--                                                                          --
--                    S Y S T E M . P A R A M E T E R S                     --
--                                                                          --
--                                 S p e c                                  --
--                                                                          --
--          Copyright (C) 1992-2014, Free Software Foundation, Inc.         --
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

--  Default version used when no target-specific version is provided

--  This package defines some system dependent parameters for GNAT. These
--  are values that are referenced by the runtime library and are therefore
--  relevant to the target machine.

--  The parameters whose value is defined in the spec are not generally
--  expected to be changed. If they are changed, it will be necessary to
--  recompile the run-time library.

--  The parameters which are defined by functions can be changed by modifying
--  the body of System.Parameters in file s-parame.adb. A change to this body
--  requires only rebinding and relinking of the application.

--  Note: do not introduce any pragma Inline statements into this unit, since
--  otherwise the relinking and rebinding capability would be deactivated.

pragma Compiler_Unit_Warning;

package System.Parameters is
   pragma Pure;

   ----------------------------------------------
   -- Characteristics of types in Interfaces.C --
   ----------------------------------------------

   long_bits : constant := Long_Integer'Size;
   --  Number of bits in type long and unsigned_long. The normal convention
   --  is that this is the same as type Long_Integer, but this may not be true
   --  of all targets.

   ptr_bits  : constant := Standard'Address_Size;
   --  Number of bits in Interfaces.C pointers, normally a standard address

end System.Parameters;
