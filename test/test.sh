for file in `find ../testcases -name "*.tig"`
do
echo $file
../build/parser_test $file 1>$file.abs
done