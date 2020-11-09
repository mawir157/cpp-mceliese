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
        total += (rows[i][c1] ? 1 : 0);

    return total;
}

std::vector<size_t> column_weights(const matrix& rows)
{
    std::vector<size_t> cols(rows.size());

    for (size_t i=0; i < rows.size(); ++i)
    {
        for (size_t j=0; j < rows.size(); ++j)
        {
            cols[j] += (rows[i][j] ? 1 : 0);
        }
    }
    return cols;
}

size_t row_weight(const code_word& r)
{
    return r.count();
}

void print_codeword(code_word r, const bool new_line)
{
    std::cout << r;

    if (new_line)
        std::cout << std::endl;

    return;
}

void print_matrix(const matrix rows)
{
    for (size_t i = 0; i < rows.size(); ++i)
        print_codeword(rows[i]);

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
    const code_word b1 = (r[c1] ? 1 : 0); 
    const code_word b2 = (r[c2] ? 1 : 0); 
    code_word x = (b1 ^ b2); 
    x = (x << c1) | (x << c2); 

    return (r ^ x); 
}

code_word reverse(const code_word r, const size_t width)
{
    code_word p = 0;
    for (size_t i = 0; i < width; ++i)
    {
        p <<= 1;
        p |= (r[i] ? 1 : 0);
    }
    return p;
}

void swap_columns(matrix& m, const size_t c1, const size_t c2)
{
    for (size_t j=0; j < m.size(); ++j)
        m[j] = swap_bits(m[j], c1, c2);

    return;
}

code_word vec_to_code_word(const std::vector<size_t> v)
{
    code_word cw = 0;
    for (size_t i = 0; i < v.size(); ++i)
        cw.set(v[i], true);

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
    code_word max = 0;
    for (size_t i = 0; i < n; ++i)
        max.set(i);

    code_word mask = 0;
    if (0 == (n % 2))
        mask.set(0);

    std::vector<matrix> matrices;

    while (mask.to_ullong() < max.to_ullong() - 1)
    {
        code_word seed = max ^ mask;
        if (verbose)
        { 
            std::cout << "First code_word: ";
            print_codeword(seed, n);
            std::cout << std::endl;
        }

        // shift up by 2 bits then set the new bits to 1.
        mask <<= 2;
        // mask |= ((BS1 << 1) & BS1);
        mask.set(0);
        mask.set(1);

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
    unsigned long long max = 1;
    max <<= bits;
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

std::vector<code_word> basis_span(const std::vector<code_word>& generator)
{
    std::vector<code_word> all_vectors;
    unsigned long long max = 1;
    max <<= generator.size();
    for (unsigned long long i = 0; i < max; ++i)
    {
        code_word comb = 0;
        const std::bitset<64> indices = i;
        for (size_t j = 0; j < generator.size(); ++j)
        {
            bool f = indices[j];
            if (f)
                comb = row_add(comb, generator[j]);
        }
        all_vectors.push_back(comb);
    }
    return all_vectors;
}

std::vector<size_t> v_space_sig(const std::vector<code_word>& generator)
{
    std::vector<size_t> sizes;
    std::vector<code_word> all_vectors = basis_span(generator);

    for (size_t i = 0; i < all_vectors.size(); ++i)
        sizes.push_back(row_weight(all_vectors[i]));

    sort(sizes.begin(), sizes.end());
    return sizes;
}

std::map<size_t, size_t> sig_table(const std::vector<code_word>& generator)
{
    std::vector<size_t> counts = v_space_sig(generator);
    std::map<size_t, size_t> f_table;
    for (size_t i = 0; i < counts.size(); ++i)
        f_table[counts[i]] += 1;

    return f_table;
}
