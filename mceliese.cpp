#include "mceliese.h"

void applySwap(matrix& M, const permUnit& swp)
{
	const size_t c1 = std::get<0>(swp);
	const size_t c2 = std::get<1>(swp);

	swap_columns(M, c1, c2);

	return;
}

void applyPermn(matrix& M, const permn& perm)
{
	for (size_t i = 0; i < perm.size(); ++i)
		applySwap(M, perm[i]);

	return;
}

matrix PrivateToPublic(const McEliesePrivate& privKey)
{
	const matrix C =  std::get<0>(privKey);// not implemented yet!
	      matrix G =  std::get<1>(privKey);
	const permn  P =  std::get<2>(privKey);

	applyPermn(G, P);

	return G;
}