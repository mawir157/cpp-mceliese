#include "matrix.h"

inline bool operator==(const matrix& lhs, const matrix& rhs)
{
    if (lhs.size() != rhs.size())
        return false;
    
    for (size_t i = 0; i < lhs.size(); ++i)
        if (lhs[i] != rhs[i])
            return false;

    return true;
}

unsigned int row_dot(const code_word r1, const code_word r2)
{
    return row_weight(r1 & r2) % 2;
}

code_word row_add(const code_word r1, const code_word r2)
{
    return (r1 ^ r2);
}

unsigned int col_weight(const matrix& rows, const size_t c1)
{
    unsigned int total = 0;
    for (size_t i=0; i < rows.size(); ++i)
        total += ((rows[i] >> c1) & 1);

    return total;
}

std::vector<unsigned int> column_weights(const matrix& rows)
{
    std::vector<unsigned int> cols(rows.size());

    for (size_t i=0; i < rows.size(); ++i)
    {
        for (size_t j=0; j < rows.size(); ++j)
        {
            cols[j] += ((rows[i] >> j) & 1);
        }
    }
    return cols;
}

unsigned int row_weight(code_word r)
{
    unsigned int wt = 0;
    while (r > 0)
    {
        wt += (r & 1);
        r >>= 1;
    }
    return wt;
}

void print_codeword(code_word r, const size_t n, const bool new_line)
{
    std::vector<unsigned int> bin;
    for (size_t i = 0; i < n; ++i)
    {
        bin.push_back(r & 1);
        r >>= 1;
    }
    for (size_t i = 1; i <= n; ++i)
        std::cout << bin[n - i];

    if (new_line)
        std::cout << std::endl;

    return;
}

void print_matrix(const matrix rows, size_t n)
{
    if (n == 0)
        n = rows.size();

    for (size_t i = 0; i < rows.size(); ++i)
        print_codeword(rows[i], n);

    std::cout << std::endl;

    return;
}

bool order_by_weight(const code_word r1, const code_word r2)
{
    const unsigned int wt1 = row_weight(r1);
    const unsigned int wt2 = row_weight(r2);
    if (wt1 < wt2)
        return true;
    
    if (wt1 > wt2)
        return false;

    return (r1 > r2);
}

code_word swap_bits(const code_word r, const size_t c1, const size_t c2)
{
    const code_word b1 =  (r >> c1) & 1; 
    const code_word b2 =  (r >> c2) & 1; 
    code_word x = (b1 ^ b2); 
    x = (x << c1) | (x << c2); 

    return (r ^ x); 
}

code_word flip_bit(const code_word r, const size_t c1)
{
    const code_word x = 1 << c1;
    return (r ^ x); 
}

void swap_columns(matrix& m, const size_t c1, const size_t c2)
{
    for (size_t j=0; j < m.size(); ++j)
        m[j] = swap_bits(m[j], c1, c2);

    return;
}

void order_columns(matrix& m)
{
    std::vector<unsigned int> col_wts = column_weights(m);
    for (unsigned int i = 1; i < m.size(); ++i)
    {
        for (unsigned int j = 1; j < m.size(); ++j)
        {
            if (col_wts[j-1] < col_wts[j])
            {
                swap_columns(m, j-1, j);

                std::iter_swap(col_wts.begin() + (j-1), col_wts.begin() + j);
            }
        }
    }
    return;
}

matrix transpose(const matrix& m)
{
    matrix tp;
    for (size_t i = 0; i < m.size(); ++i)
    {
        code_word wd = 0;
        for (size_t j = 0; j < m.size(); ++j)
        {
            wd <<= 1;
            wd += ((m[j] >> i) & 1);
        }
        tp.push_back(wd);
    }
    std::reverse(tp.begin(), tp.end()); 
    return tp;
}

void recursively_build(matrix rows,
                       unsigned int depth,
                       const code_word max_row,
                       std::vector<matrix>& matrices,
                       const bool verbose)
{
    if (depth == 0)
    {
        std::sort(rows.begin(), rows.end(), order_by_weight);
        order_columns(rows);
        if (verbose)
            print_matrix(rows);
        matrices.push_back(rows);
        return;
    }


    for (code_word rw = rows.back() - 1; rw > 0; --rw)
    {
        if (0 == (row_weight(rw) & 1))
            continue;

        bool ok = true;
        for (size_t i = 0; i < rows.size(); ++i)
        {
            if (0 != (row_dot(rows[i], rw) & 1))
            {
                ok = false;
                break;
            }
        }

        if (ok)
        {
            matrix r_copy = rows;
            r_copy.push_back(rw);
            recursively_build(r_copy, depth - 1, max_row, matrices, verbose);
        }
    }
    return;
}

std::vector<matrix> all(const unsigned int n, const bool verbose)
{
    const code_word max = (1 << n) - 1;
    code_word mask = ((n+1) & 1); // 1 or 0.
    std::vector<matrix> matrices;

    while (mask < max)
    {
        code_word seed = max ^ mask;
        if (verbose)
        {
            std::cout << "First code_word: ";
            print_codeword(seed, n);
            std::cout << std::endl;
        }

        mask <<= 2;
        mask += 3;

        matrix test = {seed}; 
        recursively_build(test, n-1, max, matrices, verbose);
        if (verbose)
            std::cout << matrices.size() << std::endl;
    }
    return matrices;
}