if [ $# != 2 ]; then
  echo 'Usage: sh generator.sh TARGET OUTDIR'
  exit
fi

# generate the $CXXSOURCES array

CXXSOURCES=''
CXXSOURCES=${CXXSOURCES}\ `echo $PWD/src/startup/*.cc`
CXXSOURCES=${CXXSOURCES}\ `echo $PWD/src/utilities/*.cc`
CXXSOURCES=${CXXSOURCES}\ `echo $PWD/src/stdlib/*.cc`
CXXSOURCES=${CXXSOURCES}\ `echo $PWD/src/memory/*.cc`
CXXSOURCES=${CXXSOURCES}\ `echo $PWD/src/scheduler/*.cc`
CXXSOURCES=${CXXSOURCES}\ `echo $PWD/dependencies/analloc2/src/*.cc`
CXXSOURCES=${CXXSOURCES}\ `echo $PWD/dependencies/analloc2/src/topology/*.cc`

if [ $1 == 'x64' ]; then
  CXXSOURCES=${CXXSOURCES}\ `echo $PWD/src/platform/x64/*.cc`
  CXXSOURCES=${CXXSOURCES}\ `echo $PWD/src/platform/x64/memory/*.cc`
  CXXSOURCES=${CXXSOURCES}\ `echo $PWD/src/platform/x64/interrupts/*.cc`
  CXXSOURCES=${CXXSOURCES}\ `echo $PWD/src/platform/x64/acpi/*.cc`
  CXXSOURCES=${CXXSOURCES}\ `echo $PWD/src/platform/x64/multiprocessing/*.cc`
else
  echo Unknown TARGET $1
  exit
fi

# generate the $CSOURCES array

CSOURCES=`echo $PWD/dependencies/anlock/src/*.c`
CSOURCES=${CSOURCES}\ `echo $PWD/src/stdlib/*.c`

# generate the $NASMSOURCES array

NASMSOURCES=''

if [ $1 == 'x64' ]; then
  NASMSOURCES=${NASMSOURCES}\ `echo $PWD/src/platform/x64/interrupts/*.s`
else
  echo Unknown TARGET $1
  exit
fi

# generate the $INCLUDES array

INCLUDES="$INCLUDES -I$PWD/src -I$PWD/src/stdlib/cheaders -I$PWD/src/stdlib/cppheaders -I$PWD/dependencies/anlock/src -I$PWD/dependencies/analloc2/include"

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

printf "all:" >>$2/Makefile
for file in $CXXSOURCES; do
  printf " `basename $file .cc`.o" >>$2/Makefile
done
for file in $CSOURCES; do
  printf " `basename $file .c`.o" >>$2/Makefile
done
for file in $NASMSOURCES; do
  printf " `basename $file .s`.o" >>$2/Makefile
done
printf "\n\n" >>$2/Makefile

for file in $CXXSOURCES; do
  printf "`basename $file .cc`.o: $file\n" >>$2/Makefile
  printf "\t" >>$2/Makefile
  printf "$CXX -c $CXXFLAGS $INCLUDES $file -o `basename $file .cc`.o\n" >>$2/Makefile
  echo >>$2/Makefile
done

for file in $CSOURCES; do
  printf "`basename $file .c`.o: $file\n" >>$2/Makefile
  printf "\t" >>$2/Makefile
  printf "$CC -c $CFLAGS $INCLUDES $file -o `basename $file .c`.o\n" >>$2/Makefile
  echo >>$2/Makefile
done

for file in $NASMSOURCES; do
  printf "`basename $file .s`.o: $file\n" >>$2/Makefile
  printf "\t" >>$2/Makefile
  printf "$NASM -f elf64 $file -o `basename $file .s`.o\n" >>$2/Makefile
  echo >>$2/Makefile
done
