path=../A1-Resources-x86_64
reference=../A1-Resources-x86_64/reference-implementations/A1-sclp
our=scan

# our=$reference

make scan

for file in ../A1-Resources-x86_64/example-programs/Level-2-test-cases/*.c; do
    echo "Checking file $file"

    # run their implementation
    ./$reference --show-tokens -d $file > theirs.toks

    # run our implementation
    ./$our < $file 1> ours.toks 

    diff -Bw ours.toks theirs.toks

    if [ $? -eq 0 ]; then
        echo "Passed"
    else 
        echo "Failed"
    fi
done

rm ours.toks theirs.toks

