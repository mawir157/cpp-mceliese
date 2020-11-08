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

size_t row_dot(const code_word r1, const code_word r2)
{
    return row_weight(r1 & r2) % 2;
}

code_word row_add(const code_word r1, const code_word r2)
{
    return (r1 ^ r2);
}

size_t col_weight(const matrix& rows, const size_t c1)
{
    size_t total = 0;
    for (size_t i=0; i < rows.size(); ++i)
        total += (get_bit(rows[i], c1) ? 1 : 0);

    return total;
}

std::vector<size_t> column_weights(const matrix& rows)
{
    std::vector<size_t> cols(rows.size());

    for (size_t i=0; i < rows.size(); ++i)
    {
        for (size_t j=0; j < rows.size(); ++j)
        {
            cols[j] += (get_bit(rows[i], j) ? 1 : 0);
        }
    }
    return cols;
}

size_t row_weight(const code_word& r)
{
    return r.count();
}

void print_codeword(code_word r, const size_t n, const bool new_line)
{
    // std::vector<code_word> bin;
    // for (size_t i = 0; i < n; ++i)
    // {
    //     bin.push_back(r & BS1);
    //     r >>= 1;
    // }
    // for (size_t i = 1; i <= n; ++i)
    //     std::cout << bin[n - i];
    std::cout << r;

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
    const size_t wt1 = row_weight(r1);
    const size_t wt2 = row_weight(r2);
    if (wt1 < wt2)
        return true;
    
    if (wt1 > wt2)
        return false;

    // return (r1 > r2);
    return (r1.to_ullong() > r2.to_ullong());
}

code_word swap_bits(const code_word r, const size_t c1, const size_t c2)
{
    const code_word b1 = (get_bit(r, c1) ? 1 : 0); 
    const code_word b2 = (get_bit(r, c2) ? 1 : 0); 
    code_word x = (b1 ^ b2); 
    x = (x << c1) | (x << c2); 

    return (r ^ x); 
}

code_word flip_bit(const code_word r, const size_t c1)
{
    code_word x = 1;
    x <<= c1;
    return (r ^ x); 
}

code_word reverse(const code_word r, const size_t width)
{
    code_word p = 0;
    for (size_t i = 0; i < width; ++i)
    {
        p <<= 1;
        p |= (get_bit(r, i) ? 1 : 0);
    }
    return p;
}

void swap_columns(matrix& m, const size_t c1, const size_t c2)
{
    for (size_t j=0; j < m.size(); ++j)
        m[j] = swap_bits(m[j], c1, c2);

    return;
}

bool get_bit(const code_word r, const size_t n)
{
    return r[n];
}

code_word set_bit(const code_word r, const size_t n, const bool bit)
{
    code_word v = bit ? 1 : 0;
    v <<= n;

    if (bit) 
        return (r | v);// we are setting a bit to 1
    else
        return (r & ~v);// we are setting a bit to 0
}

code_word vec_to_code_word(const std::vector<size_t> v)
{
    code_word cw = 0;
    for (size_t i = 0; i < v.size(); ++i)
        cw = set_bit(cw, v[i], true);

    return cw;
}

void order_columns(matrix& m)
{
    std::vector<size_t> col_wts = column_weights(m);
    for (size_t i = 1; i < m.size(); ++i)
    {
        for (size_t j = 1; j < m.size(); ++j)
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
                       size_t depth,
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


    for (unsigned long long ri = rows.back().to_ullong() - 1; ri > 0; --ri)
    {
        code_word rw = ri;
        if (0 == (row_weight(rw) & 1))
            continue;

        bool ok = true;
        for (uint64_t i = 0; i < rows.size(); ++i)
        {
            if (1 == row_dot(rows[i], rw))
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

    while (mask.to_ullong() < max.to_ullong())
    {
        code_word seed = max ^ mask;
        if (verbose)
        {
            std::cout << "First code_word: ";
            print_codeword(seed, n);
            std::cout << std::endl;
        }

        // shift up by 2 bits then set the new bit to 1.
        mask <<= 2;
        mask |= ((BS1 << 1) & BS1);

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
    const unsigned long long max = (1 << bits);
    while(m.size() < n)
    {
        // TODO: this needs to be done properly
        const code_word c = rand() % max;
        bool ok = true;

        for (uint64_t i = 0; i < m.size(); ++i)
        {
            if (1 == row_dot(m[i], c))
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