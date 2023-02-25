#!/bin/sh

# create the test directory if it doesn't exist
mkdir -p test

# download the minunit.h file from the repository
curl -L -o test/minunit.h https://raw.githubusercontent.com/siu/minunit/master/minunit.h
