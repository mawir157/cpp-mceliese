#include "linearcode.h"

LinearCode::LinearCode(const matrix& M, const size_t width) : 
    mn_code_width(width), mb_can_decode(true)
{
    mv_generator = M;
    prepend_identity(mv_generator, width);

    mv_check = M;
    mv_check = transpose(mv_check, width);
    append_identity(mv_check, width);

    mn_max_errors = (calc_minimum_weight() / 2) - 1;
    mm_syndromes = build_syn_table(mv_check, 
                                   M.size() + width,
                                   mn_max_errors);
}

LinearCode::~LinearCode()
{

}

void LinearCode::swapColumns(const size_t c1, const size_t c2)
{
    for (size_t j=0; j < mv_generator.size(); ++j)
        mv_generator[j] = swap_bits(mv_generator[j], c1, c2);

    return;
}

code_word LinearCode::encode_symbol(const code_word r)
{
    code_word plain = r;
    code_word cipher = 0;
    for (size_t i = 0; i < mv_generator.size(); ++i, plain >>= 1)
        if (1 == (plain & 1))
            cipher = row_add(cipher, mv_generator[i]);

    return cipher;
}

std::vector<code_word> LinearCode::encode_message(const std::vector<code_word>& message)
{
    std::vector<code_word> ciphertext;
    for (size_t i = 0; i <  message.size(); ++i)
        ciphertext.push_back(encode_symbol(message[i]));

    return ciphertext;
}

code_word LinearCode::decode_symbol(const code_word r)
{
    code_word check = check_symbol(r, mv_check);
    if (0 == check)
        return (r >> mn_code_width);

    code_word syndrome = mm_syndromes[check];
    code_word temp = row_add(r, syndrome);

    return (temp >> mn_code_width);
}

std::vector<code_word> LinearCode::decode_message(const std::vector<code_word>& message)
{
    if (!mb_can_decode)
    {
        std::cout << "This linear code cannot be used to decode messages." <<std::endl;
        return message;
    }

    std::vector<code_word> plaintext;
    for (size_t i = 0; i <  message.size(); ++i)
        plaintext.push_back(decode_symbol(message[i]));

    return plaintext;
}

unsigned int LinearCode::calc_minimum_weight()
{
    const size_t max_word = 1 << mv_generator.size();
    unsigned int min_wt = 1000;
    for (code_word wd = 1; wd < max_word; ++wd)
    {
        const code_word test = encode_symbol(wd);
        const unsigned int test_wt = row_weight(test);

        if (test_wt < min_wt)
            min_wt = test_wt;
    }
    std::cout << min_wt << std::endl;
    return min_wt;
}

void LinearCode::print()
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