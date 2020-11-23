#include "mceliese.h"

code_word unscramble_symbol(const code_word ms, const matrix& C)
{
    code_word sym = 0;

    matrix C_trans = transpose(C, C.size());

    for (size_t i = 0; i < C_trans.size(); ++i)
    {
        size_t b = row_dot(C_trans[i], ms);
        sym.set(i, 1 == b);
    }
    sym = reverse(sym, C.size());
    return sym;
}

std::vector<code_word> unscramble(const std::vector<code_word>& ms,
                                  const matrix& C)
{
    std::vector<code_word> uns;
    for (size_t i = 0; i < ms.size(); ++i)
        uns.push_back(unscramble_symbol(ms[i], C));

    return uns;
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
    matrix C = find(words);

    matrix bs = find(words, bits);

    LinearCode G = LinearCode(bs, bits);
    const uint64_t width = words + bits;

    // It looks like we need ~n^2 swaps to get a 'random' element of Sn
    // https://cstheory.stackexchange.com/questions/5321/
    permn P = RandomPermutation(width, width * width);

    McEliesePrivate PriKey = {C, G, P};

    return PriKey;
}

McEliesePublic PrivateToPublic(const McEliesePrivate& privKey)
{
    const matrix C =  std::get<0>(privKey);// not implemented yet!
    LinearCode   G =  std::get<1>(privKey);
    const permn  P =  std::get<2>(privKey);

    matrix G_mat = G.get_gen_mat();
    G_mat = multiply(C, C.size(), G_mat, 64);
    G.set_generator(G_mat);
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
    const matrix     C =  std::get<0>(privKey);
    const LinearCode G =  std::get<1>(privKey);
    const permn      P =  std::get<2>(privKey);

    std::vector<code_word> ms = message;
    applyPermn(ms, P, true);
    ms = G.decode_message(ms);

    const matrix C_inv = invert(C);
    ms = unscramble(ms, C_inv);

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

    const matrix     C =  std::get<0>(privKey);
    const LinearCode G = std::get<1>(privKey);
    const permn      P = std::get<2>(privKey);
    const matrix     M = G.get_extra_bits();

    private_file << G.get_code_with() << std::endl;

    private_file << "|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+"
                 << "|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|" << std::endl;

    for (size_t i = 0; i < C.size(); ++i)
    {
       private_file << C[i] << (i + 1 == C.size() ? "\n" : ",");
    }
    private_file << "|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+"
                 << "|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|" << std::endl;

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

    getline(data_file, line); // skip line
    getline(data_file, line);
    ////////////////////////////////////////////////////////////////////////////
    int from = 0;
    int comma = line.find(",", from);

    C.emplace_back(line.substr(from, comma - from));
    while (comma > 0)
    {
        from = comma + 1;
        comma = line.find(",", from);

        C.emplace_back(line.substr(from, comma - from));
    }
    ////////////////////////////////////////////////////////////////////////////
    getline(data_file, line); // skip line
    getline(data_file, line);

    from = 0;
    comma = line.find(",", from);

    G.emplace_back(line.substr(from, comma - from));
    while (comma > 0)
    {
        from = comma + 1;
        comma = line.find(",", from);

        G.emplace_back(line.substr(from, comma - from));
    }
    ////////////////////////////////////////////////////////////////////////////
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