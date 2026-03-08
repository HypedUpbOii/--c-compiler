reference=../A3-Resources-x86_64/reference-implementations/A3-sclp
our=sclp

make

for file in ../A3-Resources-x86_64/example-programs/Level-3-test-cases/*.c; do
    echo "Checking file $file"

    # run their implementation
    ./$reference --show-tac -d $file > theirs.tac 2> /dev/null
    their_status=$?
    # run our implementation
    ./$our --show-tac -d $file > ours.tac 2> /dev/null
    our_status=$?

    if [ $their_status -eq $our_status ]; then
        if [ $their_status -eq 1 ]; then
            echo "Passed show-tac"
            continue
        fi

        diff -Bw ours.tac theirs.tac > err.log

        if [ $? -eq 0 ]; then
            echo "Passed show-tac"
        else 
            echo "Failed show-tac: tac don't match in file $file"
            exit 1
        fi
    else 
        echo "Failed show-tac : parsed incorrectly on file $file"
        exit 1
    fi

done

rm ours.tac theirs.tac
