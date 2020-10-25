#!/bin/bash
# Run unit tests for individual commands
BIN=../bin # Folder that contains the binaries for all tests
PWD=$(pwd) # The current working directory
mkdir temp # Make temporary directory
mkdir temp/temp2 
rm log.txt # Delete old log file
touch log.txt # Set up log file
echo "This is a test file" > temp/foo.txt # Set up test file
echo "This is a second test file" > temp/"foo 1.txt" # Set up test file with whitespace

# Test cp
echo "Testing cp..."
$BIN/cp temp/foo.txt temp/bar.txt >> log.txt
[[ $? == 0 ]] && diff -s temp/foo.txt temp/bar.txt >> log.txt && echo "PASSED" || echo "FAILED"
$BIN/cp >> log.txt
[[ $? == 1 ]] && echo "PASSED" || echo "FAILED"
$BIN/cp temp/foo.txt temp/bar2.txt notRealDir/notRealFile.txt >> log.txt
[[ $? == 1 ]] && echo "PASSED" || echo "FAILED"
$BIN/cp temp/"foo 1.txt" temp/bar.txt >> log.txt
[[ $? == 0 ]] && diff -s temp/"foo 1.txt" temp/bar.txt >> log.txt && echo "PASSED" || echo "FAILED"

# Test ls
echo "Testing ls..."
$BIN/ls no_exist_dir >> log.txt
[[ $? == 1 ]] && echo "PASSED" || echo "FAILED"
$BIN/ls ../no_exist_dir >> log.txt
[[ $? == 1 ]] && echo "PASSED" || echo "FAILED"
$BIN/ls this is alot of bad arguments for ls to process >> log.txt
[[ $? == 1 ]] && echo "PASSED" || echo "FAILED"
$BIN/ls temp >> log.txt
[[ $? == 0 ]] && echo "PASSED" || echo "FAILED"
$BIN/ls temp/temp2 >> log.txt
[[ $? == 0 ]] && echo "PASSED" || echo "FAILED"
$BIN/ls temp/notRealDir >> log.txt
[[ $? == 1 ]] && echo "PASSED" || echo "FAILED"

# Test mkdir
echo "Testing mkdir..."
$BIN/mkdir new_dir >> log.txt
[[ $? == 0 ]] && [ -d new_dir ] && echo "PASSED" || echo "FAILED"
$BIN/mkdir temp/test_dir >> log.txt
[[ $? == 0 ]] && [ -d temp/test_dir ] && echo "PASSED" || echo "FAILED"
$BIN/mkdir temp/test_dir >> log.txt
[[ $? == 1 ]] && [ -d temp/test_dir ] && echo "PASSED" || echo "FAILED"
$BIN/mkdir temp/dir1 temp/dir2 >> log.txt
[[ $? == 0 ]] && [ -d temp/dir1 ] && [ -d temp/dir2 ] && echo "PASSED" || echo "FAILED"
$BIN/mkdir temp/"test dir" >> log.txt
[[ $? == 0 ]] && [ -d temp/"test dir" ] && echo "PASSED" || echo "FAILED"
$BIN/mkdir >> log.txt
[[ $? == 1 ]] && echo "PASSED" || echo "FAILED"

# Test rmdir
echo "Testing rmdir..."
$BIN/rmdir new_dir >> log.txt
[[ $? == 0 ]] && ! [ -d new_dir ] && echo "PASSED" || echo "FAILED"
$BIN/rmdir temp/test_dir >> log.txt
[[ $? == 0 ]] && ! [ -d temp/test_dir ] && echo "PASSED" || echo "FAILED"
$BIN/rmdir temp/dir1 temp/dir2 >> log.txt
[[ $? == 0 ]] && ! [ -d temp/dir1 ] && ! [ -d temp/dir2 ] && echo "PASSED" || echo "FAILED"
$BIN/rmdir fake_dir >> log.txt
[[ $? == 1 ]] && ! [ -d fake_dir ] && echo "PASSED" || echo "FAILED"
$BIN/rmdir >> log.txt
[[ $? == 1 ]] && echo "PASSED" || echo "FAILED"
$BIN/rmdir temp/"test dir" >> log.txt
[[ $? == 0 ]] && ! [ -d temp/"test dir" ] && echo "PASSED" || echo "FAILED"
$BIN/rmdir ../fake_dir2 >> log.txt
[[ $? == 1 ]] && ! [ -d ../"fake_dir2" ] && echo "PASSED" || echo "FAILED"

# Test pwd
echo "Testing pwd..."
[[ $($BIN/pwd) == $PWD ]] && echo "PASSED" || echo "FAILED"
$BIN/pwd three extra arguments >> log.txt
[[ $? == 1 ]] && echo "PASSED" || echo "FAILED"

# Clean up
rm -r temp
rm foo.txt
rm bar.txt
rm bar2.txt
rm "foo 1.txt"
