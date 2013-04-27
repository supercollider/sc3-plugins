#!/bin/bash

FILE_PATH=plugins_list.md

echo "" > $FILE_PATH

# find .schelp files, and use it to generate links to scdoc
for h in `find source -name "*.schelp" | sed -Ee "s/^.*\/(.*)\.schelp/\\1/g" -e '/-/d' | sort`
do
	echo "* [$h](http://doc.sccode.org/Classes/$h.html)" >> "$FILE_PATH"
done