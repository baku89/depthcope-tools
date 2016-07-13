#!/bin/bash
#
# In Dragonframe, go to the Advanced tab of Preferences
# and choose this file for Action Script.
#
# Example script showing input arguments
#  arguments should be obvious, except "Action"
#  which can be one of the following:
#
# "POSITION" is sent whenever Dragonframe moves to a new frame.
#   This is probably what you want to use. After shooting 
#   frame 10, when Dragonframe is ready to capture frame 11, it
#   sends "POSITION 11".
#
# "SHOOT" happens immediately before shooting.
#
# "DELETE" happens immediately before deleting.
#
# "CC" is capture complete. There is an additional argument,
#   the file name of the main downloaded image.
#
# "FC" is frame complete. There is an additional argument,
#   the file name of the main downloaded image.
#
# "TEST" is test shot complete. There is an additional argument,
#   the file name of the main downloaded image.
#
# "EDIT" means a timeline edit (cut/copy/paste frames) is
#   complete.
#
# "CONFORM" means a conform edits is complete.
#

echo "Dragonframe Script"

echo "Production : $1" >> ~/dragonframe_script_log.txt
echo "Scene      : $2" >> ~/dragonframe_script_log.txt
echo "Take       : $3" >> ~/dragonframe_script_log.txt
echo "Action     : $4" >> ~/dragonframe_script_log.txt
echo "Frame      : $5" >> ~/dragonframe_script_log.txt
echo "Exposure   : $6" >> ~/dragonframe_script_log.txt
echo "Exp. Name  : $7" >> ~/dragonframe_script_log.txt
echo "Filename   : $8" >> ~/dragonframe_script_log.txt

if [ "$4" == "POSITION" ]
then

echo "Move to frame $5" >> ~/dragonframe_script_log.txt

fi

if [ "$4" == "CC" ]
then

echo "CC $5-$6 -> $8" >> ~/dragonframe_script_log.txt

fi

if [ "$4" == "FC" ]
then

echo "FC $5-$6" >> ~/dragonframe_script_log.txt

fi

if [ "$4" == "EDIT" ]
then

echo "EDIT $5-$6" >> ~/dragonframe_script_log.txt

fi


