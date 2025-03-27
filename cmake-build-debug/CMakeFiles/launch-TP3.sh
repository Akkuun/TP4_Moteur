#!/bin/sh
bindir=$(pwd)
cd /home/mathis/Documents/M1_IMAGINE/MoteurDeJeux/3/TP3/TP3/
export 

if test "x$1" = "x--debugger"; then
	shift
	if test "xYES" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		/usr/bin/gdb -batch -command=$bindir/gdbscript --return-child-result /home/mathis/Documents/M1_IMAGINE/MoteurDeJeux/3/TP3/cmake-build-debug/TP3 
	else
		"/home/mathis/Documents/M1_IMAGINE/MoteurDeJeux/3/TP3/cmake-build-debug/TP3"  
	fi
else
	"/home/mathis/Documents/M1_IMAGINE/MoteurDeJeux/3/TP3/cmake-build-debug/TP3"  
fi
