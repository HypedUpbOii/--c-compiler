path=../A1-Resources-x86_64
reference=../A1-Resources-x86_64/reference-implementations/A1-sclp
our=sclp

make

for file in ../A1-Resources-x86_64/example-programs/Level-1-test-cases/*.c; do
    echo "Checking file $file"

    # run their implementation
    ./$reference --show-tokens -d $file > theirs.toks
    their_status=$?
    # run our implementation
    ./$our --show-tokens -d $file > ours.toks 
    our_status=$?

    if [ $their_status -eq $our_status ]; then
        if [ $their_status -eq 1 ]; then
            echo "Passed"
            continue
        fi

        diff -Bw ours.toks theirs.toks > err.log

        if [ $? -eq 0 ]; then
            echo "Passed"
        else 
            echo "Failed : tokens don't match in file $file"
            exit 1
        fi
    else 
        echo "Failed : parsed incorrectly on file $file"
        exit 1
    fi

done

rm ours.toks theirs.toks
