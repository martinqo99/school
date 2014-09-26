--------------------------------------------------------------
-- Projekt: riadenie maticoveho displeja, INP 2011/2012
-- Autor  : Michal Lukac, xlukac05
-- Datum  : 08.10.2011
--------------------------------------------------------------

library IEEE;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

--------------------------------------------------------------
-- Entita
--------------------------------------------------------------

entity ledc8x8 is
	port (
		SMCLK, RESET : IN std_logic;
		ROW : OUT std_logic_vector(7 downto 0);
		LED : OUT std_logic_vector(7 downto 0)
		);
end ledc8x8;

--------------------------------------------------------------
-- Behavioralny popis
--------------------------------------------------------------

architecture behavioral of ledc8x8 is

signal ce, switch : std_logic;
signal dec_result : std_logic_vector(7 downto 0);
signal reg: std_logic_vector(7 downto 0);
signal var: std_logic_vector(21 downto 0);
signal counter : std_logic_vector(7 downto 0);

-- process ctrl_cnt

begin
	ctrl_cnt: process(RESET, SMCLK)
	begin
		if RESET='1' then
			var <= (others => '0');
			counter <= "00000000";
		elsif SMCLK'event and (SMCLK = '1') then
			counter <= counter + 1;
			if counter(7) = '1' then -- zvysime citac
				var <= var + 1;
			end if;
			if (var(7 downto 0) = "11111111") then
				ce <= '1'; -- nastavime ce signal
			else
				ce <= '0';
			end if;
		end if;
	end process ctrl_cnt;

	switch <= var(21);

-- process row_cnt je rot. register

	row_cnt: process(RESET, SMCLK)
	begin
		if RESET='1' then
			reg <= "01111111";
		elsif SMCLK'event and (SMCLK = '1') then
			if ce ='1' then -- rotovanie
				reg <=  reg(0) & reg(7 downto 1);
			end if;
		end if;
	end process row_cnt;

	ROW <= reg;

-- process dec je dekoder

	dec: process(reg)
	begin
			case reg is -- vypis inicialov ML na displej
				when "01111111" => dec_result <= "11110000";
				when "10111111" => dec_result <= "00010000";
				when "11011111" => dec_result <= "00010000";
				when "11101111" => dec_result <= "00010000";
				when "11110111" => dec_result <= "00010001";
				when "11111011" => dec_result <= "00010001";
				when "11111101" => dec_result <= "00010101";
				when "11111110" => dec_result <= "00011011";
				when "11111111" => dec_result <= "00000000";
				when others => dec_result <= "00000000";
			end case;
	end process dec;

-- vlastny multi process

	ml: process(dec_result, switch)
	begin
		if switch = '1' then -- zabezpecenie blikania pomocou switch
			LED <= dec_result;
		else
			LED <= "00000000";
		end if;
	end process ml;

end architecture;
