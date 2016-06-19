#!/bin/bash

# Exports the pins using the gpio utility 
# to the /sys/class/gpio interface.

pins=( 17 18 27 22 23 24 25 4 ) # Broadcom pin numbers

# Print some usage info.
function print_usage {
  echo "Usage: `basename $0` [OPTION]"
  echo "Exports the following GPIO pins:"
  echo "${pins[@]}"
  echo "All numbers are Broadcom pin numbers."
  echo "The following option may be used:"
  echo -e "  -u,--unexport\t\tunexport all pins"
  echo -e "\t\t\t  not only the ones exported with this utility"
  echo -e "  -h,--help\t\tdisplay this help and exit"
  return
}

# Unexport all pins.
function unexport_pins {
  gpio unexportall
  echo "Unexported all pins."
  return
}

# Process parameters.
while [[ $# -gt 0 ]]; do
  current="$1"
  case $current in
    -u | --unexport)
      unexport_pins
      exit 0
      ;;
    -h | --help | *)
      print_usage
      exit 0
      ;;
  esac
  shift
done

# Export pins.
echo -n "Exporting pins: "

for i in ${pins[@]}	# loop through all the pins in the array
do
  echo -n "$i "		# print pin number
  gpio export $i out	# export pin as output
  sleep 0.2s		# wait 0.2s
  gpio -g write $i 1	# set pin high
done
echo ""

gpio exports # show all exported pins
