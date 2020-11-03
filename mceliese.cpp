#include "mceliese.h"

// doing this properly is v difficult I will come back a do properly later
std::vector<code_word> hackLinearComb(const std::vector<code_word>& M,
                                      const size_t bits, const bool rev)
{
    matrix scrambled;

    if (!rev)
    {
        scrambled.push_back(M[0]);
        for (size_t i = 1; i < M.size(); ++i)
            scrambled.push_back(row_add(M[i-1], M[i]));
    }
    else
    {
        for (size_t i = 0; i < M.size(); ++i)
        {
            code_word cipher = M[i];
            bool temp = get_bit(cipher, 0);
            code_word plain = (temp ? 1 : 0);

            for (size_t j = 1; j < bits; ++j)
            {
                plain <<= 1;
                temp ^= get_bit(cipher, j);
                plain += (temp ? 1 : 0);
            }

            plain = reverse(plain, bits);
            scrambled.push_back(plain);
        }
    }
    std::cout << std::endl;
    return scrambled;
}

void hackLinearComb(LinearCode& lc, const bool rev)
{
    matrix scrambled = hackLinearComb(lc.get_gen_mat(), rev);
    for (size_t i = 0; i < scrambled.size(); ++i)
        lc.replaceRow(i, scrambled[i]);

    return;
}

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

void applyPermn(LinearCode& lc, const permn& perm, const bool rev)
{
    if (rev)
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
                const bool rev)
{
    if (rev)
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
    matrix C/* = find(words, words)*/;

    matrix bs = find(words, bits);

    LinearCode G = LinearCode(bs, bits);
    const uint64_t width = words + bits;

    permn P = RandomPermutation(width, 8 * width + 1);

    McEliesePrivate PriKey = {C, G, P};

    return PriKey;
}

McEliesePublic PrivateToPublic(const McEliesePrivate& privKey)
{
    const matrix C =  std::get<0>(privKey);// not implemented yet!
    LinearCode   G =  std::get<1>(privKey);
    const permn  P =  std::get<2>(privKey);

    hackLinearComb(G, -1);
    applyPermn(G, P);
    return G;
}

std::vector<code_word> McE_encypt_message(const McEliesePublic& pubKey,
                                          const std::vector<code_word>& message)
{
    std::vector<code_word> message_w_errs;
    for (size_t i = 0; i < message.size(); ++i)
    {
        message_w_errs.push_back(
            add_upto_n_error(message[i],
                             pubKey.code_word_size() + pubKey.get_code_with(),
                             pubKey.get_max_error())
            );
    }

    return pubKey.encode_message(message);
}

std::vector<code_word> McE_decypt_message(const McEliesePrivate& privKey,
                                          const std::vector<code_word>& message)
{
    const LinearCode G =  std::get<1>(privKey);
    const permn      P =  std::get<2>(privKey);

    std::vector<code_word> ms = message;
    applyPermn(ms, P, true);
    ms = G.decode_message(ms);
    ms = hackLinearComb(ms, G.code_word_size(), true);

    return ms;
}
