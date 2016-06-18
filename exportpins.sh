#!/bin/bash

for i in 17 18 27 22 23 24 25 4
do
  gpio export $i out
done

gpio exports
