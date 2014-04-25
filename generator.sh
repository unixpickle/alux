#!/bin/bash
#
# Copyright (c) 2014, Alex Nichol and Alux contributors.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice,
#	this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#	this list of conditions and the following disclaimer in the documentation
#	and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.


if [ $# != 2 ]; then
  echo 'Usage: sh generator.sh TARGET OUTDIR'
  exit
fi

# generate the $SOURCES array

SOURCES=`echo $PWD/src/platform/general/*.cc`

if [ $1 == 'x64' ]; then
  SOURCES=${SOURCES}\ `echo $PWD/src/platform/x64/*.cc`
else
  echo 'Unknown TARGET ' $1
  exit
fi

# generate the $INCLUDES array

INCLUDES="$INCLUDES -I$PWD/src -I$PWD/src/stdlib/cheaders -I$PWD/src/stdlib/cppheaders"

# generate output location

if [ -d $2 ]; then
  echo 'warning: output directory already exists'
elif [ -f $2 ]; then
  echo 'error: output directory is a file!'
  exit
else
  mkdir $2
fi

# generate Makefile

for file in $SOURCES; do
  printf "`basename $file .cc`.o: $file\n" >>$2/Makefile
  printf "\t" >>$2/Makefile
  printf "$CXX -c $CFLAGS $CXXFLAGS $INCLUDES $file -o `basename $file .cc`.o\n" >>$2/Makefile
  echo >>$2/Makefile
done
