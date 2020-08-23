#!/bin/bash
###############################################################################
# 
# This script will convert Wayland Protocol XML files into C code using the
# "wayland-scanner" utility.  The generated code is then cleaned and inserted
# into the "include/Zakero_Yetani.h" file.
# 
# Requirements:
# - "wayland-scanner" must be in the PATH
# - Wayland Protocol XML files must be in the "wayland-protocol" directory
# 
# On OpenSuSE, the Wayland Protocol XML files can be found in:
# 	/usr/share/wayland-protocols/
# 
###############################################################################
# 
# Exit Error Codes:
# 
# The exit error codes are based on the /usr/include/sysexits.h for all
# non-zero errors.  An exit code of '0' is success.
#
###############################################################################
# 
# Bugs:
# - Nothing to admit
# 
###############################################################################
# 
# Todo:
# - Add an option to remove a protocol from "include/Zakero_Yetani.h"
# - Add more Todo items
# 
###############################################################################

###############################################################################
# Varibables: Read-Only

########################################
# Error Codes
readonly Error_Data=65        # EX_DATAERR
readonly Error_NoInput=66     # EX_NOINPUT
readonly Error_TempFail=75    # EX_TEMPFAIL
readonly Error_Unavailable=69 # EX_UNAVAILABLE
readonly Error_Usage=64       # EX_USAGE
readonly Error_NoPerm=77      # EX_NOPERM

########################################
# Page Viewer
if [ -n "`command -v less 2> /dev/null`" ]
then
	readonly page_viewer="less --no-init --quit-if-one-screen"
elif [ -n "`command -v more 2> /dev/null`" ]
then
	readonly page_viewer="more"
else
	readonly page_viewer=""
fi

readonly program_name=`basename $0`

cd `dirname $0`/..
readonly project_dir=`pwd`
cd - &> /dev/null

readonly header_library="$project_dir/include/Zakero_Yetani.h"

readonly wayland_dir="/usr/share/wayland-protocols"

###############################################################################
# Variables: Internal


###############################################################################
# Varibables: Arg Configurable

action="add"
file_list=""


###############################################################################
# External Checks

command_list="\
	basename \
	cat \
	cut \
	gcc \
	grep \
	rm \
	sed \
	tail \
	wayland-scanner \
	"

for cmd in $command_list
do
	if [ -z "`command -v $cmd 2> /dev/null`" ]
	then
		echo "Error: Command not found: $cmd"
		exit $Error_Unavailable
	fi
done

touch ./.$$ &> /dev/null
if [ $? -ne 0 ]
then
	echo "Error: No permission to create temporary files"
	exit $Error_NoPerm
fi
rm ./.$$

if [ ! -d "$project_dir/wayland-protocol" ]
then
	echo "Error: Can't find directory: 'wayland-protocol'"
	exit $Error_Unavailable
fi


###############################################################################
main()
{
	if [ "$action" == "add" ]
	then
		protocol_add $file_list

	elif [ "$action" == "remove" ]
	then
		protocol_remove $file_list

	elif [ "$action" == "diff" ]
	then
		protocol_diff $file_list

	elif [ "$action" == "header" ]
	then
		protocol_header $file_list
	fi
}


###############################################################################
protocol_add()
{
	for xml in $*
	do
		protocol=`basename --suffix=".xml" $xml`

		file="$$.$protocol"
		file_c="$$.c"
		file_h="$$.h"

		fold_start="// {{{ $protocol"
		fold_end="// }}}"

		# Generate Code
		wayland-scanner client-header $xml $file_h
		wayland-scanner public-code   $xml $file_c
		
		# Create consolidated code
		echo "$fold_start" > $file

		sed "/^#include /d" $file_h \
			| gcc -E -xc - \
			| sed '/^$/d' \
			| sed '/^# /d' \
			>> $file

		sed "/^#include /d" $file_c \
			| gcc -E -xc - \
			| sed 's/WL_EXPORT //g' \
			| sed '/^$/d' \
			| sed '/^# /d' \
			>> $file

		echo "$fold_end" >> $file

		protocol_remove $protocol

		# Insert Protocol Code
		sed -i -e "/\/\/ {{{ Generated Code/r$file" $header_library

		# Remove temporary files
		rm $file $file_c $file_h
	done
}


