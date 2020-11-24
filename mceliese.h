#include "linearcode.h"

typedef std::vector<size_t> permn;
typedef std::tuple<matrix, LinearCode, permn> McEliesePrivate;
typedef LinearCode McEliesePublic;

code_word applyPermutation(const code_word& cw,
                           const std::vector<size_t>& perm,
                           const bool rev);
std::vector<code_word> applyPermutation(const std::vector<code_word>& message,
                                        const std::vector<size_t>& perm,
                                        const bool rev);

permn random_permutation(const size_t width);

code_word unscramble_symbol(const code_word& ms, const matrix& C);
std::vector<code_word> unscramble(const std::vector<code_word>& ms,
                                  const matrix& C);

McEliesePrivate GenPrivateKey(const size_t words, const size_t bits);
McEliesePublic PrivateToPublic(const McEliesePrivate& privKey);
void SaveKeys(const McEliesePrivate& privKey,
              const McEliesePublic& pubKey,
              const std::string& dir_path);

McEliesePrivate ReadPrivateKey(const std::string& file_path);
McEliesePublic ReadPublicKey(const std::string& file_path);
std::vector<code_word> ReadCSV(const std::string& file_path,
                               const bool bin=false);

std::vector<code_word> McE_encypt_message(const McEliesePublic& pubKey,
                                          const std::vector<code_word>& message);

std::vector<code_word> McE_decypt_message(const McEliesePrivate& privKey,
                                          const std::vector<code_word>& message);