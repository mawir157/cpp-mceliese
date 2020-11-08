#pragma once

#include "includes.h"

// typedef uint64_t code_word;
typedef std::bitset<64> code_word;
static const code_word BS0 = 0;
static const code_word BS1 = 1;
// needed for the syndrome table;
struct Comparer
{
    bool operator() (const code_word &b1, const code_word &b2) const
    {
        return b1.to_ullong() < b2.to_ullong();
    }
};


typedef std::map<code_word, code_word, Comparer> syndrome_table;


typedef std::vector<code_word> matrix;

//
inline bool operator==(const matrix& lhs, const matrix& rhs);
code_word row_dot(const code_word r1, const code_word r2);
code_word row_add(const code_word r1, const code_word r2);

size_t col_weight(const matrix& rows, const size_t c1);
std::vector<size_t> column_weights(const matrix& rows);
size_t row_weight(const code_word& r);

void print_codeword(code_word r, const size_t n, const bool new_line=true);
void print_matrix(const matrix rows, size_t n=0);

bool order_by_weight(const code_word r1, const code_word r2);
code_word swap_bits(const code_word r, const size_t c1, const size_t c2);
code_word flip_bit(const code_word r, const size_t c1);
bool get_bit(const code_word r, const size_t c1);
code_word set_bit(const code_word r, const size_t n, const bool bit);
code_word vec_to_code_word(const std::vector<size_t> v);
code_word reverse(const code_word r, const size_t width);
void swap_columns(matrix& m, const size_t c1, const size_t c2);
void order_columns(matrix& m);

void recursively_build(matrix rows, size_t depth, const code_word max_row,
                       std::vector<matrix>& matrices, const bool verbose=false);

std::vector<matrix> all(const size_t n, const bool verbose=false);
matrix find(const size_t n, const size_t bits);