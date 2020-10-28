#include "includes.h"
#include "linearcode.h"
#include "encode.h"

int main (int argc, char **argv)
{
    srand( time(NULL) ); // TODO - use a better rng

    if (argc < 2)
    {
        std::cout << "invalid options" << std::endl;
        return -1;
    }
    unsigned int n_bits = std::stoi(argv[1]);

    clock_t startTime = clock();

    std::vector<matrix> mats = all(n_bits, false);

    sort( mats.begin(), mats.end() );
    mats.erase( unique( mats.begin(), mats.end() ), mats.end() );
    const size_t best_i = rand() % mats.size();
    std::cout << "Found " << mats.size() << " self dual codes of length " 
              << (2*n_bits) << std::endl;

    std::cout << "Time to run: "
              << double( clock() - startTime ) / (double)CLOCKS_PER_SEC
              << " seconds." << std::endl;

    std::vector<code_word> message;
    for (size_t i = 0; i < 100; ++i)
        message.push_back(rand() % (1 << n_bits));

    std::tuple<matrix, matrix> gc = gen_check_matrices(mats[best_i]);

    const matrix linear_code = std::get<0>(gc);
    const matrix check_matrix = std::get<1>(gc);

    std::cout << "Code we are testing:" << std::endl;
    std::cout << "Generating matrix:" << std::endl;
    print_matrix(linear_code, 2*n_bits);
    std::cout << "Checking matrix:" << std::endl;
    print_matrix(check_matrix, 2*n_bits);

    unsigned int max_errors = (minimum_weight(linear_code) / 2) - 1;
    std::cout << "Minimum weight = " << minimum_weight(linear_code) << std::endl;
    std:: cout << "Maximum errors = " << max_errors << std::endl;

    std::vector<code_word> ciphertext =
                               encode_message(message, linear_code);

    // add errors to the cipher text
    std::vector<code_word> errortext;
    for (size_t i = 0; i < ciphertext.size(); ++i)
        errortext.push_back(add_upto_n_error(ciphertext[i], 2*n_bits, max_errors));

    std::vector<code_word> checktext2 = check_message(errortext, check_matrix);

    syndrome_table st = build_syn_table(check_matrix, max_errors);
    std::cout << "Syndrome table" << std::endl;
    print_syn_table(st, n_bits);

    std::vector<code_word> plaintext1 =  
                                   decode_message(ciphertext, check_matrix, st);     
    std::vector<code_word> plaintext2 =  
                                   decode_message(errortext, check_matrix, st); 
    
    for (size_t i = 0; i < plaintext1.size(); ++i)
    {
        if (plaintext1[i] != plaintext2[i])
        {
            print_codeword(plaintext1[i], linear_code.size(), false);
            std::cout << " -> ";
            print_codeword(plaintext2[i], linear_code.size());
        }
    }

    return 0;
}
