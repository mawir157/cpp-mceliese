#include "encode.h"

void prepend_identity(matrix& rows)
{
    code_word prefix = (1 << (2*rows.size() - 1));
    for (size_t i = 0;  i < rows.size(); ++i, prefix >>= 1)
        rows[i] += prefix;

    return;
}

void append_identity(matrix& rows)
{
    unsigned int shift = rows.size() - 1;
    code_word prefix = (1 << shift);
    for (size_t i = 0;  i < rows.size(); ++i, prefix >>= 1)
    {
        rows[i] <<= (shift + 1);
        rows[i] += prefix;
    }
    return;
}

code_word check_symbol(const code_word r, const matrix& check_code)
{
    code_word cipher = r;
    code_word check = 0;
    for (size_t i = 0; i < check_code.size(); ++i)
    {
        check <<= 1;
        unsigned int dot = row_dot(cipher, check_code[i]);
        check += dot;
    }
    return check;
}

std::vector<code_word> check_message(const std::vector<code_word>& message,
                                     const matrix& check_code)
{
    std::vector<code_word> checktext;
    for (size_t i = 0; i <  message.size(); ++i)
        checktext.push_back(check_symbol(message[i], check_code));

    return checktext;
}

// this is horribly inefficient - can redo in a clever way
std::vector<code_word> words_with_at_most_n_bits(const unsigned int n,
                                                 const unsigned int max_bits)
{
    std::vector<code_word> words;
    words.reserve(1000);
    unsigned int limit = 1 << max_bits;

    for (code_word wd = 1; wd < limit; ++ wd)
        if (row_weight(wd) <= n)
            words.push_back(wd);

    return words;
}

syndrome_table build_syn_table(const matrix& check_matrix,
                               const unsigned int max_errors)
{
    size_t n = 2*check_matrix.size();

    std::map<code_word, code_word> s_table;
    std::vector<code_word> errors = words_with_at_most_n_bits(max_errors, n);

    for (size_t i = 0; i < errors.size(); ++i)
    {
        const code_word err = errors[i];
        const code_word check = check_symbol(err, check_matrix);

        s_table[check] = err;
    }

    return s_table;
}

void print_syn_table(syndrome_table& st, const unsigned int n_bits)
{
    for (syndrome_table::iterator it = st.begin(); it != st.end(); ++it)
    {
        std::cout << "[";
        print_codeword(it->first, n_bits, false);
        std::cout << " => ";
        print_codeword(it->second, 2*n_bits, false);
        std::cout << "]" << std::endl;
    }
}

code_word add_upto_n_error(const code_word& wd, const unsigned int width,
                           const unsigned int n = 1)
{
    code_word new_word = wd;
    for (size_t i = 0; i < n; ++i)
    {
        const unsigned int pos = rand() % width;
        new_word = flip_bit(new_word, pos);
    }
    return new_word;
}
