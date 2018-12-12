change_selection [get_s {{design}} ]
## Check the HDL design
check_design > report/check_design.txt
## Create Constraints 
create_clock −period {{clock_period}} −name {{clk}} [get_ports {{clk}}]
