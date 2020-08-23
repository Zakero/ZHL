#!/bin/bash

wayland_data="$$.wayland-client-protocol.h"
project_data="$$.Zakero_Yetani.h"

grep "WL_SHM_FORMAT_.* = " /usr/include/wayland/wayland-client-protocol.h \
	| sed "s/.*\(WL.*\) = .*/\1/" \
	| sort \
	> $wayland_data

grep "X(WL_SHM_FORMAT_" ./include/Zakero_Yetani.h \
	| sed "s/.*\(WL_SHM_FORMAT_[^ ]*\).*/\1/" \
	| sort \
	> $project_data

diff --side-by-side --suppress-common-lines $wayland_data $project_data

rm $wayland_data $project_data
