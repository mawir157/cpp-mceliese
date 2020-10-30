#include "linearcode.h"

typedef std::tuple<size_t, size_t> permUnit;
typedef std::vector<permUnit> permn;
typedef std::tuple<matrix, LinearCode, permn> McEliesePrivate;
typedef LinearCode McEliesePublic;

void applySwap(LinearCode& lc, const permUnit& swp);
void applyPermn(LinearCode& lc, const permn& perm, const bool reverse=false);

permn RandomPermutation(const size_t width, const size_t upto);
void PrintPermutation(const permn& ps);

LinearCode PrivateToPublic(const McEliesePrivate& privKey);
void SaveKeys(const McEliesePrivate& privKey, const McEliesePublic& pubKey);

code_word McE_encypt_symbol(const McEliesePublic& pubKey, const code_word wd);

code_word McE_decypt_symbol(const McEliesePrivate& privKey, const code_word wd);