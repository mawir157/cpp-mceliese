make -B
rm cipher.txt
./mceliese -g 10 30
./mceliese -e test.txt public.mce >> cipher.txt

echo "Plaintext"
cat test.txt

echo
echo "Decoded text"
./mceliese -d cipher.txt private.mce

