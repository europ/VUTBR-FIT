-- IVH - Seminar VHDL
-- Final Projekt
-- xtotha01
-- Adrian Toth
-- co-authors (some codes are from): Ing. Zdenek Vasicek, Ph.D. (http://www.fit.vutbr.cz/~vasicek/)
-- 									 Ing. Vojtech Mrazek		(http://www.fit.vutbr.cz/~imrazek/)
--################################################################################################################

library ieee;

use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.vga_controller_cfg.all;
use work.maskpack.ALL;

architecture behav of tlv_pc_ifc is
	--############################################################################################################
	--===============================================
	-- USEFUL CONSTANTS REPRESENTING INDEX OF NEIGHBOURS
	constant IDX_TOP    : NATURAL := 0;
	constant IDX_LEFT   : NATURAL := 1;
	constant IDX_RIGHT  : NATURAL := 2;
	constant IDX_BOTTOM : NATURAL := 3;
	constant IDX_ENTER  : NATURAL := 4;

	--===============================================
	-- REGISTERS (nessessary for mapping cell neighbours)
	signal REGISTER_SELECT_REQ : std_logic_vector(99 downto 0) := "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"; 
	signal REGISTER_INVERT_REQ : std_logic_vector(99 downto 0) := "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

	--===============================================
	-- START STATE
	signal INIT_ACTIVE   : std_logic_vector(24 downto 0) := "0000000000000000000000000";
	signal INIT_SELECTED : std_logic_vector(24 downto 0) := "0000000000000000000000000";

	--===============================================
	-- 25 bit signals requiRED for depict the game (for cursor and for activity (on/off))
	signal TO_VGA_ACTIVE   : std_logic_vector(24 downto 0);
	signal TO_VGA_SELECTED : std_logic_vector(24 downto 0);

	--===============================================
	-- KEYS
	signal KEYBOARD_FITKIT : std_logic_vector(15 downto 0);
	signal BUTTON: std_logic_vector(4 downto 0);

	--===============================================
	-- RESET SIGNAL
	signal  MY_RESET : std_logic := '1';

	--===============================================
	-- FREQUENCY DIVIDER
	signal FREQ_DIV_5Hz  : std_logic;
	signal FREQ_DIV_1MHz : std_logic;

	--===============================================
	signal VGA_MODE   : std_logic_vector(60 downto 0);

	--===============================================
	-- COLOR SETTINGS

	-- ACTIVE / NOT ACTIVE (on/off)
	signal  COLOR 	  	  : std_logic_vector(8 downto 0);
	signal  CELL_light 	  : std_logic_vector(8 downto 0) := "100011000"; -- active cell - khaki
	signal  CELL_dark 	  : std_logic_vector(8 downto 0) := "000000000"; -- not active cell - white

	-- SELECTED (cursor)
	signal  SEL 	  : std_logic_vector(8 downto 0) := "111000000";
	signal  SEL_light : std_logic_vector(8 downto 0) := "111100000"; -- selected and active cell - light khaki
	signal  SEL_dark  : std_logic_vector(8 downto 0) := "001001001"; -- selected and not active cell - grey

	--===============================================
	-- MAP SPECIFIC COLORS TO COLOR VECTOR
	alias RED   is COLOR(8 downto 6);
	alias GREEN is COLOR(5 downto 3);
	alias BLUE  is COLOR(2 downto 0);

	--===============================================
	signal ROW 	  : std_logic_vector(11 downto 0);
	signal COLUMN : std_logic_vector(11 downto 0);

	--############################################################################################################
begin

	--=========================================================================
	gen_1mhz : entity work.engen generic map ( MAXVALUE => 39)    port map ( CLK => CLK, ENABLE => '1',           EN => FREQ_DIV_1MHz );
	gen_5hz  : entity work.engen generic map ( MAXVALUE => 62500) port map ( CLK => CLK, ENABLE => FREQ_DIV_1MHz, EN => FREQ_DIV_5Hz  );

	--=========================================================================
	-- KEYBOARD PORTING
	kbrd_ctrl: entity work.keyboard_controller(arch_keyboard)
	port map (
		CLK => CLK,
		RST => RESET,

		KB_KIN   => KIN,
		KB_KOUT  => KOUT,

		DATA_OUT => KEYBOARD_FITKIT(15 downto 0)
	);

	--=========================================================================
	-- CELL GENERATING
	riadky: for y in 0 to 4 generate
	begin
		stlpce:  for x in 0 to 4 generate
		begin

   			CELL_inst: entity work.cell
   			generic map (MASK => getmask(x,y,5,5))

   			port map (
				SELECTED => TO_VGA_SELECTED(y*5+x),
				ACTIVE 	 => TO_VGA_ACTIVE(y*5+x),

				INIT_ACTIVE   => INIT_ACTIVE(y*5+x),
				INIT_SELECTED => INIT_SELECTED(y*5+x),

				SELECT_REQ_OUT => REGISTER_SELECT_REQ( ((y*5+x)*4+3) downto ((y*5+x)*4+0) ),
				INVERT_REQ_OUT => REGISTER_INVERT_REQ( ((y*5+x)*4+3) downto ((y*5+x)*4+0) ),

				SELECT_REQ_IN(IDX_TOP)    => REGISTER_SELECT_REQ(  (((y-1) mod 5)*5 + x)*4 + IDX_BOTTOM  ),
				SELECT_REQ_IN(IDX_RIGHT)  => REGISTER_SELECT_REQ(  (y*5 + ((x+1) mod 5))*4 + IDX_LEFT    ),
				SELECT_REQ_IN(IDX_LEFT)   => REGISTER_SELECT_REQ(  (y*5 + ((x-1) mod 5))*4 + IDX_RIGHT   ),
				SELECT_REQ_IN(IDX_BOTTOM) => REGISTER_SELECT_REQ(  (((y+1) mod 5)*5 + x)*4 + IDX_TOP     ),

				INVERT_REQ_IN(IDX_TOP)    => REGISTER_INVERT_REQ(  (((y-1) mod 5)*5 + x)*4 + IDX_BOTTOM  ),
				INVERT_REQ_IN(IDX_RIGHT)  => REGISTER_INVERT_REQ(  (y*5 + ((x+1) mod 5))*4 + IDX_LEFT    ),
				INVERT_REQ_IN(IDX_LEFT)   => REGISTER_INVERT_REQ(  (y*5 + ((x-1) mod 5))*4 + IDX_RIGHT   ),
				INVERT_REQ_IN(IDX_BOTTOM) => REGISTER_INVERT_REQ(  (((y+1) mod 5)*5 + x)*4 + IDX_TOP     ),

				CLK   => CLK,
				RESET => MY_RESET,

				KEYS  => BUTTON(4 downto 0)
			);

		end generate stlpce;
	end generate riadky;

	--=========================================================================
	-- SET UP GRAPHIC MODE (640x480, 60 Hz refresh)
	setmode(r640x480x60, VGA_MODE);

	vga: entity work.vga_controller(arch_vga_controller)
		generic map (REQ_DELAY => 1)
		port map (
			CLK    		=> CLK,
			RST    		=> RESET,
			ENABLE 		=> '1',
			MODE   		=> VGA_MODE,

			DATA_RED    => RED,
			DATA_GREEN  => GREEN,
			DATA_BLUE   => BLUE,
			ADDR_COLUMN => COLUMN,
			ADDR_ROW    => ROW,

			VGA_RED     => RED_V,
			VGA_BLUE    => BLUE_V,
			VGA_GREEN 	=> GREEN_V,
			VGA_HSYNC 	=> HSYNC_V,
			VGA_VSYNC 	=> VSYNC_V
		);

	--=========================================================================
	-- KEY PRESS PROCESS
	klavesy: process(CLK)
	begin
		if rising_edge(CLK) then
			BUTTON <= "00000";
			if (FREQ_DIV_5Hz='1') then

				-- KEY 2 (DOWN)
				if KEYBOARD_FITKIT(4)='1' then
					MY_RESET <= '0';
					BUTTON(IDX_TOP)<='1';

				-- KEY 4 (LEFT)
				elsif KEYBOARD_FITKIT(9)='1' then
					BUTTON(IDX_RIGHT)<='1';
					MY_RESET <= '0';

				-- KEY 6 (RIGHT)
				elsif KEYBOARD_FITKIT(1)='1' then
					MY_RESET <= '0';
					BUTTON(IDX_LEFT)<='1';

				-- KEY 8 (UP)
				elsif KEYBOARD_FITKIT(6)='1' then
					MY_RESET <= '0';
					BUTTON(IDX_BOTTOM)<='1';

				-- KEY 5 (ENTER)
				elsif KEYBOARD_FITKIT(5)='1' then
					MY_RESET <= '0';
					BUTTON(IDX_ENTER) <= '1';

				-- KEY A
				elsif KEYBOARD_FITKIT(12)='1' then
					INIT_ACTIVE   <= "1010101010101010101010101";
					INIT_SELECTED <= "0000000000001000000000000";
					MY_RESET <= '1';

				-- KEY B
				elsif KEYBOARD_FITKIT(13)='1' then
					INIT_ACTIVE   <= "0000001110010100111000000";
					INIT_SELECTED <= "0000000000001000000000000";
					MY_RESET <= '1';

				-- KEY C
				elsif KEYBOARD_FITKIT(14)='1' then
					INIT_ACTIVE   <= "0010001010100010101000100";
					INIT_SELECTED <= "0000000000001000000000000";
					MY_RESET <= '1';

				-- KEY D
				elsif KEYBOARD_FITKIT(15)='1' then
					INIT_ACTIVE   <= "1101110001001001000111011";
					INIT_SELECTED <= "0000000000001000000000000";
					MY_RESET <= '1';
				end if;
			end if;
		end if;
	end process;

	--=========================================================================
	-- GRAPHIC PROCESS
	process (CLK)
	begin
		--RESET_CELL <= '0';
		if (CLK'event) and (CLK='1') then

			-- == 1 ==
			if (ROW(11 downto 6) = "000000001") and (COLUMN(11 downto 6) = "000000010") then
				if (TO_VGA_ACTIVE(0) = '1' and TO_VGA_SELECTED(0) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(0) = '0' and TO_VGA_SELECTED(0) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(0) = '1' and TO_VGA_SELECTED(0) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 2 ==
			elsif (ROW(11 downto 6) = "000000001") and (COLUMN(11 downto 6) = "000000011") then
				if (TO_VGA_ACTIVE(1) = '1' and TO_VGA_SELECTED(1) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(1) = '0' and TO_VGA_SELECTED(1) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(1) = '1' and TO_VGA_SELECTED(1) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 3 ==
			elsif (ROW(11 downto 6) = "000000001") and (COLUMN(11 downto 6) = "000000100") then
				 if (TO_VGA_ACTIVE(2) = '1' and TO_VGA_SELECTED(2) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(2) = '0' and TO_VGA_SELECTED(2) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(2) = '1' and TO_VGA_SELECTED(2) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 4 ==
			elsif (ROW(11 downto 6) = "000000001") and (COLUMN(11 downto 6) = "000000101") then
				if (TO_VGA_ACTIVE(3) = '1' and TO_VGA_SELECTED(3) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(3) = '0' and TO_VGA_SELECTED(3) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(3) = '1' and TO_VGA_SELECTED(3) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 5 ==
			elsif (ROW(11 downto 6) = "000000001") and (COLUMN(11 downto 6) = "000000110") then
				if (TO_VGA_ACTIVE(4) = '1' and TO_VGA_SELECTED(4) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(4) = '0' and TO_VGA_SELECTED(4) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(4) = '1' and TO_VGA_SELECTED(4) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 6 ==
			elsif (ROW(11 downto 6) = "000000010") and (COLUMN(11 downto 6) = "000000010") then
				if (TO_VGA_ACTIVE(5) = '1' and TO_VGA_SELECTED(5) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(5) = '0' and TO_VGA_SELECTED(5) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(5) = '1' and TO_VGA_SELECTED(5) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 7 ==
			elsif (ROW(11 downto 6) = "000000010") and (COLUMN(11 downto 6) = "000000011") then
				if (TO_VGA_ACTIVE(6) = '1' and TO_VGA_SELECTED(6) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(6) = '0' and TO_VGA_SELECTED(6) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(6) = '1' and TO_VGA_SELECTED(6) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 8 ==
			elsif (ROW(11 downto 6) = "000000010") and (COLUMN(11 downto 6) = "000000100") then
				if (TO_VGA_ACTIVE(7) = '1' and TO_VGA_SELECTED(7) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(7) = '0' and TO_VGA_SELECTED(7) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(7) = '1' and TO_VGA_SELECTED(7) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 9 ==
			elsif (ROW(11 downto 6) = "000000010") and (COLUMN(11 downto 6) = "000000101") then
				if (TO_VGA_ACTIVE(8) = '1' and TO_VGA_SELECTED(8) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(8) = '0' and TO_VGA_SELECTED(8) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(8) = '1' and TO_VGA_SELECTED(8) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 10 ==
			elsif (ROW(11 downto 6) = "000000010") and (COLUMN(11 downto 6) = "000000110") then
				if (TO_VGA_ACTIVE(9) = '1' and TO_VGA_SELECTED(9) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(9) = '0' and TO_VGA_SELECTED(9) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(9) = '1' and TO_VGA_SELECTED(9) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 11 ==
			elsif (ROW(11 downto 6) = "000000011") and (COLUMN(11 downto 6) = "000000010") then
				if (TO_VGA_ACTIVE(10) = '1' and TO_VGA_SELECTED(10) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(10) = '0' and TO_VGA_SELECTED(10) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(10) = '1' and TO_VGA_SELECTED(10) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 12 ==
			elsif (ROW(11 downto 6) = "000000011") and (COLUMN(11 downto 6) = "000000011") then
				if (TO_VGA_ACTIVE(11) = '1' and TO_VGA_SELECTED(11) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(11) = '0' and TO_VGA_SELECTED(11) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(11) = '1' and TO_VGA_SELECTED(11) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 13 ==
			elsif (ROW(11 downto 6) = "000000011") and (COLUMN(11 downto 6) = "000000100") then
				if (TO_VGA_ACTIVE(12) = '1' and TO_VGA_SELECTED(12) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(12) = '0' and TO_VGA_SELECTED(12) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(12) = '1' and TO_VGA_SELECTED(12) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 14 ==
			elsif (ROW(11 downto 6) = "000000011") and (COLUMN(11 downto 6) = "000000101") then
			  if (TO_VGA_ACTIVE(13) = '1' and TO_VGA_SELECTED(13) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(13) = '0' and TO_VGA_SELECTED(13) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(13) = '1' and TO_VGA_SELECTED(13) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 15 ==
			elsif (ROW(11 downto 6) = "000000011") and (COLUMN(11 downto 6) = "000000110") then
				if (TO_VGA_ACTIVE(14) = '1' and TO_VGA_SELECTED(14) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(14) = '0' and TO_VGA_SELECTED(14) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(14) = '1' and TO_VGA_SELECTED(14) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 16 ==
			elsif (ROW(11 downto 6) = "000000100") and (COLUMN(11 downto 6) = "000000010") then
				if (TO_VGA_ACTIVE(15) = '1' and TO_VGA_SELECTED(15) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(15) = '0' and TO_VGA_SELECTED(15) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(15) = '1' and TO_VGA_SELECTED(15) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 17 ==
			elsif (ROW(11 downto 6) = "000000100") and (COLUMN(11 downto 6) = "000000011") then
				if (TO_VGA_ACTIVE(16) = '1' and TO_VGA_SELECTED(16) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(16) = '0' and TO_VGA_SELECTED(16) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(16) = '1' and TO_VGA_SELECTED(16) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 18 ==
			elsif (ROW(11 downto 6) = "000000100") and (COLUMN(11 downto 6) = "000000100") then
				if (TO_VGA_ACTIVE(17) = '1' and TO_VGA_SELECTED(17) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(17) = '0' and TO_VGA_SELECTED(17) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(17) = '1' and TO_VGA_SELECTED(17) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 19 ==
			elsif (ROW(11 downto 6) = "000000100") and (COLUMN(11 downto 6) = "000000101") then
				if (TO_VGA_ACTIVE(18) = '1' and TO_VGA_SELECTED(18) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(18) = '0' and TO_VGA_SELECTED(18) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(18) = '1' and TO_VGA_SELECTED(18) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 20 ==
			elsif (ROW(11 downto 6) = "000000100") and (COLUMN(11 downto 6) = "000000110") then
				if (TO_VGA_ACTIVE(19) = '1' and TO_VGA_SELECTED(19) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(19) = '0' and TO_VGA_SELECTED(19) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(19) = '1' and TO_VGA_SELECTED(19) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 21 ==
			elsif (ROW(11 downto 6) = "000000101") and (COLUMN(11 downto 6) = "000000010") then
				if (TO_VGA_ACTIVE(20) = '1' and TO_VGA_SELECTED(20) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(20) = '0' and TO_VGA_SELECTED(20) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(20) = '1' and TO_VGA_SELECTED(20) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 22 ==
			elsif (ROW(11 downto 6) = "000000101") and (COLUMN(11 downto 6) = "000000011") then
				if (TO_VGA_ACTIVE(21) = '1' and TO_VGA_SELECTED(21) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(21) = '0' and TO_VGA_SELECTED(21) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(21) = '1' and TO_VGA_SELECTED(21) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 23 ==
			elsif (ROW(11 downto 6) = "000000101") and (COLUMN(11 downto 6) = "000000100") then
				if (TO_VGA_ACTIVE(22) = '1' and TO_VGA_SELECTED(22) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(22) = '0' and TO_VGA_SELECTED(22) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(22) = '1' and TO_VGA_SELECTED(22) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 24 ==
			elsif (ROW(11 downto 6) = "000000101") and (COLUMN(11 downto 6) = "000000101") then
				if (TO_VGA_ACTIVE(23) = '1' and TO_VGA_SELECTED(23) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(23) = '0' and TO_VGA_SELECTED(23) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(23) = '1' and TO_VGA_SELECTED(23) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			-- == 25 ==
			elsif (ROW(11 downto 6) = "000000101") and (COLUMN(11 downto 6) = "000000110") then
				if (TO_VGA_ACTIVE(24) = '1' and TO_VGA_SELECTED(24) = '1') then
					COLOR <= SEL_light;
				elsif (TO_VGA_ACTIVE(24) = '0' and TO_VGA_SELECTED(24) = '1') then
					COLOR <= SEL_dark;
				elsif (TO_VGA_ACTIVE(24) = '1' and TO_VGA_SELECTED(24) = '0') then
					COLOR <= CELL_light;
				else
					COLOR <= CELL_dark;
				end if;

			else
				COLOR <=  "000000000";

			end if;

			-- LINES BETWEEN SQUARES
			if (ROW=128 or ROW=192 or ROW=254 or ROW=320 or COLUMN=192 or COLUMN=255 or COLUMN=320 or COLUMN=384) then
				COLOR <= "000000000";
			end if;

		end if;
	end process;
	--=========================================================================
end behav;

