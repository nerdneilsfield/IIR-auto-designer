## Template aruguments
# - file_path : the path of HDL code "/home/xxx/1.vhdl"
# - design: the module of entity name of HDL code eg. "TOP" 
# - file_type: the type of HDL code - verilog or vhdl 
# - clk: the clock name in HDL code - eg. CLK
# - clock_period: period of clock - unit: ns 
# - input_delay - unit: ns
# - output_dealy - unit: ns
# - load

# Set Designer
# set company {EE252}
# set designer {Projects 1}

## Set the fundary library path
set my_lib_path "/opt/synopsys/PDK_dir/TSMC90/aci/sc−x/synopsys"
set search_path "$search_path $my_lib_path"
set search_path "$search_path $my_lib_path"
set synthetic_library {typical.db}
set target_library {typical.db}
set link_library {* typical.db}
set symbol_library {generic.sdb}
## load the HDL code
read_file −format {{file_type}} { {{file_path}} }
change_selection [get_s {{design}} ]
## Check the HDL design
check_design > report/check_design.txt
## Create Constraints 
create_clock −period {{clock_period}} −name {{clk}} [get_ports {{clk}}]
set_input_delay {{input_delay}} −clock {{clk}} [remove_from_collection [all_inputs] {{clk}}]
set_output_delay {{output_delay}} −clock {{clk}} [all_outputs]
set_clock_uncertainty .01 [all_clocks]
set_clock_latency 0.5 −source [get_ports {{clk}}]
set_load {{load}} [all_outputs]
set_max_area 0
## Compilation
compile
## Save the report
report_hierarchy > report/hierachy.txt
report_reference > report/reference.txt
report_area > report/area.txt
report_resources > report/resources.txt
report_timing  > report/timing.txt
report_constraint −verbose  > report/constaint.txt
report_qor > report/qor.txt
## Save the database and gate level netlists
write_sdc Top.sdc
write −f ddc −hierarchy −output  output/netlist.ddc
write −hierarchy −format verilog −output  output/netlist.v
write −hierarchy −format vhdl −output output/netlist.vhdl
exit
