#include "linearcode.h"

LinearCode::LinearCode(const matrix& M, const size_t width) :
    mv_generator(M)
  , mn_code_width(width)
  , mb_can_decode(true)
{
    // mv_generator = M;
    prepend_identity(mv_generator, width);

    mv_check = M;
    mv_check = transpose(mv_check, width);
    append_identity(mv_check, width);

    size_t min_weight = calc_minimum_weight();
    mn_max_errors = (min_weight <= 2) ? 0 : ((min_weight / 2) - 1);
    mm_syndromes = build_syn_table(mv_check,
                                   M.size() + width,
                                   mn_max_errors);
}

LinearCode::~LinearCode()
{

}

code_word LinearCode::encode_symbol(const code_word& r) const
{
    code_word plain = reverse(r, code_word_size());
    code_word cipher = 0;
    for (size_t i = 0; i < code_word_size(); ++i, plain >>= 1)
        if (BS1 == (plain & BS1))
            cipher = row_add(cipher, mv_generator[i]);

    return cipher;
}

std::vector<code_word> LinearCode::encode_message(const std::vector<code_word>& message) const
{
    std::vector<code_word> ciphertext;
    for (size_t i = 0; i < message.size(); ++i)
        ciphertext.push_back(encode_symbol(message[i]));

    return ciphertext;
}

code_word LinearCode::decode_symbol(const code_word& r) const
{
    const code_word check = check_symbol(r, mv_check);
    if (BS0 == check)
        return (r >> mn_code_width);

    const code_word syndrome = mm_syndromes.at(check);
    code_word temp = row_add(r, syndrome);

    return (temp >> mn_code_width);
}

std::vector<code_word> LinearCode::decode_message(const std::vector<code_word>& message) const
{
    if (!mb_can_decode)
    {
        std::cout << "This linear code cannot be used to decode messages." <<std::endl;
        return message;
    }

    std::vector<code_word> plaintext;
    for (size_t i = 0; i < message.size(); ++i)
        plaintext.push_back(decode_symbol(message[i]));

    return plaintext;
}

size_t LinearCode::calc_minimum_weight() const
{
    unsigned long long max_word = 1;
    max_word <<= code_word_size();
    size_t min_wt = 1000;
    for (unsigned long long wi = 1; wi < max_word; ++wi)
    {
        const code_word wd = wi;
        const code_word test = encode_symbol(wd);
        const size_t test_wt = row_weight(test);

        if (test_wt < min_wt)
            min_wt = test_wt;
    }

    return min_wt;
}

matrix LinearCode::get_extra_bits() const
{
    matrix x_bits;
    unsigned long long mask = 1;
    mask <<= mn_code_width;
    mask -= 1;

    code_word cw_mask = mask;
    for (size_t i = 0; i < mv_generator.size(); ++i)
        x_bits.push_back(mv_generator[i] & cw_mask);

    return x_bits;
}