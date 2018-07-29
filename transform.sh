#!/bin/bash

lines=$(jq -c '.[]' $1 | wc -l)
echo "19 6 ${lines}" >> votes.in

jq -c '.[]' $1 | while read line
do
    echo ${line} | jq '.[]' >> votes.in
    echo '' >> votes.in
done
