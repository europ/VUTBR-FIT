-- cpu.vhd: Simple 8-bit CPU (BrainLove interpreter)
-- Copyright (C) 2016 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): xtotha01 (Adrian Toth)

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru

   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti

   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (1) / zapis (0)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti

   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data

   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

    -- PC
    signal pc_register       : std_logic_vector(11 downto 0);
    signal pc_register_inc   : std_logic;
    signal pc_register_dec   : std_logic;

    -- CNT
    signal cnt_register      : std_logic_vector(7 downto 0);
    signal cnt_register_inc  : std_logic;
    signal cnt_register_dec  : std_logic;

    -- TMP
    signal tmp_register      : std_logic_vector(7 downto 0);
    signal tmp_register_load : std_logic;

    -- PTR
    signal ptr_register      : std_logic_vector(9 downto 0);
    signal ptr_register_inc  : std_logic;
    signal ptr_register_dec  : std_logic;

    -- MX
    signal mx_memory         : std_logic_vector(7 downto 0);

    -- FSM
    signal FSM_sel           : std_logic_vector(1 downto 0);
    type state_type is (
    					init,

    					fetch, decode,

    					inc_ptr, dec_ptr, -- > <

    					inc_data_1, dec_data_1, -- + -
    					inc_data_2, dec_data_2,

    					begin_loop_1,  end_loop_1, -- [ ]
    					begin_loop_2,  end_loop_2,
    					begin_loop_3,  end_loop_3,
    					begin_loop_4,  end_loop_4,
    								   end_loop_5,

    					put_data_1, get_data_1, -- . ,
    					put_data_2, get_data_2,

    					data_to_tmp_1, tmp_to_data_1, -- $ !
    					data_to_tmp_2,

    					halt, -- null

    					other

				    );
    signal present_state : state_type;
    signal next_state    : state_type;

begin


-- PC: PROGRAM COUNTER
PC_program_counter: process (RESET, CLK, pc_register_inc, pc_register_dec, pc_register)
begin
	if (RESET = '1') then
		pc_register <= (others => '0');
	elsif (rising_edge(CLK)) then
		if (pc_register_inc = '1') then
			pc_register <= pc_register + 1;
		elsif (pc_register_dec = '1') then
			pc_register <= pc_register - 1;
		end if;
	end if;
end process;
CODE_ADDR <= pc_register;


-- CNT: counter
CNT_counter: process (RESET, CLK, cnt_register_inc, cnt_register_dec, cnt_register)
begin
	if (RESET = '1') then
		cnt_register <= (others => '0');
	elsif (rising_edge(CLK)) then
		if (cnt_register_inc = '1') then
			cnt_register <= cnt_register + 1;
		elsif (cnt_register_dec = '1') then
			cnt_register <= cnt_register - 1;
		end if;
	end if;
end process;


-- TMP: temporary
TMP_remporary: process(RESET, CLK, tmp_register_load, tmp_register)
begin
	if (RESET = '1') then
		tmp_register <= (others => '0');
	elsif (rising_edge(CLK)) then
		if (tmp_register_load = '1') then
			tmp_register <= DATA_RDATA;
		end if;
	end if;
end process;


-- PTR: pointer
PTR_pointer: process(RESET, CLK, ptr_register_inc, ptr_register_dec, ptr_register)
begin
	if (RESET = '1') then
		ptr_register <= (others => '0');
	elsif (rising_edge(CLK)) then
		if (ptr_register_inc = '1') then
			ptr_register <= ptr_register + 1;
		elsif (ptr_register_dec = '1') then
			ptr_register <= ptr_register - 1;
		end if;
	end if;
end process;
DATA_ADDR <= ptr_register;


-- MX: multiplexor
MX_multiplexor: process(FSM_sel, DATA_RDATA, IN_DATA, tmp_register, mx_memory)
begin
	mx_memory <= DATA_RDATA;
	case(FSM_sel) is
		when "00" => DATA_WDATA <= IN_DATA;
		when "01" => DATA_WDATA <= tmp_register;
		when "10" => DATA_WDATA <= (mx_memory - 1);
		when "11" => DATA_WDATA <= (mx_memory + 1);
		when others => null;
	end case;
