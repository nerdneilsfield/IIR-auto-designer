#!/bin/bash

uuid1=`uuidgen`

mkdir -p /tmp/ee252

uuid3=`uuidgen`
path=`pwd`
real_time=`date '+%Y%m%d-%H%-M-%S'`
mkdir -p /tmp/ee252/$uuid3
mkdir -p /tmp/ee252/$uuid3/report
mkdir -p /tmp/ee252/$uuid3/output
mkdir -p /tmp/ee252/$uuid3/rtl
cp $1 /tmp/ee252/$uuid3/rtl/Top.vhd
# mv  /tmp/ee252/${uuid4}.vhd /tmp/ee252/$uuid3/rtl/Top.vhd
cp /var/ee252/REG.vhd /tmp/ee252/$uuid3/rtl/
cp /var/ee252/setup.tcl /tmp/ee252/$uuid3/
cd /tmp/ee252/$uuid3
sed -i "s/CLOCK_PERIOD/$2/g" setup.tcl > /dev/null
dc_shell-t -f setup.tcl
clear
# cat /tmp/ee252/$uuid/report/qor.txt
cp /tmp/ee252/$uuid3/report/qor.txt $path/qor/"${3}.txt"
zip -r "synth${3}.zip" rtl output report setup.tcl
mv "synth${3}.zip"  $path/zip/
cd $path

rm -rf /tmp/ee252/$uuid3
