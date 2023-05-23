if [ $# -lt 2 ]; then

  echo "Expected input file & output file as args."
  echo "Example:"
  echo "./tiger.sh test.tig test"

  exit 1

fi

./tiger $1 temp.bc
if [ $? != 0 ]
then
    echo "Fail to compile, exit."
    exit $?
fi
clang temp.bc -c -o temp.o
clang -o $2 temp.o runtime.o
rm temp*