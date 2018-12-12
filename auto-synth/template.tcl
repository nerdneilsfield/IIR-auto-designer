set file_name "rtl/Top.vhd"
set reg_name "rtl/REG.vhd"
set dir_path [pwd]
## Set the fundary library path
set my_lib_path "/opt/synopsys/PDK_dir/TSMC90/aci/sc-x/synopsys"
set search_path "$search_path $my_lib_path"
set search_path "$search_path $my_lib_path"
set synthetic_library {typical.db}
set target_library {typical.db}
set link_library {* typical.db}
set symbol_library {generic.sdb}

## load the HDL code
set file_path $dir_path/$file_name
set reg_path $dir_path/$reg_name
read_file -format vhdl $reg_path
read_file -format vhdl $file_path
#read_file -format vhdl {/home/dengqi/Source/EE252/Projects1/rtl/REG.vhd}
change_selection [get_s TOP]
## Check the HDL design
check_design > report.txt

## Create Constraints
create_clock -period CLOCK_PERIOD -name CLK [get_ports CLK]
#set_input_delay 0.5 -clock CLK [remove_from_collection [all_inputs] CLK]
#set_output_delay 0.5 -clock CLK [all_outputs]
#set_clock_uncertainty .01 [all_clocks]
#set_clock_latency 0.5 -source [get_ports CLK]
#set_load 0.5 [all_outputs]
set_max_area 0

## Compilation
compile

## Save the report
report_hierarchy > report/hierachy.txt
report_reference > report/reference.txt
report_area > report/area.txt
report_area -hierarchy > report/area.txt
report_resources > report/resources.txt
report_timing  > report/timing.txt
report_constraint -verbose  > report/constaint.txt
report_qor > report/qor.txt

## Save the database and gate level netlists
write_sdc Top.sdc
write -f ddc -hierarchy -output  output/Top_netlist.ddc
write -hierarchy -format verilog -output  output/Top_netlist.v
write -hierarchy -format vhdl -output output/Top_netlist.vhdl


exit