# cpp-mceliese
Boneheaded implementation of McEliese encryption in C++
## Compilation
```
make
```

## Testing
```
./full.sh
```

## Generate public and private keys
```
./mceliese -g dim extra_bits
```
where 
 * `dim` is the number of bits in each plaintext symbol
 * `extra_bits` is the number of extra checksum bits to allow error correction.
      
The encryption process will map a `dim`-bit symbol to a `dim + extra_bits`-bit symbol.

Currently we impose that `dim + extra_bits` < 128.
This can be changed by altering the value of `BITS` in `matrix.h`
(large values of `BITS` will cause memory problems when building the syndrome table).

Creates two files `private.mce` and `public.mce`.

## Encrypt data
```
./mceliese -e ./clear.txt public.mce >> encrypted.txt
```
where `public.mce` is the public key generated above.

## Decrypt data
```
./mceliese -d ./encrypted.txt private.mce >> plain.txt
```
where `private.ce` is the private key generated above.

## Create 'all' Orthognal matrices
```
./mceliese -o dim
```
Calculates all n-by-n orthogonal matrices upto permuation of rows and columns.
[Work in progress, overcounts at the moment]
