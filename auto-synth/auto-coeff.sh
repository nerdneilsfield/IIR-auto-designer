#!/bin/bash

if [ -z $1 ] && [ -z $2 ]; then
    echo "Help:"
    echo "auto-coeff [clock] [coeffients]"
    echo 'You need to use " to warp the coeffients up!'
else
    uuidx=`uuidgen`
    coeff2code $2 > /tmp/ee252/${uuidx}.vhd
    auto-synth /tmp/ee252/${uuidx}.vhd $1
    rm -rf /tmp/ee252/${uuidx}.vhd
fi