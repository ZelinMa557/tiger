for file in `find ../testcases -name "*.tig"`
do
echo $file
../build/parser_test $file 1>$file.abs
../build/checker_test $file 2>$file.err.txt
done