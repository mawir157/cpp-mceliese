#include "matrix.h"

inline bool operator==(const matrix& lhs, const matrix& rhs)
{
    if (lhs.size() != rhs.size())
        return false;
    
    for (uint64_t i = 0; i < lhs.size(); ++i)
        if (lhs[i] != rhs[i])
            return false;

    return true;
}

uint64_t row_dot(const code_word r1, const code_word r2)
{
    return row_weight(r1 & r2) % 2;
}

code_word row_add(const code_word r1, const code_word r2)
{
    return (r1 ^ r2);
}

uint64_t col_weight(const matrix& rows, const uint64_t c1)
{
    uint64_t total = 0;
    for (uint64_t i=0; i < rows.size(); ++i)
        total += ((rows[i] >> c1) & 1);

    return total;
}

std::vector<uint64_t> column_weights(const matrix& rows)
{
    std::vector<uint64_t> cols(rows.size());

    for (uint64_t i=0; i < rows.size(); ++i)
    {
        for (uint64_t j=0; j < rows.size(); ++j)
        {
            cols[j] += ((rows[i] >> j) & 1);
        }
    }
    return cols;
}

uint64_t row_weight(code_word r)
{
    uint64_t wt = 0;
    while (r > 0)
    {
        wt += (r & 1);
        r >>= 1;
    }
    return wt;
}

void print_codeword(code_word r, const uint64_t n, const bool new_line)
{
    std::vector<uint64_t> bin;
    for (uint64_t i = 0; i < n; ++i)
    {
        bin.push_back(r & 1);
        r >>= 1;
    }
    for (uint64_t i = 1; i <= n; ++i)
        std::cout << bin[n - i];

    if (new_line)
        std::cout << std::endl;

    return;
}

void print_matrix(const matrix rows, uint64_t n)
{
    if (n == 0)
        n = rows.size();

    for (uint64_t i = 0; i < rows.size(); ++i)
        print_codeword(rows[i], n);

    std::cout << std::endl;

    return;
}

bool order_by_weight(const code_word r1, const code_word r2)
{
    const uint64_t wt1 = row_weight(r1);
    const uint64_t wt2 = row_weight(r2);
    if (wt1 < wt2)
        return true;
    
    if (wt1 > wt2)
        return false;

    return (r1 > r2);
}

code_word swap_bits(const code_word r, const uint64_t c1, const uint64_t c2)
{
    const code_word b1 =  (r >> c1) & 1; 
    const code_word b2 =  (r >> c2) & 1; 
    code_word x = (b1 ^ b2); 
    x = (x << c1) | (x << c2); 

    return (r ^ x); 
}

code_word flip_bit(const code_word r, const uint64_t c1)
{
    code_word x = 1;
    x <<= c1;
    return (r ^ x); 
}

void swap_columns(matrix& m, const uint64_t c1, const uint64_t c2)
{
    for (uint64_t j=0; j < m.size(); ++j)
        m[j] = swap_bits(m[j], c1, c2);

    return;
}

void order_columns(matrix& m)
{
    std::vector<uint64_t> col_wts = column_weights(m);
    for (uint64_t i = 1; i < m.size(); ++i)
    {
        for (uint64_t j = 1; j < m.size(); ++j)
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

void recursively_build(matrix rows,
                       uint64_t depth,
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
        for (uint64_t i = 0; i < rows.size(); ++i)
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

std::vector<matrix> all(const uint64_t n, const bool verbose)
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

matrix find(const uint64_t n, const uint64_t bits)
{
    matrix m;
    const code_word max = (1 << bits);
    while(m.size() < n)
    {
        code_word c = rand() % max;
        bool ok = true;

        for (uint64_t i = 0; i < m.size(); ++i)
        {
            if (0 != (row_dot(m[i], c) & 1))
            {
                ok = false;
                break;
            }
        }

        if (ok)
            m.push_back(c);
    }

    return m;
}