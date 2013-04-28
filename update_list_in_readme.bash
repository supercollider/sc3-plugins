#!/bin/bash

READMEPATH=README.md
TEMPPATH=README.md.tmp

# take a copy of the readme, with any generated links filtered back out
cat "$READMEPATH" | sed -Ee "/^\* \[\w+\]\(.*\)$/d" > "$TEMPPATH"

# find .schelp files, and use it to generate links to scdoc
#for f in `find source/*/sc -name "*.schelp" | sort | sed s:..\/source\/::`
for h in `find source -name "*.schelp" | sed -Ee "s/^.*\/(.*)\.schelp/\\1/g" -e '/-/d' | sort`
do
	echo "* [$h](http://doc.sccode.org/Classes/$h.html)" >> "$TEMPPATH"
done

# copy the finished readme back into place
mv -f "$TEMPPATH" "$READMEPATH"