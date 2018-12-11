#!/usr/bin/env python3

from jinja2 import Template

template_file = ""

with open('template.setup.tcl', 'r') as file:
    template_file = file.read()


template = Template(template_file)

res = template.render(
    file_path = "No",
    file_type = "vhdl",
    clk = "CLK",
    design = "TOP",
    clock_period = "5",
    input_delay = "5",
    output_delay = "5",
    load = "0.5"
)

print(res)
