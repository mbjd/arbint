#!/bin/bash
# quick dirty script to see the arbint representation of a number
# you can input a number or a bc expression
# If you're on linux, you have to change 'gtac' to tac
# (On mac the coreutils package from homebrew prefixes all commands with 'g')

for i in $(echo "obase=2; $1" | bc | tr -d '\\\n' | rev | fold -w32 | rev); do
	echo "ibase=2;$i" | bc
done | nl -v0 | gtac | column -t | sed 's/^/(2^32)^/g;s/  / * /g'