end process;


-- FSM: finite-state machine
-- PRESENT STATE
FSM_finite_state_machine_PresentState: process(RESET, CLK, EN, present_state, next_state)
begin
	if (RESET = '1') then
		present_state <= init;
	elsif (rising_edge(CLK)) then
		if (EN = '1') then
			present_state <= next_state;
		end if;
	end if;
end process;


-- FSM: finite-state machine
-- NEXT STATE
FSM_finite_state_machine_NextState: process(present_state, next_state, DATA_RDATA, CODE_DATA, cnt_register, OUT_BUSY, IN_VLD)
begin
	-- nastavime FSM do init pri spusteni
	next_state <= init;

	-- vsetky vystupne signaly FSM inicializujeme do '0'
	pc_register_inc   <= '0';
	pc_register_dec   <= '0';

	cnt_register_inc  <= '0';
	cnt_register_dec  <= '0';

	ptr_register_inc  <= '0';
	ptr_register_dec  <= '0';

	tmp_register_load <= '0';

	FSM_sel <= "00";

	CODE_EN   <= '0';

	DATA_RDWR <= '0';
	DATA_EN   <= '0';

	IN_REQ    <= '0';
	OUT_WE    <= '0';

	OUT_DATA <= DATA_RDATA;

	-- FSM
	case present_state is

		when init =>
			next_state <= fetch;

		when fetch =>
			CODE_EN <= '1';
			next_state <= decode;

		when decode =>
			case(CODE_DATA) is
				when X"3E"  => next_state <= inc_ptr;       -- >
				when X"3C"  => next_state <= dec_ptr;       -- <
				when X"2B"  => next_state <= inc_data_1;    -- +
				when X"2D"  => next_state <= dec_data_1;    -- -
				when X"5B"  => next_state <= begin_loop_1;  -- [
				when X"5D"  => next_state <= end_loop_1;    -- ]
				when X"2E"  => next_state <= put_data_1;    -- .
				when X"2C"  => next_state <= get_data_1;    -- ,
				when X"24"  => next_state <= data_to_tmp_1; -- $
				when X"21"  => next_state <= tmp_to_data_1; -- !
				when X"00"  => next_state <= halt;          -- null
				when others => next_state <= other;         -- OTHER
			end case;

--------------------------------------------------------
		when inc_ptr => -- >
			-- PTR <= PTR + 1
			-- PC <= PC + 1
			ptr_register_inc <= '1';
			pc_register_inc  <= '1';
			next_state <= init;

--------------------------------------------------------
		when dec_ptr => -- <
			-- PTR <= PTR - 1
			-- PC <= PC + 1
			ptr_register_dec <= '1';
			pc_register_inc  <= '1';
			next_state <= init;

--------------------------------------------------------
		when inc_data_1 => -- +
		    -- DATA_RDATA <= ram[PTR]
			DATA_EN   <= '1';
			DATA_RDWR <= '1';
			next_state <= inc_data_2;

		when inc_data_2 =>
			-- ram[PTR] <= DATA_RDATA + 1
			-- PC <= PC + 1
			FSM_sel <= "11";
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			pc_register_inc <= '1';
			next_state <= init;

--------------------------------------------------------
		when dec_data_1 => -- -
			-- DATA_RDATA <= ram[PTR]
			DATA_EN   <= '1';
			DATA_RDWR <= '1';
			next_state <= dec_data_2;

		when dec_data_2 =>
			-- ram[PTR] <= DATA_RDATA - 1
			-- PC <= PC + 1
			FSM_sel <= "10";
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			pc_register_inc <= '1';
			next_state <= init;

