#include "includes.h"
#include "matrix.h"
#include "encode.h"
#include "linearcode.h"

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

    LinearCode lc = LinearCode(mats[best_i], n_bits);
    lc.print();

    std::vector<code_word> ciphertext = lc.encode_message(message);

    // add errors to the cipher text
    std::vector<code_word> errortext;
    for (size_t i = 0; i < ciphertext.size(); ++i)
        errortext.push_back(add_upto_n_error(ciphertext[i],
                                             2*lc.get_code_with(),
                                             lc.get_max_error()));

    std::vector<code_word> plaintext1 = lc.decode_message(ciphertext);
    std::vector<code_word> plaintext2 = lc.decode_message(errortext);
    
    for (size_t i = 0; i < plaintext1.size(); ++i)
    {
        if (plaintext1[i] != plaintext2[i])
        {
            print_codeword(plaintext1[i], lc.get_code_with(), false);
            std::cout << " -> ";
            print_codeword(plaintext2[i], lc.get_code_with());
        }
    }

    return 0;
}
