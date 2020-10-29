#include "linearcode.h"

typedef std::tuple<size_t, size_t> permUnit;
typedef std::vector<permUnit> permn;
typedef std::tuple<matrix, matrix, permn> McEliesePrivate;
typedef std::tuple<matrix, unsigned int> McEliesePublic;

void applySwap(matrix& M, const permUnit& swp);
void applyPermn(matrix& M, const permn& perm);

matrix PrivateToPublic(const McEliesePrivate& privKey);
void SaveKeys(const McEliesePrivate& privKey, const McEliesePublic& pubKey);

code_word McE_encypt_symbol(const McEliesePublic& pubKey, const code_word wd);

code_word McE_decypt_symbol(const McEliesePrivate& privKey, const code_word wd);