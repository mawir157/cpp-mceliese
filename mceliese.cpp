#include "mceliese.h"

void applySwap(LinearCode& lc, const permUnit& swp)
{
    const uint64_t c1 = std::get<0>(swp);
    const uint64_t c2 = std::get<1>(swp);

    lc.swapColumns(c1, c2);

    return;
}

void applySwap(code_word& message, const permn& perm)
{

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

void applyPermn(std::vector<code_word>& message, const permn& ps,
                const bool reverse)
{
    if (reverse)
    {
        for (uint64_t i = ps.size(); i > 0; --i)
        {
            const permUnit swp = ps[i-1];
            const uint64_t c1 = std::get<0>(swp);
            const uint64_t c2 = std::get<1>(swp);
            swap_columns(message, c1, c2);
        }
    }
    else
    {
        for (uint64_t i = 0; i < ps.size(); ++i)
        {
            const permUnit swp = ps[i];
            const uint64_t c1 = std::get<0>(swp);
            const uint64_t c2 = std::get<1>(swp);
            swap_columns(message, c1, c2);
        }
    }
    return;
}

permn RandomPermutation(const uint64_t width, const uint64_t upto)
{
    permn ps;
    while (ps.size() < upto)
    {
        const uint64_t c1 = rand() % width;
        const uint64_t c2 = rand() % width;

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

McEliesePrivate GenPrivateKey(const uint64_t words, const uint64_t bits)
{
    matrix C;

    matrix bs = find(words, bits);
    LinearCode G = LinearCode(bs, bits);

    const uint64_t width = words + bits;

    // permn P = RandomPermutation(width, 2 * width);
    permn P = RandomPermutation(width, 1);

    McEliesePrivate PriKey = {C, G, P};

    return PriKey;
}

McEliesePublic PrivateToPublic(const McEliesePrivate& privKey)
{
    const matrix C =  std::get<0>(privKey);// not implemented yet!
    LinearCode   G =  std::get<1>(privKey);
    const permn  P =  std::get<2>(privKey);

    // applyPermn(G, P);

    return G;
}

std::vector<code_word> McE_encypt_message(const McEliesePublic& pubKey,
                                          const std::vector<code_word>& message)
{
    return pubKey.encode_message(message);
}

std::vector<code_word> McE_decypt_message(const McEliesePrivate& privKey,
                                          const std::vector<code_word>& message)
{
    const LinearCode G =  std::get<1>(privKey);
    const permn      P =  std::get<2>(privKey);

    std::vector<code_word> ms = message;
    // applyPermn(ms, P, true);

    return G.decode_message(ms);
}
