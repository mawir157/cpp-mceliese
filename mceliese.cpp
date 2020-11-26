#include "mceliese.h"

code_word unscramble_symbol(const code_word& ms, const matrix& C)
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

code_word applyPermutation(const code_word& cw,
               const std::vector<size_t>& perm,
               const bool rev)
{
  //permutation looks like
  // 0123456789
  // 2435019867
  code_word permutated;
  if (rev)
  {
    for (size_t i = 0; i < perm.size(); ++i)
      permutated.set(perm[i], cw[i]);
  }
  else
  {
    for (size_t i = 0; i < perm.size(); ++i)
      permutated.set(i, cw[perm[i]]);
  } 
  return permutated; 
}

std::vector<code_word> applyPermutation(const std::vector<code_word>& message,
                    const std::vector<size_t>& perm,
                    const bool rev)
{
  std::vector<code_word> permutated;
  for (size_t i = 0; i < message.size(); ++i)
    permutated.push_back(applyPermutation(message[i], perm, rev));

  return permutated;
}

permn random_permutation(const size_t width)
{
  std::vector<size_t> digits;
  for (size_t i = 0; i <  width; ++i)
    digits.push_back(i);

  std::random_shuffle(digits.begin(), digits.end());

  return digits;
}

McEliesePrivate GenPrivateKey(const size_t words, const size_t bits)
{
  matrix C = find(words);

  matrix bs = find(words, bits);

  LinearCode G = LinearCode(bs, bits);
  const size_t width = words + bits;

  permn P = random_permutation(width);

  McEliesePrivate PriKey = {C, G, P};

  return PriKey;
}

McEliesePublic PrivateToPublic(const McEliesePrivate& privKey)
{
  const matrix C =  std::get<0>(privKey);// not implemented yet!
  LinearCode   G =  std::get<1>(privKey);
  const permn  P =  std::get<2>(privKey);

  matrix G_mat = G.get_gen_mat();
  G_mat = multiply(C,   C.size(),
           G_mat, G.get_code_with() + G.code_word_size());
  G_mat = applyPermutation(G_mat, P, false);
  G.set_generator(G_mat);
  return G;
}

std::vector<code_word> McE_encypt_message(const McEliesePublic& pubKey,
                      const std::vector<code_word>& message)
{
  // encrypt message...
  std::vector<code_word> encoded_message = pubKey.encode_message(message);

  // then add errors
  std::vector<code_word> message_w_errs;

  for (size_t i = 0; i < encoded_message.size(); ++i)
  {
    message_w_errs.push_back(
      add_upto_n_error(encoded_message[i],
               pubKey.code_word_size() + pubKey.get_code_with(),
               pubKey.get_max_error())
      );
  }

  return message_w_errs;
}

std::vector<code_word> McE_decypt_message(const McEliesePrivate& privKey,
                      const std::vector<code_word>& message)
{
  const matrix   C =  std::get<0>(privKey);
  const LinearCode G =  std::get<1>(privKey);
  const permn    P =  std::get<2>(privKey);

  std::vector<code_word> ms = message;
  ms = applyPermutation(ms, P, true);
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

  const matrix   C =  std::get<0>(privKey);
  const LinearCode G = std::get<1>(privKey);
  const permn    P = std::get<2>(privKey);
  const matrix   M = G.get_extra_bits();

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
    private_file << P[i] << ((i + 1 == P.size()) ? "\n" : ",");

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

  from = 0;
  comma = line.find(",", from);

  P.push_back(std::stoi(line.substr(from, comma - from)));
  while (comma > 0)
  {
    from = comma + 1;
    comma = line.find(",", from);

    P.push_back(std::stoi(line.substr(from, comma - from)));
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