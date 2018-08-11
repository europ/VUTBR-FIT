-- IVH - Seminar VHDL
-- xtotha01
-- Adrian Toth
-- CLK SLOWLER
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity engen is
   generic (
     MAXVALUE : integer
   );
   port (
     CLK : in std_logic;
     ENABLE : in std_logic;
     EN  : out std_logic
   );
end entity;

architecture main of engen is
   signal cnt : integer range 0 to MAXVALUE := 0;
begin

   process (CLK)
   begin
      if (CLK'event) and (CLK = '1') then
         EN <= '0';
         if (ENABLE = '1') then
            if (cnt = MAXVALUE) then
               cnt <= 0;
               EN <= '1';
            else
               cnt <= cnt + 1;
            end if;
         end if;
      end if;
   end process;

end;
