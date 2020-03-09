#!/usr/bin/env bash

home=$(dirname "$0")

if [[ !(-f $home/../crossot.exe) ]]; then
    printf "\e[31mERROR\e[0m: Need compiled executable file \e[4m../crossot.exe\e[0m to run tests on\n"
    exit 1
fi

NUMBER_TESTS=20
pass=0
for ((i=1; i <= NUMBER_TESTS; i++)); do
    printf "\e[4m#$i...\e[0m\n"
    $home/../crossot.exe $home/trial$i.PNG $home/answers$i.txt #>/dev/null
    if (($? != 0)); then
        printf "\e[31mFAIL\e[0m\n"
    else
        ((pass++))
        printf "\e[32mPASS\e[0m\n"
    fi
done
percent=$((pass*100/NUMBER_TESTS))
if ((percent==100)); then
    printf "\e[32m"
elif ((percent>50)); then
    printf "\e[33m"
else
    printf "\e[31m"
fi
printf "$percent%%\e[0m TEST CASES PASSED ($pass/$NUMBER_TESTS)\n"
