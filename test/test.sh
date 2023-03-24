for file in `find ../testcases -name "*.tig"`
do
echo $file
../build/lexer_test $file 1>$file.token
../build/parser_test $file 1>$file.abs
done