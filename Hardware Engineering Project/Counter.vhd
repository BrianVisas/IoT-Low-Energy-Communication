----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    14:24:48 06/16/2021 
-- Design Name: 
-- Module Name:    Counter - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity Counter is
generic(n : POSITIVE := 10); --size of counter
    Port ( clk : in  STD_LOGIC;
           enable : in  STD_LOGIC;
           reset : in  STD_LOGIC;
           counter_output : out  STD_LOGIC_VECTOR (n-1 downto 0));
end Counter;


architecture Behavioral of Counter is
signal count : STD_LOGIC_VECTOR(n-1 downto 0);
begin
process (clk, reset)
begin
if (reset = '0') then
count <= (others => '0');
elsif (clk'event and clk = '1') then
if (enable = '1') then
count <= count + 1;
end if;
end if;
end process;
counter_output <= count;
end Behavioral;
