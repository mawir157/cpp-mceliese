#include "linearcode.h"

LinearCode::LinearCode(const matrix& M, const uint64_t width) : 
    mn_code_width(width), mb_can_decode(true)
{
    mv_generator = M;
    prepend_identity(mv_generator, width);

    mv_check = M;
    mv_check = transpose(mv_check, width);
    append_identity(mv_check, width);

    uint64_t min_weight = calc_minimum_weight();
    mn_max_errors = (min_weight <= 2) ? 0 : ((min_weight / 2) - 1);
    mm_syndromes = build_syn_table(mv_check, 
                                   M.size() + width,
                                   mn_max_errors);
}

LinearCode::~LinearCode()
{

}

void LinearCode::swapColumns(const uint64_t c1, const uint64_t c2)
{
    for (uint64_t j=0; j < mv_generator.size(); ++j)
        mv_generator[j] = swap_bits(mv_generator[j], c1, c2);

    return;
}

code_word LinearCode::encode_symbol(const code_word r) const
{
    code_word plain = r;
    code_word cipher = 0;
    for (uint64_t i = 0; i < mv_generator.size(); ++i, plain >>= 1)
        if (1 == (plain & 1))
            cipher = row_add(cipher, mv_generator[i]);

    return cipher;
}

std::vector<code_word> LinearCode::encode_message(const std::vector<code_word>& message) const
{
    std::vector<code_word> ciphertext;
    for (uint64_t i = 0; i <  message.size(); ++i)
        ciphertext.push_back(encode_symbol(message[i]));

    return ciphertext;
}

code_word LinearCode::decode_symbol(const code_word r) const
{
    code_word check = check_symbol(r, mv_check);
    if (0 == check)
        return (r >> mv_generator.size());

    code_word syndrome = mm_syndromes[check];
    code_word temp = row_add(r, syndrome);

    return (temp >> mv_generator.size());
}

std::vector<code_word> LinearCode::decode_message(const std::vector<code_word>& message) const
{
    if (!mb_can_decode)
    {
        std::cout << "This linear code cannot be used to decode messages." <<std::endl;
        return message;
    }

    std::vector<code_word> plaintext;
    for (uint64_t i = 0; i <  message.size(); ++i)
        plaintext.push_back(decode_symbol(message[i]));

    return plaintext;
}

uint64_t LinearCode::calc_minimum_weight() const
{
    uint64_t max_word = 1;
    max_word <<= mv_generator.size();
    uint64_t min_wt = 1000;
    for (code_word wd = 1; wd < max_word; ++wd)
    {
        const code_word test = encode_symbol(wd);
        const uint64_t test_wt = row_weight(test);

        if (test_wt < min_wt)
            min_wt = test_wt;
    }

    return min_wt;
}

void LinearCode::print() const
{
    std::cout << "Generating matrix:" << std::endl;
    print_matrix(mv_generator, mv_generator.size() + mn_code_width);
    if (mb_can_decode)
    {
        std::cout << "Checking matrix:" << std::endl;
        print_matrix(mv_check, mv_generator.size() + mn_code_width);
    }
    else
        std::cout << "Cannot decode" << std::endl;

    std:: cout << "Maximum errors = " << mn_max_errors << std::endl;

    if (mb_can_decode)
    {
        std::cout << "Syndrome table" << std::endl;
        print_syn_table(mm_syndromes, mn_code_width);
    }
}