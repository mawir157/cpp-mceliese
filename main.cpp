#include "includes.h"
#include "matrix.h"
#include "encode.h"
#include "linearcode.h"
#include "mceliese.h"

int main (int argc, char **argv)
{
    srand( time(NULL) ); // TODO - use a better rng

    RunMode mode = RunMode::unknown;
    std::string message_file = "NULL";
    std::string key_file = "NULL";
    unsigned int arg_1 = 0;
    unsigned int arg_2 = 0;

    if (argc < 1)
    {
        std::cout << "invalid options" << std::endl;
        return -1;
    }
    else
    {
        // std::string cli_mode = argv[1];
        if ((std::strcmp(argv[1], "-o") == 0) ||
            (std::strcmp(argv[1], "-O") == 0))
        {
            if (argc != 3)
            {
                std::cout << "[Usage] ./mceliese -o [n_dims]" 
                          << std::endl;
                return -1;
            }
            mode = RunMode::orthogonal;
            arg_1 = std::stoi(argv[2]);
        }
        else if ((std::strcmp(argv[1], "-g") == 0) ||
                 (std::strcmp(argv[1], "-G") == 0))
        {
            if (argc != 4)
            {
                std::cout << "[Usage] ./mceliese -g [n_dims] [n_extra_bits]" 
                          << std::endl;
                return -1;
            }
            mode = RunMode::generate_keys;
            arg_1 = std::stoi(argv[2]);
            arg_2 = std::stoi(argv[3]);
        }
        else if  ((std::strcmp(argv[1], "-e") == 0) ||
                  (std::strcmp(argv[1], "-E") == 0))
        {
            if (argc != 4)
            {
                std::cout << "[Usage] ./mceliese -e [message filepath] [public key file]" 
                          << std::endl;
                return -1;
            }
            mode = RunMode::encode_message;
            std::string message_file = argv[2];
            std::string key_file     = argv[3];
        }
        else if  ((std::strcmp(argv[1], "-d") == 0) ||
                  (std::strcmp(argv[1], "-D") == 0))
        {
            if (argc != 4)
            {
                std::cout << "[Usage] ./mceliese -D [message filepath] [private key file]" 
                          << std::endl;
                return -1;
            }
            mode = RunMode::encode_message;
            std::string message_file = argv[2];
            std::string key_file     = argv[3];
        }
        else
        {
            std::cout << "Invalid option" << std::endl;
            std::cout << "Valid options: -o/O n     generate all orthogonal matrices" << std::endl;
            std::cout << "               -g/G n m   generate public and private keys" << std::endl;
            std::cout << "               -e/E f1 f2 encode message" << std::endl;
            std::cout << "               -d/D f1 f2 decode message" << std::endl;
        }
    }

    if (RunMode::orthogonal == mode)
    {
        size_t n_bits = arg_1;

        clock_t startTime = clock();

        std::vector<matrix> mats = all(n_bits, false);

        sort( mats.begin(), mats.end() );
        mats.erase( unique( mats.begin(), mats.end() ), mats.end() );
        std::cout << "Found " << mats.size() << " " << n_bits << "-by-" << n_bits
                  << " orthognal matrices over F2" << std::endl;

        std::cout << "Time to run: "
                  << double( clock() - startTime ) / (double)CLOCKS_PER_SEC
                  << " seconds." << std::endl;

        return 0;       
    }

    if (RunMode::generate_keys == mode)
    {
        size_t n_words = arg_1;
        size_t n_bits = arg_2;
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


    return 0;
}
