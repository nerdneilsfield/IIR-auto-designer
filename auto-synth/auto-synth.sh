#!/bin/bash

if [ -z $1 ] && [ -z $2 ]; then
    echo "Help:"
    echo "auto-synth [vhd-file-path] [clock]"
else
    uuid=`uuidgen`
    path=`pwd`
    real_time=`date '+%Y%m%d-%H%-M-%S'`
    mkdir -p /tmp/ee252/$uuid
    mkdir -p /tmp/ee252/$uuid/report
    mkdir -p /tmp/ee252/$uuid/output
    mkdir -p /tmp/ee252/$uuid/rtl
    cp /var/ee252/setup.tcl /tmp/ee252/$uuid/
    cp $1 /tmp/ee252/$uuid/rtl/Top.vhd
    cp /var/ee252/REG.vhd /tmp/ee252/$uuid/rtl/
    cd /tmp/ee252/$uuid
    sed -i "s/CLOCK_PERIOD/$2/g" setup.tcl > /dev/null
    dc_shell-t -f setup.tcl
    clear
    cat /tmp/ee252/$uuid/report/qor.txt
    zip -r "auto-syntdate${real_time}.zip" rtl output report setup.tcl
    mv "auto-syntdate${real_time}.zip"  $path
    cd $path
    rm -rf /tmp/ee252/$uuid
    echo "The synthesis zip file have been to your current directory!"
    echo "auto-syntdate${real_time}.zip"
fi