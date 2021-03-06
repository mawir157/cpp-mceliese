#include "encode.h"

void prepend_identity(matrix& rows, const size_t n_bits)
{
    // have to do this in two steps since literal 1 is 32-bit
    code_word prefix = 1;
    prefix <<= (rows.size() + n_bits - 1);
    for (size_t i = 0;  i < rows.size(); ++i, prefix >>= 1)
        rows[i] |= prefix;

    return;
}

void append_identity(matrix& rows, const size_t n_bits)
{
    size_t shift = n_bits - 1;
    code_word prefix = 1;
    prefix <<= shift;
    for (size_t i = 0;  i < rows.size(); ++i, prefix >>= 1)
    {
        rows[i] <<= (shift + 1);
        rows[i] |= prefix;
    }
    return;
}

code_word check_symbol(const code_word& r, const matrix& check_code)
{
    code_word cipher = r;
    code_word check = 0;
    for (size_t i = 0; i < check_code.size(); ++i)
    {
        check <<= 1;
        code_word dot = row_dot(cipher, check_code[i]);
        check |= dot;
    }
    return check;
}

syndrome_table build_syn_table(const matrix& check_matrix,
                               const size_t width,
                               const size_t max_errors)
{
    size_t n = width;

    syndrome_table s_table;
    std::vector<code_word> errors = words_with_at_most_n_bits(max_errors, n);
    for (size_t i = 0; i < errors.size(); ++i)
    {
        const code_word err = errors[i];
        const code_word check = check_symbol(err, check_matrix);

        s_table[check] = err;
    }

    return s_table;
}

code_word add_upto_n_error(const code_word& wd, const size_t width,
                           const size_t n = 1)
{
    code_word new_word = wd;
    for (size_t i = 0; i < n; ++i)
        new_word.flip(rand() % width);

    return new_word;
}
