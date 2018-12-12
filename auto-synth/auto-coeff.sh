#!/bin/bash

if [ -z $1 ] && [ -z $2 ]; then
    echo "Help:"
    echo "auto-coeff [clock] [coeffients_file_inline]"
    echo 'You need to use " to warp the coeffients up!'
else
    uuidx=`uuidgen`
    coeffients=`cat $2`
    coeff2code $coeffients > /tmp/ee252/${uuidx}.vhd
    auto-synth /tmp/ee252/${uuidx}.vhd $1
    rm -rf /tmp/ee252/${uuidx}.vhd
fi