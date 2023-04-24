for file in `find ../testcases -name "*.tig"`
do
echo $file
../build/checker_test $file 1>$file.abs 2>$file.err
done