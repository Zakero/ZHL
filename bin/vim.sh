#!/bin/bash
###############################################################################
# What Is It?
#
# This script will start Vim in a new window and load all the source code files
# into it.
#
###############################################################################
# Linux/KDE
#
# Additional setup is required for smooth integration with KDE and Konsole.
# 
# 1 - Start a KDE Konsole
# 2 - Create a new Profile
#     2.1 - Ctrl+Shift+M to toggle the menu if needed
#     2.2 - From the menu: "Settings -> Manage Profiles..."
#     2.3 - Click on the "+ New..." button
#     2.4 - Set the Profile name to "VIM"
#     2.5 - Adjust the configuration to you prefernce
#           - Recommend setting "Scrolling -> Scrollback" to "None"
# 
###############################################################################

readonly kernal_name="$(uname -s)"

if [ "${kernal_name}" = "Linux" ]
then
	readonly project_dir="$(fossil info | grep local-root | sed 's/.*:  *\(.*\)/\1/')"
	readonly project_name="$(fossil info | grep project-name | sed 's/.*:  *\(.*\)/\1/')"

	#readonly vim_command="konsole --separate --profile VIM -p 'TerminalColumns=274' -p 'TerminalRows=61' -e vim"
	readonly vim_command="konsole --separate --profile VIM -e vim"
	readonly vim_server="--servername ${project_name}"

	cd ${project_dir}
elif [ "${kernal_name}" = "Haiku" ]
then
	readonly vim_command="gvim"
	readonly vim_server=""

	cd /ZHL
fi

readonly vim_files="""
	ReadMe.md
	include/*
	"""

${vim_command} \
	-c 'set exrc | vsplit | vsplit' \
	${vim_server} \
	$(echo ${vim_files}) \
	&>/dev/null \
	&
