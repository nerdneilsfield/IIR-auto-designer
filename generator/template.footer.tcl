# set_input_delay {{input_delay}} −clock {{clk}} [remove_from_collection [all_inputs] {{clk}}]
# set_output_delay {{output_delay}} −clock {{clk}} [all_outputs]
# set_clock_uncertainty .01 [all_clocks]
# set_clock_latency 0.5 −source [get_ports {{clk}}]
# set_load {{load}} [all_outputs]
set_max_area 0
## Compilation
compile
## Save the report
report_hierarchy > report/hierachy.txt
report_reference > report/reference.txt
report_area -hierarchy > report/area.txt
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