###############################################################################
protocol_remove()
{
	protocol_list=""

	for file in $file_list
	do
		protocol_list="$protocol_list `basename --suffix=.xml $file`"
	done

	for protocol in $protocol_list
	do
		fold_start="// {{{ $protocol"
		fold_end="// }}}"

		tmp=`grep -n "$fold_start" $header_library`
		if [ $? -eq 0 ]
		then
			line_start=`echo $tmp | cut -d: -f 1`

			tmp=`tail --lines=+$line_start $header_library \
				| grep -n --max-count=1 "$fold_end" \
				| cut -d: -f 1 \
				`
			line_end=$(( line_start + tmp - 1 ))

			sed -i -e "${line_start},${line_end}d" $header_library
		fi
	done
}


###############################################################################
protocol_diff()
{
	local_list=""
	protocol_list=""

	for file in $file_list
	do
		if [ "${file: -4}" != ".xml" ]
		then
			continue
		fi

		protocol=`basename --suffix=".xml" $file`
		protocol_file=$(basename "$file")
		remote_file=$(find $wayland_dir -name "$protocol_file")

		echo "$protocol"
		if [ -n "$remote_file" ]
		then
			d=$(diff $file $remote_file)
			if [ -z "$d" ]
			then
				echo "- No differences found"
			else
				echo "- Differences found"
			fi
		fi

		protocol_stable=$(echo $protocol_file | sed "s/-unstable-.*\./\./")
		if [ "$protocol_file" != "$protocol_stable" ]
		then
			found=$(find $wayland_dir -name "$protocol_stable")
			if [ -n "$found" ]
			then
				echo "- Stable version available: $found"
			fi
		fi

		echo ""
	done
}


###############################################################################
protocol_header()
{
	for xml in $*
	do
		file=`basename --suffix=".xml" $xml`

		wayland-scanner client-header $xml $file.h
	done
}


###############################################################################
show_usage()
{
	if [ -n "$page_viewer" ]
	then
		usage | $page_viewer
	else
		usage
	fi
}


###############################################################################
usage()
{
	cat <<- USAGE_END
	Usage: $program_name [Options] FILE
	
	Options:
	  -a, --add
	    Add the Wayland Protocol in FILE to the header library.
	    This is the default action if no actions are specified.

	  -r, --remove
	    Remove the Wayland Protocol in FILE from the header library.

	  -d, --diff
	    Compare the current Wayland Protocols with what is in the OS.

	  --header-only
	    Generate a stand-alone header file.  Wayland-Protocol Header files
	    are usually the best place to read about the protocol's API.

	  -h, --help
	    Show this message and exit.

	Arguments:
	  FILE
	    The name of the file to use from the "wayland-protocol" directory.
	    Multiple files may be given.

	USAGE_END
}


###############################################################################
# Parse the args
while [ "$1" != "" ]
do
	case $1 in
		-a|--add)
			action="add"
			;;
		-r|--remove)
			action="remove"
			;;
		-d|--diff)
			action="diff"
			;;
		--header-only)
			action="header"
			;;
		-h|--help)
			show_usage
			exit
			;;
		-*)
			echo "Error: Option \"$1\" is not supported."
			echo "Use \"-h\" for help:"
			echo "  $program_name -h"
			exit $Error_Usage
			;;
		*)
			file="$project_dir/wayland-protocol/$1"
			if [ ! -f "$file" ]
			then
				echo "Error: Can't find file: $file"
				exit $Error_NoInput
			fi
			file_list="$file_list $file"
			;;
	esac
	shift
done


###############################################################################
# Make sure that all required information is present
if [ -z "$file_list" ]
then
	echo "Error: No file name was provided."
	echo "Use \"-h\" for help:"
	echo "	$program_name -h"
	exit $Error_NoInput
fi

main
exit
