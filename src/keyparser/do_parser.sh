#!/bin/bash
#
# Run this script to generate keys.h from keyparser.in
#
# You should check key names and fix some errors by hand
# (for example Key_Www -> Key_WWW, Key_Leftctrl -> Key_LeftCtrl)
#

capitalize()
{
	len=`echo $1 | wc -c`
	word="$1"
	begin=1
	for((i=0;i<len;i++)) {
		if [ "${word:$i:1}" == "_" ]; then
			echo -n "_"
			begin=1
		else
			if [ $begin -eq 1 ]; then
				begin=0
				echo -n ${word:$i:1}
			else
				echo -n `echo -n ${word:$i:1} | tr [A-Z] [a-z]`
			fi
		fi
	}
}

KEYS=`cat keyparser.in | sort`

echo -en "#ifndef __KEYS_H__\n#define __KEYS_H__\n\n"
echo -en "#include <linux/input.h>\n\n"


echo -n "static int key_values[] = {"

first=1
for i in $KEYS; do
	if [ $first -eq 1 ]; then
		echo -e "$i,"
		first=0
	else
		echo -e "                    $i,"
	fi
done
echo -e "};\n"

first=1
echo -n "static char *key_names[] = {"
for i in $KEYS; do
	if [ $first -eq 1 ]; then
		echo -n "\""
		first=0
	else
		echo -n "                     \""
	fi
	echo -n `capitalize $i`
	echo "\","
done
echo -e "};\n"

echo -n "static const int key_count = "
echo -n `cat keyparser.in | wc -l`
echo ";"

echo -e "\n#endif /* __KEYS_H__ */"
