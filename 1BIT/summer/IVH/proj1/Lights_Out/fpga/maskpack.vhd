-- IVH - Seminar VHDL
-- Projekt 1
-- xtotha01
-- Adrian Toth
library IEEE;
use IEEE.STD_LOGIC_1164.all;
---------------------------------------------------------------------------------
package maskpack is
    type mask_t is
        record
            top: std_logic;
            left: std_logic;
            right: std_logic;
            bottom: std_logic;
        end record;
    function getmask(x,y : natural; COLUMNS, ROWS : natural) return mask_t;
end maskpack;
---------------------------------------------------------------------------------
package body maskpack is
    function getmask(x,y : natural; COLUMNS, ROWS : natural) return mask_t is
        variable Mask:mask_t := (top => '1', left => '1', right => '1', bottom => '1');
        begin
            if (x = 0) then
                Mask.left := '0';
            end if;
            if (y = 0) then
                Mask.top := '0';
            end if;
            if (x = (COLUMNS-1)) then
                Mask.right := '0';
            end if;
            if (y = (ROWS-1)) then
                Mask.bottom := '0';
            end if;
            return Mask;
        end getmask;
end maskpack;