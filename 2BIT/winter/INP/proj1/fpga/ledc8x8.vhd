--###################################################################
-- Adrian Toth
-- INP - proj1
-- 3.10.2016

--###################################################################
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

--###################################################################
entity ledc8x8 is
    port (
        RESET : in  std_logic;
        SMCLK : in  std_logic;
        ROW   : out std_logic_vector (7 downto 0) := "00000000";
        LED   : out std_logic_vector (7 downto 0) := "00000000"
    );
end ledc8x8;

--###################################################################
architecture main of ledc8x8 is

    signal switch_row    :std_logic := '0';
    signal char_type     :std_logic := '0';
    signal row_counter   :std_logic_vector (2 downto 0)  := "000";
    signal clk_slow      :std_logic_vector (7 downto 0)  := "00000000";
    signal row_my        :std_logic_vector (7 downto 0)  := "00000000";
    signal led_my        :std_logic_vector (7 downto 0)  := "00000000";
    signal char_changer  :std_logic_vector (21 downto 0) := "0000000000000000000000";

begin
    process (RESET, SMCLK) begin
        if RESET = '1' then
            row_my <= "00000000";
            led_my <= "00000000";
            row_counter <= "000";


        elsif rising_edge(SMCLK) then

            -- character changer -----------------------------------------
            char_changer <= char_changer + 1;
            if char_changer(21 downto 0) = "1110000100000000000000" then
                char_type <= not char_type;
                char_changer <= "0000000000000000000000";
            end if;

            -- row switcher ----------------------------------------------
            clk_slow <= clk_slow + 1;
            if clk_slow(7 downto 0) = "11111111" then
                switch_row <= '1';
            else
                switch_row <= '0';
            end if;

            -- row shifter -----------------------------------------------
            if switch_row = '1' then
                case row_counter is
                    when "000" => row_my <= "00000001";
                    when "001" => row_my <= "00000010";
                    when "010" => row_my <= "00000100";
                    when "011" => row_my <= "00001000";
                    when "100" => row_my <= "00010000";
                    when "101" => row_my <= "00100000";
                    when "110" => row_my <= "01000000";
                    when "111" => row_my <= "10000000";
                    when others =>
                end case;
                row_counter <= row_counter+1;
            end if;

            -- character display -----------------------------------------
            if char_type = '1' then

            	-- char T ------------------------------------------------
                case row_my is
                    when "00000001" => led_my <= "10000000";
                    when "00000010" => led_my <= "11110111";
                    when "00000100" => led_my <= "11110111";
                    when "00001000" => led_my <= "11110111";
                    when "00010000" => led_my <= "11110111";
                    when "00100000" => led_my <= "11110111";
                    when "01000000" => led_my <= "11110111";
                    when "10000000" => led_my <= "11111111";
                    when others =>
                end case;

            elsif char_type = '0' then

            	-- char A ------------------------------------------------
                case row_my is
                    when "00000001" => led_my <= "11110111";
                    when "00000010" => led_my <= "11101011";
                    when "00000100" => led_my <= "11011101";
                    when "00001000" => led_my <= "10000000";
                    when "00010000" => led_my <= "10111110";
                    when "00100000" => led_my <= "10111110";
                    when "01000000" => led_my <= "10111110";
                    when "10000000" => led_my <= "11111111";
                    when others =>
                end case;

            end if;
        end if ;
    end process;

    ROW <= row_my;
    LED <= led_my;

end main;
