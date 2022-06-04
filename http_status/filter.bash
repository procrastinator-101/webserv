#!/bin/bash

#filter lines that don't have status codes
sed -n '/^[0-9][0-9][0-9]/p' $1 | sed 's/(.*)//g' > tmp0.txt

#get status code
cat tmp0.txt | cut -f 1 -d ' ' > tmp1.txt

#get status msg
cat tmp0.txt | cut -f 2- -d ' ' > tmp2.txt

#merge the two
paste -d : tmp1.txt tmp2.txt > tmp3.txt

#set them to map insert
cat tmp3.txt | sed 's/\([0-9].*\):\(.*\)/_library.insert(std::make_pair(\1, "\2"));/g' > filemerge.txt


#cleanup
rm -rf tmp0.txt tmp1.txt tmp2.txt tmp3.txt