--------------------------------------------------------
		when begin_loop_1 => -- [
			-- PC <= PC + 1
			pc_register_inc <= '1';
			DATA_EN <= '1';
			DATA_RDWR <= '1';
			next_state <= begin_loop_2;

		when begin_loop_2 =>
			-- if (ram[PTR] == 0)
			if (DATA_RDATA = "00000000") then
				-- CNT <= 1
				cnt_register_inc <= '1';
				next_state <= begin_loop_3;
			else
				next_state <= init;
			end if;

		when begin_loop_3 =>
			-- while (CNT != 0)
			if (cnt_register = "00000000") then
				next_state <= init;
			else
				-- c <= rom[PC]
				CODE_EN <= '1';
				next_state <= begin_loop_4;
			end if;

		when begin_loop_4 =>
			-- if (c=='[') CNT <= CNT + 1
			-- elsif (c==']') CNT <= CNT - 1
			if (CODE_DATA = X"5B") then -- [
				cnt_register_inc <= '1';
			elsif (CODE_DATA = X"5D") then -- ]
			    cnt_register_dec <= '1';
			end if;
			pc_register_inc <= '1';
			next_state <= begin_loop_3;

--------------------------------------------------------
		when end_loop_1 => -- ]
			DATA_EN <= '1';
			DATA_RDWR <= '1';
			next_state <= end_loop_2;

		when end_loop_2 =>
			-- if (ram[PTR] == 0)
			if (DATA_RDATA = "00000000") then
				-- PC <= PC +1
				pc_register_inc <= '1';
				next_state <= init;
			else
				-- CNT <= CNT + 1
				-- PC <= PC - 1
				cnt_register_inc <= '1';
				pc_register_dec <= '1';
				next_state <= end_loop_3;
			end if;

		when end_loop_3 =>
			-- while (CNT != 0)
			if (cnt_register = "00000000") then
				next_state <= init;
			else
				-- c <= rom[PC]
				CODE_EN <= '1';
				next_state <= end_loop_4;
			end if;

		when end_loop_4 =>
			-- if (c==']') CNT <= CNT + 1
			-- elsif (c==']') CNT <= CNT - 1
			if (CODE_DATA = X"5D") then -- ]
				cnt_register_inc <= '1';
			elsif (CODE_DATA = X"5B") then -- [
				cnt_register_dec <= '1';
			end if;
			next_state <= end_loop_5;

		when end_loop_5 =>
			-- if (CNT == 0) PC <= PC + 1
			-- else PC <= PC - 1
			if (cnt_register = "00000000") then
				pc_register_inc <= '1';
			else
				pc_register_dec <= '1';
			end if;
			next_state <= end_loop_3;

--------------------------------------------------------
		when put_data_1 => -- .
			-- while (OUT_BUSY) {}
			DATA_EN <= '1';
			DATA_RDWR <= '1';
			next_state <= put_data_2;

		when put_data_2 =>
			-- OUT_DATA <= ram[PTR]
			-- PC <= PC + 1
			if (OUT_BUSY = '1') then
				next_state <= put_data_2;
			else
				OUT_WE <= '1';
				pc_register_inc <= '1';
				next_state <= init;
			end if;

--------------------------------------------------------
		when get_data_1 => -- ,
			-- while (!IN_VLD) {}
			IN_REQ <= '1';
			if (IN_VLD = '1') then
				FSM_sel <= "00";
				next_state <= get_data_2;
			else
				next_state <= get_data_1;
			end if;

		when get_data_2 =>
			-- ram[PTR] <= IN_DATA
			-- PC <= PC + 1
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			pc_register_inc <= '1';
			next_state <= init;

--------------------------------------------------------
		when data_to_tmp_1 => -- $
		    -- DATA_RDATA <= ram[PTR]
			DATA_EN <= '1';
			DATA_RDWR <= '1';
			next_state <= data_to_tmp_2;

		when data_to_tmp_2 =>
			-- TMP <= DATA_RDATA
			-- PC <= PC + 1
			tmp_register_load <= '1';
			pc_register_inc <= '1';
			next_state <= init;

--------------------------------------------------------
		when tmp_to_data_1 => -- !
			-- ram[PTR] <= TMP
			-- PC <= PC + 1
		    FSM_sel <= "01";
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			pc_register_inc <= '1';
			next_state <= init;

--------------------------------------------------------
		when halt => -- null
		    -- PC <= PC
			next_state <= halt;

--------------------------------------------------------
		when other => -- OTHER
			-- PC <= PC + 1
			pc_register_inc <= '1';
			next_state <= init;

--------------------------------------------------------
		when others =>
			null;

	end case;
end process;

end behavioral;
