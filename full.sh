if [ $# -gt 0 ]
then
  dim=$1
else
  dim=8
fi

if [ $# -gt 1 ]
then
  extra_bits=$2
else
  extra_bits=48
fi

make -B
echo "Compilation    OK!"
rm -f cipher.txt
rm -f decoded.txt

./mceliese -g $dim $extra_bits
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
