#pragma once

#include "matrix.h"

void prepend_identity(matrix& rows, const size_t n_bits);
void append_identity(matrix& rows, const size_t n_bits);

code_word check_symbol(const code_word& r, const matrix& check_code);

syndrome_table build_syn_table(const matrix& l_code,
															 const size_t width,
                               const size_t max_errors);

code_word add_upto_n_error(const code_word& wd, const size_t width,
                           const size_t n);