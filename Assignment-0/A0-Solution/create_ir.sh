#!/bin/bash
# run in A0-Answers directory

valid_files=$(ls example-programs/Level-5-test-cases)
invalid_files=$(ls example-programs/Level-5-invalid-test-cases)

for f in $valid_files; do
    reference-implementations/A0-sclp --show-ast example-programs/Level-5-test-cases/$f
    reference-implementations/A0-sclp --show-tac example-programs/Level-5-test-cases/$f
    reference-implementations/A0-sclp --show-rtl example-programs/Level-5-test-cases/$f
    reference-implementations/A0-sclp --show-asm example-programs/Level-5-test-cases/$f
done

for f in $invalid_files; do
    reference-implementations/A0-sclp --show-ast example-programs/Level-5-invalid-test-cases/$f
    reference-implementations/A0-sclp --show-tac example-programs/Level-5-invalid-test-cases/$f
    reference-implementations/A0-sclp --show-rtl example-programs/Level-5-invalid-test-cases/$f
    reference-implementations/A0-sclp --show-asm example-programs/Level-5-invalid-test-cases/$f
done