-- fsm.vhd: Finite State Machine
-- Author: Adrian Toth (xtotha01)

library ieee;
use ieee.std_logic_1164.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (
   					-- ROOT BRANCH
                    TEST1,                  -- checking first number => choose branch
                    -- BRANCH A
                    TEST_A2, TEST_A3, TEST_A4, TEST_A5, TEST_A6, TEST_A7, TEST_A8, TEST_A9, TEST_A10,
                    -- BRANCH B
                    TEST_B2, TEST_B3, TEST_B4, TEST_B5, TEST_B6, TEST_B7, TEST_B8, TEST_B9, TEST_B10,
                    -- END OF BRANCH
                    TEST_A_END,  			-- input code A is correct, waiting for #
                    TEST_B_END,				-- input code B is correct, waiting for #
                    -- OTHER
                    PRINT_MESSAGE,          -- correct input
                    PRINT_ERROR_MESSAGE,    -- wrong input
                    FAULT_STATE,            -- there was an error in past, waiting for #
                    FINISH                  -- end of states
                   );
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= TEST1;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;
-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is

-- ROOT BRANCH ##################################################################################

   -- TEST1 ==========================================================
   when TEST1 =>
      next_state <= TEST1;
      if (KEY(1) = '1') then
         next_state <= TEST_A2;
        elsif (KEY(4) = '1') then
            next_state <= TEST_B2;
            elsif (KEY(15) = '1') then
                   next_state <= PRINT_ERROR_MESSAGE;
               elsif (KEY(15 downto 0) /= "0000000000000000") then
                  next_state <= FAULT_STATE;
               end if;

