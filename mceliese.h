#include "linearcode.h"

typedef std::tuple<uint64_t, uint64_t> permUnit;
typedef std::vector<permUnit> permn;
typedef std::tuple<matrix, LinearCode, permn> McEliesePrivate;
typedef LinearCode McEliesePublic;

void hackLinearComb(LinearCode& lc, const bool reverse=false);
std::vector<code_word> hackLinearComb(const std::vector<code_word>& message,
	                                    const size_t bits,
	                                    const bool reverse=false);

void applySwap(LinearCode& lc, const permUnit& swp);
void applyPermn(LinearCode& lc, const permn& perm, const bool reverse=false);

void applySwap(code_word& message, const permn& perm);
void applyPermn(std::vector<code_word>& message, const permn& perm,
	              const bool reverse=false);

permn RandomPermutation(const uint64_t width, const uint64_t upto);
void PrintPermutation(const permn& ps);

McEliesePrivate GenPrivateKey(const uint64_t words, const uint64_t bits);
McEliesePublic PrivateToPublic(const McEliesePrivate& privKey);
void SaveKeys(const McEliesePrivate& privKey,
	            const McEliesePublic& pubKey,
	            const std::string& dir_path);

McEliesePrivate ReadPrivateKey(const std::string& file_path);
McEliesePublic ReadPublicKey(const std::string& file_path);
std::vector<code_word> ReadCSV(const std::string& file_path);

std::vector<code_word> McE_encypt_message(const McEliesePublic& pubKey,
                                          const std::vector<code_word>& message);

std::vector<code_word> McE_decypt_message(const McEliesePrivate& privKey,
	                                        const std::vector<code_word>& message);