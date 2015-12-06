#!/bin/bash

if [ $# -ne 1 ]; then 
echo "Usage: bash "$0" <*.gaccess-file>";
exit -1;
fi

output_file="temp_files/""$1""_read"
simlation_output_file="$1"".simuation.out"

if [ -f "$simlation_output_file" ];then 
  rm "$simlation_output_file"
fi

python get_reads.py "$1" > "$output_file"

count=$(wc -l < "$output_file")

cd arc
make 
cd ..

for cache_size in `seq 3000 128 10001`;
do
  ./arc/test $count $cache_size < "$output_file" >> "$simlation_output_file"
done