-- A BRANCH #####################################################################################

   -- TEST_A2 ==========================================================
   when TEST_A2 =>
      next_state <= TEST_A2;
      if (KEY(0) = '1') then
         next_state <= TEST_A3;
         elsif (KEY(15) = '1') then
            next_state <= PRINT_ERROR_MESSAGE;
            elsif (KEY(15 downto 0) /= "0000000000000000") then
               next_state <= FAULT_STATE;
            end if;

   -- TEST_A3 ==========================================================
   when TEST_A3 =>
      next_state <= TEST_A3;
      if (KEY(0) = '1') then
         next_state <= TEST_A4;
         elsif (KEY(15) = '1') then
            next_state <= PRINT_ERROR_MESSAGE;
            elsif (KEY(15 downto 0) /= "0000000000000000") then
               next_state <= FAULT_STATE;
            end if;

   -- TEST_A4 ==========================================================
   when TEST_A4 =>
      next_state <= TEST_A4;
      if (KEY(4) = '1') then
         next_state <= TEST_A5;
         elsif (KEY(15) = '1') then
            next_state <= PRINT_ERROR_MESSAGE;
            elsif (KEY(15 downto 0) /= "0000000000000000") then
               next_state <= FAULT_STATE;
            end if;

   -- TEST_A5 ==========================================================
   when TEST_A5 =>
      next_state <= TEST_A5;
      if (KEY(8) = '1') then
         next_state <= TEST_A6;
         elsif (KEY(15) = '1') then
            next_state <= PRINT_ERROR_MESSAGE;
            elsif (KEY(15 downto 0) /= "0000000000000000") then
               next_state <= FAULT_STATE;
            end if;

   -- TEST_A6 ==========================================================
   when TEST_A6 =>
      next_state <= TEST_A6;
      if (KEY(3) = '1') then
         next_state <= TEST_A7;
         elsif (KEY(15) = '1') then
            next_state <= PRINT_ERROR_MESSAGE;
            elsif (KEY(15 downto 0) /= "0000000000000000") then
               next_state <= FAULT_STATE;
            end if;

   -- TEST_A7 ==========================================================
   when TEST_A7 =>
      next_state <= TEST_A7;
      if (KEY(4) = '1') then
         next_state <= TEST_A8;
         elsif (KEY(15) = '1') then
            next_state <= PRINT_ERROR_MESSAGE;
            elsif (KEY(15 downto 0) /= "0000000000000000") then
               next_state <= FAULT_STATE;
            end if;

   -- TEST_A8 ==========================================================
   when TEST_A8 =>
      next_state <= TEST_A8;
      if (KEY(0) = '1') then
         next_state <= TEST_A9;
         elsif (KEY(15) = '1') then
            next_state <= PRINT_ERROR_MESSAGE;
            elsif (KEY(15 downto 0) /= "0000000000000000") then
               next_state <= FAULT_STATE;
            end if;

   -- TEST_A9 ==========================================================
   when TEST_A9 =>
      next_state <= TEST_A9;
      if (KEY(5) = '1') then
         next_state <= TEST_A10;
         elsif (KEY(15) = '1') then
            next_state <= PRINT_ERROR_MESSAGE;
            elsif (KEY(15 downto 0) /= "0000000000000000") then
               next_state <= FAULT_STATE;
            end if;

   -- TEST_A10 =========================================================
   when TEST_A10 =>
      next_state <= TEST_A10;
      if (KEY(2) = '1') then
         next_state <= TEST_A_END;
         elsif (KEY(15) = '1') then
            next_state <= PRINT_ERROR_MESSAGE;
            elsif (KEY(15 downto 0) /= "0000000000000000") then
               next_state <= FAULT_STATE;
            end if;

   -- TEST_A_END =========================================================
    when TEST_A_END =>
      next_state <= TEST_A_END;
      if (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE;
        elsif (KEY(15 downto 0) /= "0000000000000000") then
           next_state <= FAULT_STATE;
        end if;

-- B BRANCH #####################################################################################

   -- TEST_B2 ==========================================================
   when TEST_B2 =>
      next_state <= TEST_B2;
      if (KEY(6) = '1') then
         next_state <= TEST_B3;
         elsif (KEY(15) = '1') then
            next_state <= PRINT_ERROR_MESSAGE;
            elsif (KEY(15 downto 0) /= "0000000000000000") then
               next_state <= FAULT_STATE;
            end if;

   -- TEST_B3 ==========================================================
   when TEST_B3 =>
      next_state <= TEST_B3;
      if (KEY(8) = '1') then
         next_state <= TEST_B4;
         elsif (KEY(15) = '1') then
            next_state <= PRINT_ERROR_MESSAGE;
            elsif (KEY(15 downto 0) /= "0000000000000000") then
               next_state <= FAULT_STATE;
            end if;

   -- TEST_B4 ==========================================================
   when TEST_B4 =>
      next_state <= TEST_B4;
      if (KEY(9) = '1') then
         next_state <= TEST_B5;
         elsif (KEY(15) = '1') then
            next_state <= PRINT_ERROR_MESSAGE;
            elsif (KEY(15 downto 0) /= "0000000000000000") then
               next_state <= FAULT_STATE;
            end if;

   -- TEST_B5 ==========================================================
   when TEST_B5 =>
      next_state <= TEST_B5;
      if (KEY(2) = '1') then
         next_state <= TEST_B6;
         elsif (KEY(15) = '1') then
            next_state <= PRINT_ERROR_MESSAGE;
            elsif (KEY(15 downto 0) /= "0000000000000000") then
               next_state <= FAULT_STATE;
            end if;

   -- TEST_B6 ==========================================================
   when TEST_B6 =>
      next_state <= TEST_B6;
      if (KEY(2) = '1') then
         next_state <= TEST_B7;
         elsif (KEY(15) = '1') then
            next_state <= PRINT_ERROR_MESSAGE;
            elsif (KEY(15 downto 0) /= "0000000000000000") then
               next_state <= FAULT_STATE;
            end if;

   -- TEST_B7 ==========================================================
   when TEST_B7 =>
      next_state <= TEST_B7;
      if (KEY(5) = '1') then
         next_state <= TEST_B8;
         elsif (KEY(15) = '1') then
            next_state <= PRINT_ERROR_MESSAGE;
            elsif (KEY(15 downto 0) /= "0000000000000000") then
               next_state <= FAULT_STATE;
            end if;

   -- TEST_B8 ==========================================================
   when TEST_B8 =>
      next_state <= TEST_B8;
      if (KEY(5) = '1') then
         next_state <= TEST_B9;
         elsif (KEY(15) = '1') then
            next_state <= PRINT_ERROR_MESSAGE;
            elsif (KEY(15 downto 0) /= "0000000000000000") then
               next_state <= FAULT_STATE;
            end if;

   -- TEST_B9 ==========================================================
   when TEST_B9 =>
      next_state <= TEST_B9;
      if (KEY(7) = '1') then
         next_state <= TEST_B10;
         elsif (KEY(15) = '1') then
            next_state <= PRINT_ERROR_MESSAGE;
            elsif (KEY(15 downto 0) /= "0000000000000000") then
               next_state <= FAULT_STATE;
            end if;

   -- TEST_B10 =========================================================
   when TEST_B10 =>
      next_state <= TEST_B10;
      if (KEY(8) = '1') then
         next_state <= TEST_B_END;
         elsif (KEY(15) = '1') then
            next_state <= PRINT_ERROR_MESSAGE;
            elsif (KEY(15 downto 0) /= "0000000000000000") then
               next_state <= FAULT_STATE;
            end if;

   -- TEST_B_END =========================================================
    when TEST_B_END =>
      next_state <= TEST_B_END;
      if (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE;
        elsif (KEY(15 downto 0) /= "0000000000000000") then
           next_state <= FAULT_STATE;
        end if;

-- END OF BRANCH ################################################################################

   -- FAULT_STATE ======================================================
   when FAULT_STATE =>
      next_state <= FAULT_STATE;
      if (KEY(15) = '1') then
         next_state <= PRINT_ERROR_MESSAGE;
        elsif (KEY(15 downto 0) /= "0000000000000000") then
           next_state <= FAULT_STATE;
        end if;

   -- PRINT_ERROR_MESSAGE ==============================================
   when PRINT_ERROR_MESSAGE =>
      next_state <= PRINT_ERROR_MESSAGE;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;

   -- PRINT_MESSAGE ====================================================
   when PRINT_MESSAGE =>
      next_state <= PRINT_MESSAGE;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;

   -- FINISH ===========================================================
   when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= TEST1;
      end if;

   -- OTHER ============================================================
   when others =>
      next_state <= TEST1;

-- ##############################################################################################

   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
     -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_ERROR_MESSAGE =>
      FSM_MX_MEM     <= '0';
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_MESSAGE =>
      FSM_MX_MEM     <= '1';
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      -- # pressed, need to clear the display
      if (KEY(15) = '1') then
         FSM_LCD_CLR <= '1';
      -- printing character *
      elsif (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR <= '1';
      end if;

   end case;
end process output_logic;

end architecture behavioral;
