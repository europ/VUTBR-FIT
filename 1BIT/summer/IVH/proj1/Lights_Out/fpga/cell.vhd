-- IVH - Seminar VHDL
-- Projekt 3
-- xtotha01
-- Adrian Toth
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use work.maskpack.ALL; -- proj1
--===============================================================================================
entity cell is
   GENERIC (
	  MASK              : mask_t := (others => '1') -- mask_t from math_pack(proj1)
   );
   Port (
	  INVERT_REQ_IN     : in   STD_LOGIC_VECTOR (3 downto 0);
	  INVERT_REQ_OUT    : out  STD_LOGIC_VECTOR (3 downto 0);

	  KEYS              : in   STD_LOGIC_VECTOR (4 downto 0);

	  SELECT_REQ_IN     : in   STD_LOGIC_VECTOR (3 downto 0);
	  SELECT_REQ_OUT    : out  STD_LOGIC_VECTOR (3 downto 0);

	  INIT_ACTIVE       : in   STD_LOGIC;
	  ACTIVE            : out  STD_LOGIC;

	  INIT_SELECTED     : in   STD_LOGIC;
	  SELECTED          : out  STD_LOGIC;

	  CLK               : in   STD_LOGIC;
	  RESET             : in   STD_LOGIC
   );
end cell;
--===============================================================================================
architecture Behavioral of cell is
  constant IDX_TOP    : NATURAL := 0; -- index sousedni bunky nachazejici se nahore v signalech *_REQ_IN a *_REQ_OUT, index klavesy posun nahoru v KEYSS
  constant IDX_LEFT   : NATURAL := 1; -- ... totez        ...                vlevo
  constant IDX_RIGHT  : NATURAL := 2; -- ... totez        ...                vpravo
  constant IDX_BOTTOM : NATURAL := 3; -- ... totez        ...                dole

  constant IDX_ENTER  : NATURAL := 4; -- index klavesy v KEYSS, zpusobujici inverzi bunky (enter, klavesa 5)
  signal active_sig   : STD_LOGIC;
  signal selected_sig : STD_LOGIC;
begin

