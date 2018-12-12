#!/bin/bash

if [ -z $1 ] && [ -z $2 ]; then
    echo "Help:"
    echo "auto-coeff [coeffients] [clock]"
    echo 'You need to use " to warp the coeffients up!'
else
    uuidx=`uuidgen`
    coeff2code $1 > /tmp/ee252/${uuidx}.vhd
    auto-synth /tmp/ee252/${uuidx}.vhd $2
    rm -rf /tmp/ee252/${uuidx}.vhd
fi