#!/bin/bash

WORDLIST=${WORDLIST:-rockyou.txt}
TABLE=${TABLE:-mytable.table}
HASHLIST=${HASHLIST:-hashes.txt}
PASSLIST=${PASSLIST:-passwords.txt}

PROG=$(which stormbreaker)
if [[ -z $PROG ]]; then
    PROG=./stormbreaker
fi

# G Mode table generation test
echo "Starting table generation test ..."
$PROG -G -i $WORDLIST -o $TABLE
if [[ $(wc -l < $WORDLIST) -ne $(wc -l < $TABLE) ]]; then
    echo "FAILED: The generated table is not the same size as the input !"
    exit
fi

# G Mode hash generation test
echo "Stating hash generation test ..."
$PROG -G -m -i $WORDLIST -o $HASHLIST
if [[ $(wc -l < $WORDLIST) -ne $(wc -l < $HASHLIST) ]]; then
    echo "FAILED: The generated hashes file is not the same size as the input !"
    exit
fi

echo "Starting hash lookup test ..."
$PROG -L -t $TABLE -i $HASHLIST -o matches.txt
if [[ $(wc -l < $WORDLIST) -ne $(wc -l < matches.txt) ]]; then
    echo "FAILED: Not all hash found a match !"
    exit
fi

echo "Starting accuracy test ..."
$PROG -G -j 1 -m -i $WORDLIST -o $HASHLIST
$PROG -L -j 1 -m -t $TABLE -i $HASHLIST -o $PASSLIST
if [[ $(diff $WORDLIST $PASSLIST) ]]; then
    echo "FAILED: Input and output don't match !"
    exit
fi

echo "SUCCESS !"