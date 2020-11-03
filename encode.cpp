#include "encode.h"

void prepend_identity(matrix& rows, const uint64_t n_bits)
{
    code_word prefix = (1 << (rows.size() + n_bits - 1));
    for (uint64_t i = 0;  i < rows.size(); ++i, prefix >>= 1)
        rows[i] += prefix;

    return;
}

void append_identity(matrix& rows, const uint64_t n_bits)
{
    uint64_t shift = n_bits - 1;
    code_word prefix = (1 << shift);
    for (uint64_t i = 0;  i < rows.size(); ++i, prefix >>= 1)
    {
        rows[i] <<= (shift + 1);
        rows[i] += prefix;
    }
    return;
}

matrix transpose(const matrix& m, const uint64_t n_bits)
{
    matrix tp;
    for (uint64_t i = 0; i < n_bits; ++i)
    {
        code_word wd = 0;
        for (uint64_t j = 0; j < m.size(); ++j)
        {
            wd <<= 1;
            wd += ((m[j] >> i) & 1);
        }
        tp.push_back(wd);
    }
    std::reverse(tp.begin(), tp.end()); 
    return tp;
}


code_word check_symbol(const code_word r, const matrix& check_code)
{
    code_word cipher = r;
    code_word check = 0;
    for (uint64_t i = 0; i < check_code.size(); ++i)
    {
        check <<= 1;
        uint64_t dot = row_dot(cipher, check_code[i]);
        check += dot;
    }
    return check;
}

std::vector<code_word> check_message(const std::vector<code_word>& message,
                                     const matrix& check_code)
{
    std::vector<code_word> checktext;
    for (uint64_t i = 0; i <  message.size(); ++i)
        checktext.push_back(check_symbol(message[i], check_code));

    return checktext;
}

// this is horribly inefficient - can redo in a clever way
std::vector<code_word> words_with_at_most_n_bits(const uint64_t n,
                                                 const uint64_t max_bits)
{
    std::vector<code_word> words;
    std::vector<size_t> ns; // ns = [0,1,2,..,max_bits - 1]
    for(size_t i = 0; i < max_bits; ++i)
        ns.push_back(i);

    for (size_t m = 1; m <= n; ++m)
    {
        std::vector<std::vector<size_t>> cs;
        std::vector<size_t> comb;
        combs(cs, ns, comb, 0, m);

        for (size_t i = 0; i < cs.size(); ++i)
            words.push_back(vec_to_code_word(cs[i]));
    }
    return words;
}

void combs(std::vector<std::vector<size_t>>& cs, const std::vector<size_t>& ss,
           std::vector<size_t>& combination,
           const size_t offset, const size_t k) {
  if (k == 0)
  {
    cs.push_back(combination);
    return;
  }
  for (size_t i = offset; i <= ss.size() - k; ++i)
  {
    combination.push_back(ss[i]);
    combs(cs, ss, combination, i+1, k-1);
    combination.pop_back();
  }
  return;
}

syndrome_table build_syn_table(const matrix& check_matrix,
                               const uint64_t width,
                               const uint64_t max_errors)
{
    uint64_t n = width;

    std::map<code_word, code_word> s_table;
    std::vector<code_word> errors = words_with_at_most_n_bits(max_errors, n);
    for (uint64_t i = 0; i < errors.size(); ++i)
    {
        const code_word err = errors[i];
        const code_word check = check_symbol(err, check_matrix);

        s_table[check] = err;
    }

    return s_table;
}

void print_syn_table(const syndrome_table& st, const size_t n_bits,
                     const size_t n_words)
{
    for (syndrome_table::const_iterator it = st.begin(); it != st.end(); ++it)
    {
        std::cout << "[";
        print_codeword(it->first, n_bits, false);
        std::cout << " => ";
        print_codeword(it->second, n_words + n_bits, false);
        std::cout << "]" << std::endl;
    }
}

code_word add_upto_n_error(const code_word& wd, const uint64_t width,
                           const uint64_t n = 1)
{
    code_word new_word = wd;
    for (uint64_t i = 0; i < n; ++i)
    {
        const uint64_t pos = rand() % width;
        new_word = flip_bit(new_word, pos);
    }
    return new_word;
}
