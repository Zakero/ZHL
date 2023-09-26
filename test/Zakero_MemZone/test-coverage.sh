#!/bin/bash

rm -rf test-coverage
mkdir test-coverage

g++ \
	-std=c++23 \
	-O0 \
	-Wall \
	-Werror \
	-Wpedantic \
	-coverage \
	-g \
	-o test-coverage/Zakero_MemZone \
	Zakero_MemZone.cpp

(cd test-coverage; ./Zakero_MemZone)

lcov \
	--capture \
	--directory test-coverage \
	--exclude "*/atomic" \
	--exclude "*/bit" \
	--exclude "*/bits/*" \
	--exclude "*/cmath" \
	--exclude "*/compare" \
	--exclude "*/doctest.h" \
	--exclude "*/ext/*" \
	--exclude "*/iomanip" \
	--exclude "*/limits" \
	--exclude "*/new" \
	--exclude "*/stdexcept" \
	--exclude "*/type_traits" \
	--list-full-path \
	--output-file test-coverage/Zakero_MemZone.lcov \
	--test-name "Zakero_MemZone" \

genhtml \
	--dark-mode \
	--demangle-cpp \
	--frames \
	--legend \
	--output-directory test-coverage \
	test-coverage/Zakero_MemZone.lcov \
