#!/usr/bin/env bash

home=$(dirname "$0")
NUMBER_TESTS=20
pass=0
for ((i=1; i <= NUMBER_TESTS; i++)); do
    printf "\e[4m#$i...\e[0m\n"
    $home/../cross.exe $home/trial$i.PNG $home/answers$i.txt #>/dev/null
    if (($? != 0)); then
        printf "\e[31mFAIL\e[0m\n"
    else
        ((pass++))
        printf "\e[32mPASS\e[0m\n"
    fi
done
pass=$((pass/NUMBER_TESTS*100))
if ((pass==100)); then
    printf "\e[32m"
elif ((pass>50)); then
    printf "\e[33m"
else
    printf "\e[31m"
fi
printf "$pass%%\e[0m TEST CASES PASSED"
