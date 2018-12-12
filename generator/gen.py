#!/usr/bin/env python3

from jinja2 import Template
from os import path
import toml

_base_dir = path.abspath('.')

template_header = ""
template_file = ""
template_footer = ""
configs = {}


with open('template.setup.tcl', 'r') as file:
    template_file = file.read()
with open('template.header.tcl', 'r') as file:
    template_header = file.read()
with open('template.footer.tcl', 'r') as file:
    template_footer = file.read()
with open('setup.toml', 'r') as file:
    configs = toml.loads(file.read())

template = Template(template_file)

template_path = "read_file -format %s {%s}\n" % (
    configs["file_type"], path.join(_base_dir, configs["path"]))
template_mid = template.render(
    clk=configs["clk_name"],
    design=configs["design"],
    clock_period=configs["clock_period"]
)

res = template_header + template_path + template_mid + template_footer

with open('setup.tcl', 'w') as file:
    file.write(res)
