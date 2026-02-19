path=../A2-Resources-x86_64
reference=../A2-Resources-x86_64/reference-implementations/A2-sclp
our=sclp

make

for file in ../A2-Resources-x86_64/example-programs/tests/*.c; do
    echo "Checking file $file"

    # run their implementation
    ./$reference --show-tokens --sa-scan -d $file > theirs.toks 2> /dev/null
    their_status=$?
    # run our implementation
    ./$our --show-tokens --sa-scan -d $file > ours.toks 2> /dev/null
    our_status=$?

    if [ $their_status -eq $our_status ]; then
        if [ $their_status -eq 1 ]; then
            echo "Passed sa-scan"
            continue
        fi

        diff -Bw ours.toks theirs.toks > err.log

        if [ $? -eq 0 ]; then
            echo "Passed sa-scan"
        else 
            echo "Failed sa-scan : tokens don't match in file $file"

            exit 1
        fi
    else 
        echo "Failed sa-scan : parsed incorrectly on file $file"
        exit 1
    fi

    # run their implementation
    ./$reference --show-tokens --sa-parse -d $file > theirs.toks 2> /dev/null
    their_status=$?
    # run our implementation
    ./$our --show-tokens --sa-parse -d $file > ours.toks 2> /dev/null
    our_status=$?

    if [ $their_status -eq $our_status ]; then
        if [ $their_status -eq 1 ]; then
            echo "Passed sa-parse"
            continue
        fi

        diff -Bw ours.toks theirs.toks > err.log

        if [ $? -eq 0 ]; then
            echo "Passed sa-parse"
        else
            echo "Failed sa-parse : tokens don't match in file $file"

            exit 1
        fi
    else
        echo "Failed sa-parse : parsed incorrectly on file $file"
        exit 1
    fi
    # run their implementation
    ./$reference --show-ast -d $file > theirs.ast 2> /dev/null
    their_status=$?
    # run our implementation
    ./$our --show-ast -d $file > ours.ast 2> /dev/null
    our_status=$?

    if [ $their_status -eq $our_status ]; then
        if [ $their_status -eq 1 ]; then
            echo "Passed show-ast"
            continue
        fi

        diff -Bw ours.ast theirs.ast > err.log

        if [ $? -eq 0 ]; then
            echo "Passed show-ast"
        else 
            echo "Failed show-ast: ast don't match in file $file"
            exit 1
        fi
    else 
        echo "Failed show-ast : parsed incorrectly on file $file"
        exit 1
    fi

done

rm ours.toks theirs.toks
rm ours.ast theirs.ast
