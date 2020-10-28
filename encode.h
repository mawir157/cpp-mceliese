#pragma once

#include "linearcode.h"

typedef std::map<code_word, code_word> syndrome_table;

void print_syn_table(syndrome_table& st, const unsigned int n_bits);

void prepend_identity(matrix& rows);
void append_identity(matrix& rows);
std::tuple<matrix, matrix> gen_check_matrices(const matrix& rows);

code_word encode_symbol(const code_word r, const matrix& m);
std::vector<code_word> encode_message(const std::vector<code_word>& message,
                                      const matrix& linear_code);
code_word check_symbol(const code_word r, const matrix& check_code);
std::vector<code_word> check_message(const std::vector<code_word>& message,
                                     const matrix& check_code);
code_word decode_symbol(const code_word r, const matrix& check_code, 
                        syndrome_table& stable);
std::vector<code_word> decode_message(const std::vector<code_word>& message,
                                      const matrix& check_code,
                                      syndrome_table& stable);

unsigned int minimum_weight(const matrix& linear_code);
syndrome_table build_syn_table(const matrix& l_code,
                               const unsigned int max_errors);
std::vector<code_word> words_with_at_most_n_bits(const unsigned int n,
																								 const unsigned int max_bits);

code_word add_upto_n_error(const code_word& wd, const unsigned int width,
                           const unsigned int n);