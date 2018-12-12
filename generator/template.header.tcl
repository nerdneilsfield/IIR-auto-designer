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

## load the HDL code
## Set the fundary library path
set my_lib_path "/opt/synopsys/PDK_dir/TSMC90/aci/sc−x/synopsys"
set search_path "$search_path $my_lib_path"
set search_path "$search_path $my_lib_path"
set synthetic_library {typical.db}
set target_library {typical.db}
set link_library {* typical.db}
set symbol_library {generic.sdb}
