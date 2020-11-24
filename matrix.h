#pragma once

#include "includes.h"

static const size_t BITS = 64;
typedef std::bitset<BITS> code_word;
static const code_word BS0 = 0;

void increment_codeword(code_word& cw);
bool gt_codeword(const code_word &b1, const code_word &b2);

// needed for the syndrome table;
struct Comparer
{
    bool operator() (const code_word &b1, const code_word &b2) const
    {
        for (size_t i = 0; i < BITS; ++i)
        {
             // if most significant bits differ
            if (b1[BITS - 1 - i] ^ b2[BITS - 1 - i])
                return b2[BITS - 1 - i];
        }
        return false;
    }
};

typedef std::map<code_word, code_word, Comparer> syndrome_table;

typedef std::vector<code_word> matrix;

size_t row_dot(const code_word& r1, const code_word& r2);
code_word row_add(const code_word& r1, const code_word& r2);

size_t row_weight(const code_word& r);

matrix transpose(const matrix& m, const size_t n_bits);

void print_codeword(const code_word& r, const bool new_line=true);
void print_matrix(const matrix& rows);

code_word vec_to_code_word(const std::vector<size_t>& v);
code_word reverse(const code_word& r, const size_t width);
bool row_order(const code_word& lhs, const code_word& rhs);

bool row_order(const code_word& lhs, const code_word& rhs);
matrix canonise(const matrix& cw);

matrix identity(const size_t n);
matrix multiply(const matrix& m1, const size_t bits1,
                const matrix& m2, const size_t bits2);
matrix invert(const matrix& M);

void recursively_build(matrix rows, size_t depth, const code_word& max_row,
                       std::vector<matrix>& matrices, const bool verbose=false);
void recursively_build_new(matrix rows,
                           size_t depth,
                           const size_t cur_index,
                           const std::vector<code_word>& odd_words,
                           std::vector<matrix>& matrices,
                           const bool verbose=false);

std::vector<matrix> all(const size_t n, const bool verbose=false);
matrix find(const size_t n, const size_t bits);
matrix find(const size_t n);
std::vector<code_word> basis_span(const std::vector<code_word>& generator);

std::vector<size_t> v_space_sig(const std::vector<code_word>& generator);
std::map<size_t, size_t> sig_table(const std::vector<code_word>& generator);                                 

std::vector<code_word> words_with_at_most_n_bits(const size_t n,
                                                 const size_t max_bits);
std::vector<code_word> words_with_odd_weight(const size_t max_bits,
                                             std::vector<size_t>& breaks);
void combs(std::vector<std::vector<size_t>>& cs, const std::vector<size_t>& ss,
           std::vector<size_t>& combination,
           const size_t offset, const size_t k);