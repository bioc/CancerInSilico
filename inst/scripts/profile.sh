#! /usr/bin/env bash

rm callgrind.out.*
R -d "valgrind --tool=callgrind" -f profile_standard.R > valgrind_out.txt
kcachegrind callgrind.out.*

