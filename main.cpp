#include "includes.h"
#include "matrix.h"
#include "encode.h"
#include "linearcode.h"
#include "mceliese.h"

int main (int argc, char **argv)
{
    srand( time(NULL) ); // TODO - use a better rng

    if (argc < 2)
    {
        std::cout << "invalid options" << std::endl;
        return -1;
    }
    uint64_t n_bits = std::stoi(argv[1]);

    // clock_t startTime = clock();

    // std::vector<matrix> mats = all(n_bits, false);

    // sort( mats.begin(), mats.end() );
    // mats.erase( unique( mats.begin(), mats.end() ), mats.end() );
    // const uint64_t best_i = rand() % mats.size();
    // std::cout << "Found " << mats.size() << " self dual codes of length " 
    //           << (2*n_bits) << std::endl;

    // std::cout << "Time to run: "
    //           << double( clock() - startTime ) / (double)CLOCKS_PER_SEC
    //           << " seconds." << std::endl;

    // LinearCode lc = LinearCode(mats[best_i], n_bits);
    // lc.print();
/*
    n_bits = 15;
    uint64_t n_words = 7;
    matrix bs = find(n_words, n_bits);

    std::vector<code_word> message;
    for (uint64_t i = 0; i < 100; ++i)
        message.push_back(rand() % (1 << n_words));

    LinearCode lc = LinearCode(bs, n_bits);
    lc.print();

    std::vector<code_word> ciphertext = lc.encode_message(message);

    // add errors to the cipher text
    std::vector<code_word> errortext;
    for (uint64_t i = 0; i < ciphertext.size(); ++i)
        errortext.push_back(add_upto_n_error(ciphertext[i],
                                             n_words + lc.get_code_with(),
                                             lc.get_max_error()));

    std::vector<code_word> plaintext1 = lc.decode_message(ciphertext);
    std::vector<code_word> plaintext2 = lc.decode_message(errortext);
    
    for (uint64_t i = 0; i < plaintext1.size(); ++i)
    {
        if (message[i] != plaintext2[i])
        {
            print_codeword(message[i], n_words, false);
            std::cout << " -> ";
            print_codeword(ciphertext[i], n_bits + n_words, false);
            std::cout << " -> ";
            print_codeword(plaintext1[i], n_words, false);
            std::cout << " -> ";
            print_codeword(plaintext2[i], n_words);
        }
    }

    return 0;
*/

    uint64_t n_words = 8;
    n_bits = 23;
    McEliesePrivate priv = GenPrivateKey(n_words, n_bits);
    LinearCode   G =  std::get<1>(priv);
    McEliesePublic   pub = PrivateToPublic(priv);
    
    G.print();
    pub.print();

    std::vector<code_word> message;
    for (uint64_t i = 0; i < 25; ++i)
        message.push_back(rand() % (1 << n_words));

    std::vector<code_word> ciphertext = McE_encypt_message(pub, message);
    for (size_t i = 0; i < message.size(); ++i)
    {
        print_codeword(message[i], n_words, false);
        std::cout << (i + 1 == message.size() ? "\n" : "-");
    }

    std::vector<code_word> plaintext = McE_decypt_message(priv, ciphertext);

    for (size_t i = 0; i < plaintext.size(); ++i)
    {
        print_codeword(plaintext[i], n_words, false);
        std::cout << (i + 1 == plaintext.size() ? "\n" : "-");
    }

    return 0;
}
