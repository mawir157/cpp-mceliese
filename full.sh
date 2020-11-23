make -B
echo "Compilation    OK!"
rm -f cipher.txt
rm -f decoded.txt
./mceliese -g 8 48
echo "Key generation OK!"
./mceliese -e test.txt public.mce >> cipher.txt
echo "Encryption     OK!"
./mceliese -d cipher.txt private.mce >> decoded.txt
echo "Decryption     OK!"

DIFF=$(diff test.txt decoded.txt) 
if [ "$DIFF" != "" ] 
then
  echo "There was an error:"
  echo "Plaintext"
	cat test.txt
	echo
  echo "Decoded text"
	cat decoded.txt
else
  echo "Completed encryption/decryption circuit with no errors. :)"
fi