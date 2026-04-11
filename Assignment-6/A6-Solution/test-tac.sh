reference=A5-sclp
our=sclp

make

for file in ../A5-Resources-x86_64/example-programs/Level-5-test-cases/*.c
do
    echo "Checking file $file"

    # run their implementation
    ./$reference --show-ast --sa-ast -d $file > theirs.ast 2> /dev/null
    their_status=$?
    # run our implementation
    ./$our --show-ast --sa-ast -d $file > ours.ast 2> /dev/null
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
            echo "Failed show-ast: ASTs don't match in file $file"
            exit 1
        fi
    else 
        echo "Failed show-ast : parsed incorrectly on file $file"
        exit 1
    fi

    # run their implementation
    ./$reference --show-tac --sa-tac -d $file > theirs.tac 2> /dev/null
    their_status=$?
    # run our implementation
    ./$our --show-tac --sa-tac -d $file > ours.tac 2> /dev/null
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

    # run their implementation
    ./$reference --show-rtl --sa-rtl -d -s $file > theirs.rtl 2> /dev/null
    their_status=$?
    # run our implementation
    ./$our --show-rtl --sa-rtl -d $file > ours.rtl 2> /dev/null
    our_status=$?

    if [ $their_status -eq $our_status ]; then
        if [ $their_status -eq 1 ]; then
            echo "Passed show-rtl"
            continue
        fi

        diff -Bw ours.rtl theirs.rtl > err.log

        if [ $? -eq 0 ]; then
            echo "Passed show-rtl"
        else 
            echo "Failed show-rtl: rtl don't match in file $file"
            exit 1
        fi
    else 
        echo "Failed show-rtl : parsed incorrectly on file $file"
        exit 1
    fi

    # run their implementation
    ./$reference --show-asm -d -s $file > theirs.spim 2> /dev/null
    their_status=$?
    # run our implementation
    ./$our --show-asm -d $file > ours.spim 2> /dev/null
    our_status=$?

    if [ $their_status -eq $our_status ]; then
        if [ $their_status -eq 1 ]; then
            echo "Passed show-asm"
            continue
        fi

        diff -Bw ours.spim theirs.spim > err.log

        if [ $? -eq 0 ]; then
            echo "Passed show-asm"
        else 
            echo "Failed show-asm: SPIM don't match in file $file"
            exit 1
        fi
    else 
        echo "Failed show-asm : parsed incorrectly on file $file"
        exit 1
    fi
done

rm ours.ast theirs.ast
rm ours.tac theirs.tac
rm ours.rtl theirs.rtl
rm ours.spim theirs.spim