--OK Pozadavky na funkci (sekvencni chovani vazane na vzestupnou hranu CLK)
--OK   pri resetu se nastavi ACTIVE a SELECTED na vychozi hodnotu danou signaly INIT_ACTIVE a INIT_SELECTED
--OK   pokud je bunka aktivni a prijde signal z klavesnice, tak se bud presune aktivita pomoci SELECT_REQ na dalsi bunky nebo se invertuje stav bunky a jejiho okoli pomoci INVERT_REQ (klavesa ENTER)
--OK   pokud bunka neni aktivni a prijde signal INVERT_REQ, invertuje svuj stav
--OK   pozadavky do okolnich bunek se posilaji a z okolnich bunek prijimaji, jen pokud je maska na prislusne pozici v '1'



	process (CLK, RESET)
	begin

		-- pri resetu se nastavi ACTIVE a SELECTED na vychozi hodnotu danou signaly INIT_ACTIVE a INIT_SELECTED
		  -- reseting cell
		if ( RESET = '1' ) then
			active_sig <= INIT_ACTIVE;
			selected_sig <= INIT_SELECTED;

		-- Pozadavky na funkci (sekvencni chovani vazane na vzestupnou hranu CLK)
		-- Pozadavky do okolnich bunek se posilaji a z okolnich bunek prijimaji, jen pokud je maska na prislusne pozici v '1'
		elsif rising_edge(CLK) then

			-- erasing previous values for SELECT
			SELECT_REQ_OUT(IDX_TOP) <= '0';
			SELECT_REQ_OUT(IDX_LEFT) <= '0';
			SELECT_REQ_OUT(IDX_RIGHT) <= '0';
			SELECT_REQ_OUT(IDX_BOTTOM) <= '0';

			-- erasing previous values for INVERT
			INVERT_REQ_OUT(IDX_TOP) <= '0';
			INVERT_REQ_OUT(IDX_LEFT) <= '0';
			INVERT_REQ_OUT(IDX_RIGHT) <= '0';
			INVERT_REQ_OUT(IDX_BOTTOM) <= '0';

			-- pokud je bunka aktivni a prijde signal z klavesnice, tak se bud presune aktivita
			--   pomoci SELECT_REQ na dalsi bunky nebo se invertuje stav bunky a jejiho okoli
			--   pomoci INVERT_REQ (klavesa ENTER)
			if ( selected_sig = '1' ) then

				-- skip from selected cell to another direction (up-top,left,right,down-bottom)
				--   if neighbor exits
				if ( KEYS(IDX_TOP) = '1' and MASK.top = '1' ) then
					SELECT_REQ_OUT(IDX_TOP) <= '1';
					selected_sig <= '0';
				end if;

				if ( KEYS(IDX_LEFT) = '1' and MASK.left = '1' ) then
					SELECT_REQ_OUT(IDX_LEFT) <= '1';
					selected_sig <= '0';
				end if;

				if ( KEYS(IDX_RIGHT) = '1' and MASK.right = '1' ) then
					SELECT_REQ_OUT(IDX_RIGHT) <= '1';
					selected_sig <= '0';
				end if;

				if ( KEYS(IDX_BOTTOM) = '1' and MASK.bottom = '1' ) then
					SELECT_REQ_OUT(IDX_BOTTOM) <= '1';
					selected_sig <= '0';
				end if;

				-- cell is selected and inverted(ENTER was pressed)
					-- invert myself
					-- send invert request to neigbor cells (top,left,right,bottom)
				if ( KEYS(IDX_ENTER) = '1' ) then

					active_sig <= not active_sig;

					if ( MASK.top = '1' ) then
						INVERT_REQ_OUT(IDX_TOP) <= '1';
					end if;

					if ( MASK.left = '1' ) then
						INVERT_REQ_OUT(IDX_LEFT) <= '1';
					end if;

					if ( MASK.right = '1' ) then
						INVERT_REQ_OUT(IDX_RIGHT) <= '1';
					end if;

					if ( MASK.bottom = '1' ) then
						INVERT_REQ_OUT(IDX_BOTTOM) <= '1';
					end if;

				end if;

			end if;

			-- pokud bunka neni aktivni a prijde signal INVERT_REQ, invertuje svuj stav
			  -- cell got request to invert from neighbor cell
			if (selected_sig = '0') then

				if ( INVERT_REQ_IN(IDX_TOP) = '1' and MASK.top = '1') then
					active_sig <= not active_sig;
				end if;

				if ( INVERT_REQ_IN(IDX_LEFT) = '1' and MASK.left = '1') then
					active_sig <= not active_sig;
				end if;

				if ( INVERT_REQ_IN(IDX_RIGHT) = '1' and MASK.right = '1') then
					active_sig <= not active_sig;
				end if;

				if ( INVERT_REQ_IN(IDX_BOTTOM) = '1' and MASK.bottom = '1') then
					active_sig <= not active_sig;
				end if;

				if ( SELECT_REQ_IN(IDX_TOP) = '1' and MASK.top = '1' ) then
					selected_sig <= '1';
				end if;

				if ( SELECT_REQ_IN(IDX_LEFT) = '1' and MASK.left = '1' ) then
					selected_sig <= '1';
				end if;

				if ( SELECT_REQ_IN(IDX_RIGHT) = '1' and MASK.right = '1' ) then
					selected_sig <= '1';
				end if;

				if ( SELECT_REQ_IN(IDX_BOTTOM) = '1' and MASK.bottom = '1' ) then
					selected_sig <= '1';
				end if;

			end if;
		end if;
	end process;

	ACTIVE <= active_sig;
	SELECTED <= selected_sig;

end Behavioral;
