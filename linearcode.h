#pragma once

#include "includes.h"

typedef uint64_t code_word;
typedef std::vector<code_word> matrix;

inline bool operator==(const matrix& lhs, const matrix& rhs);
unsigned int row_dot(const code_word r1, const code_word r2);
code_word row_add(const code_word r1, const code_word r2);

unsigned int col_weight(const matrix& rows, const size_t c1);
std::vector<unsigned int> column_weights(const matrix& rows);
unsigned int row_weight(code_word r);

void print_codeword(code_word r, const size_t n, const bool new_line=true);
void print_matrix(const matrix rows, size_t n=0);

bool order_by_weight(const code_word r1, const code_word r2);
code_word swap_bits(const code_word r, const size_t c1, const size_t c2);
code_word flip_bit(const code_word r, const size_t c1);
void swap_columns(matrix& m, std::vector<unsigned int>& count,
                  const size_t c1, const size_t c2);
void order_columns(matrix& m);
matrix transpose(const matrix& m);

void recursively_build(matrix rows, unsigned int depth, const code_word max_row,
                       std::vector<matrix>& matrices, const bool verbose=false);

std::vector<matrix> all(const unsigned int n, const bool verbose=false);