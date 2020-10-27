#!/bin/bash
# Run the shell test input
mkdir temp
../soyshell < shell_test.txt 1> log.txt 2>> log.txt
# Verify the results
echo "Testing user defined constants..."
[ -d temp/emacs ] && echo "PASSED" || echo "FAILED"
echo "Testing && conditional..."
[ -d temp/and_test1 ] && [ -d temp/and_test2 ] && ! [ -d temp/and_test3 ] && echo "PASSED" || echo "FAILED"
echo "Testing || conditional..."
[ -d temp/or_test1 ] && ! [ -d temp/or_test2 ] && [ -d temp/or_test3 ] && echo "PASSED" || echo "FAILED"
echo "Testing braces..."
[ -d temp/brace_test1 ] && [ -d temp/brace_test2 ] && ! [ -d temp/brace_test3 ] && [ -d temp/brace_test4 ] && echo "PASSED" || echo "FAILED"
echo "Testing quotes..."
[ -d temp/dir\ with\ space ] && echo "PASSED" || echo "FAILED"
# Cleanup
rm -r temp
