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
                plain |= (temp ? 1 : 0);
            }

            plain = reverse(plain, bits);
            scrambled.push_back(plain);
        }
    }
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

void SaveKeys(const McEliesePrivate& privKey,
              const McEliesePublic& pubKey,
              const std::string& dir_path)
{
    std::string public_key_dir = dir_path;
    public_key_dir.append("/");
    public_key_dir.append("public.mce");

    std::string private_key_dir = dir_path;
    private_key_dir.append("/");
    private_key_dir.append("private.mce");
    ////////////////////////////////////////////////////////////////////////////
    std::ofstream public_file;
    public_file.open(public_key_dir, std::ofstream::out | std::ofstream::trunc);

    public_file << pubKey.get_code_with() << std::endl;
    public_file << pubKey.get_max_error() << std::endl;

    const matrix R = pubKey.get_gen_mat();
    for (size_t i = 0; i < R.size(); ++i)
    {
       public_file << R[i] << (i + 1 == R.size() ? "\n" : ",");
    }
    public_file.close();

    ////////////////////////////////////////////////////////////////////////////
    std::ofstream private_file;
    private_file.open(private_key_dir, std::ofstream::out | std::ofstream::trunc);

    const LinearCode G = std::get<1>(privKey);
    const permn      P = std::get<2>(privKey);
    const matrix     M = G.get_extra_bits();

    private_file << G.get_code_with() << std::endl;

    for (size_t i = 0; i < M.size(); ++i)
    {
       private_file << M[i] << (i + 1 == M.size() ? "\n" : ",");
    }
    private_file << "|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+"
                << "|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|" << std::endl;
    
    for (size_t i = 0; i < P.size(); ++i)
    {
        const uint64_t c1 = std::get<0>(P[i]);
        const uint64_t c2 = std::get<1>(P[i]);

        private_file << c1 << "|" << c2
                    << (i + 1 == P.size() ? "\n" : ",");
    }

    private_file.close();
}

McEliesePrivate ReadPrivateKey(const std::string& file_path)
{
    std::string line;
    std::ifstream data_file(file_path);

    size_t w;
    matrix C;
    matrix G;
    permn  P;

    getline(data_file, line);
    w = std::stoi(line.substr(0, 10));

    getline(data_file, line);
    int from = 0;
    int comma = line.find(",", from);

    G.emplace_back(line.substr(from, comma - from));
    while (comma > 0)
    {
        from = comma + 1;
        comma = line.find(",", from);

        G.emplace_back(line.substr(from, comma - from));
    }

    LinearCode lc = LinearCode(G, w);
    getline(data_file, line); // skip line
    getline(data_file, line);

    from    = 0;
    int bar = line.find("|", from);
    comma   = line.find(",", from);
    int value1, value2;

    value1 = std::stoi(line.substr(from, bar - from));
    value2 = std::stoi(line.substr(bar+1, comma - bar + 1));
    permUnit p(value1, value2);
    P.push_back(p);
    while (comma > 0)
    {
        from = comma + 1;
        comma = line.find(",", from);
        bar = line.find("|", from);

        value1 = std::stoi(line.substr(from, bar - from));
        value2 = std::stoi(line.substr(bar+1, comma - bar + 1));

        permUnit p(value1, value2);
        P.push_back(p);
    }

    McEliesePrivate PrivateKey = {C, lc, P};

    return PrivateKey;
}

McEliesePublic ReadPublicKey(const std::string& file_path)
{
    std::string line;
    std::ifstream data_file(file_path);

    matrix G;

    getline(data_file, line);
    size_t w = std::stoi(line.substr(0, 10));
    getline(data_file, line);
    size_t errs = std::stoi(line.substr(0, 10));

    getline(data_file, line);
    int from = 0;
    int comma = line.find(",", from);
    G.emplace_back(line.substr(from, comma - from));
    while (comma > 0)
    {
        from = comma + 1;
        comma = line.find(",", from);

        G.emplace_back(line.substr(from, comma - from));
    }
    LinearCode lc(G, w);
 
    lc.set_generator(G);
    lc.set_max_errors(errs);

    return lc;
}

std::vector<code_word> ReadCSV(const std::string& file_path,
                               const bool bin)
{
    std::string line;
    std::ifstream data_file(file_path);

    std::vector<code_word> message;

    getline(data_file, line);
    int from = 0;
    int comma = line.find(",", from);
    int value;

    if (!bin)
    {
        value = std::stoi(line.substr(from, comma - from));
        message.push_back(value);
    }
    else
    {
       message.emplace_back(line.substr(from, comma - from)); 
    }
    while (comma > 0)
    {
        from = comma + 1;
        comma = line.find(",", from);

        if (!bin)
        {
            value = std::stoi(line.substr(from, comma - from));
            message.push_back(value);
        }
        else
        {
           message.emplace_back(line.substr(from, comma - from)); 
        }
    }

    return message;
}