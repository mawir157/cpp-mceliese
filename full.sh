make -B
rm -f cipher.txt
rm -f decoded.txt
./mceliese -g 8 56
./mceliese -e test.txt public.mce >> cipher.txt
./mceliese -d cipher.txt private.mce >> decoded.txt

diff test.txt decoded.txt
DIFF=$(diff test.txt decoded.txt) 
if [ "$DIFF" != "" ] 
then
  echo "There was an error:"
  echo "Plaintext"
	cat test.txt
	echo
  echo "Decoded text"
	cat decoded.txt
  echo $DIFF
else
  echo "Completed encryption/decrptyion circuit with no errors. :)"
fi