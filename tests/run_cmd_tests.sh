#!/bin/bash
# Run unit tests for individual commands
BIN=../bin # Folder that contains the binaries for all tests
PWD=$(pwd) # The current working directory
LS_OUT=$(ls) # Expected output for ls on current directory
mkdir temp # Make temporary directory
rm log.txt # Delete old log file
touch log.txt # Set up log file
echo "This is a test file" > temp/foo.txt # Set up test file

# Test cp
echo "Testing copy..."
$BIN/cp temp/foo.txt temp/bar.txt >> log.txt
[[ $? == 0 ]] && diff -s temp/foo.txt temp/bar.txt >> log.txt && echo "PASSED" || echo "FAILED"
$BIN/cp >> log.txt
[[ $? == 1 ]] && echo "PASSED" || echo "FAILED"

# Test ls
echo "Testing ls..."
[[ $($BIN/ls) == $LS_OUT ]] && echo "PASSED" || echo "FAILED"
$BIN/ls no_exist_dir >> log.txt
[[ $? == 1 ]] && echo "PASSED" || echo "FAILED"
$BIN/ls ../no_exist_dir >> log.txt
[[ $? == 1 ]] && echo "PASSED" || echo "FAILED"
$BIN/ls this is alot of bad arguments for ls to process >> log.txt
[[ $? == 1 ]] && echo "PASSED" || echo "FAILED"

# Test mkdir
echo "Testing mkdir..."
$BIN/mkdir temp/test_dir >> log.txt
[[ $? == 0 ]] && [ -d temp/test_dir ] && echo "PASSED" || echo "FAILED"
$BIN/mkdir temp/dir1 temp/dir2 >> log.txt
[[ $? == 0 ]] && [ -d temp/dir_1 ] && [ -d temp/dir_2 ] && echo "PASSED" || echo "FAILED"

# Test cd
echo "Testing cd..."
$BIN/cd temp/test_dir >> log.txt
[[ $? == 0 ]] && ! [ -d temp/test_dir ] && echo "PASSED" || echo "FAILED"
$BIN/cd ../.. >> log.txt
[[ $? == 0 ]] && [ -d temp/dir_1 ] && [ -d temp/dir_2 ] && echo "PASSED" || echo "FAILED"
$BIN/cd temp/dir_1 temp/dir_2 >> log.txt
[[ $? == 0 ]] && [ -d temp/dir_1 ] && [ -d temp/dir_2 ] && echo "PASSED" || echo "FAILED"

# Test running executables and test exit
echo "Testing running an executable..."
$BIN/cd .. >> log.txt
$BIN/./soyshell >> tests/log.txt
[[ $? == 0 ]] && [ -d tests/temp/test_dir ] && echo "PASSED" || echo "FAILED"
echo "Testing exit..."
$BIN/exit >> tests/log.txt
$BIN/cd tests >> log.txt
[[ $? == 0 ]] && [ -d tests/temp/dir_1 ] && [ -d tests/temp/dir_2 ] && echo "PASSED" || echo "FAILED"

# Test rmdir
echo "Testing rmdir..."
$BIN/rmdir temp/test_dir >> log.txt
[[ $? == 0 ]] && ! [ -d temp/test_dir ] && echo "PASSED" || echo "FAILED"
$BIN/rmdir temp/dir_1 temp/dir_2 >> log.txt
[[ $? == 0 ]] && ! [ -d temp/dir_1 ] && ! [ -d temp/dir_2 ] && echo "PASSED" || echo "FAILED"

# Test pwd
echo "Testing pwd..."
[[ $($BIN/pwd) == $PWD ]] && echo "PASSED" || echo "FAILED"
[[ $($BIN/pwd three extra arguments) == $PWD ]] && echo "PASSED" || echo "FAILED"

# Clean up
rm -r temp
rm foo.txt
rm bar.txt
