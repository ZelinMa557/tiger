for ((i=1; i <= 49; i++))
do
../build/lexer_test ../testcases/test$i.tig 1>../testcases/test$i.out
done