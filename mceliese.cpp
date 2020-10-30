#include "mceliese.h"

void applySwap(LinearCode& lc, const permUnit& swp)
{
	const uint64_t c1 = std::get<0>(swp);
	const uint64_t c2 = std::get<1>(swp);

	lc.swapColumns(c1, c2);

	return;
}

void applyPermn(LinearCode& lc, const permn& perm, const bool reverse)
{
	if (reverse)
	{
		for (uint64_t i = perm.size(); i > 0; --i)
			applySwap(lc, perm[i-1]);
	}
	else
	{
		for (uint64_t i = 0; i < perm.size(); ++i)
			applySwap(lc, perm[i]);		
	}

	lc.set_decode_flag(false);

	return;
}

permn RandomPermutation(const uint64_t width, const uint64_t upto)
{
	permn ps;
	while (ps.size() < upto)
	{
		const uint64_t c1 = rand() % (1 << (2*width));
		const uint64_t c2 = rand() % (1 << (2*width));

		if (c1 == c2)
			continue;

		permUnit p(c1,c2);
		ps.push_back(p);
	}
	return ps;
}

void PrintPermutation(const permn& ps)
{
	for (uint64_t i = 0; i < ps.size(); ++i)
	{
		const permUnit p = ps[i];
		const uint64_t c1 = std::get<0>(p);
		const uint64_t c2 = std::get<1>(p);
		std::cout << "(" << c1 << "," << c2 << ")";
	}
	std::cout << std::endl;
}

LinearCode PrivateToPublic(const McEliesePrivate& privKey)
{
	const matrix C =  std::get<0>(privKey);// not implemented yet!
	LinearCode   G =  std::get<1>(privKey);
	const permn  P =  std::get<2>(privKey);

	applyPermn(G, P);

	return G;
}