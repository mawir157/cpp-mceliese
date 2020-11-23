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

std::vector<size_t> column_weights(const matrix& rows, const size_t n_bits)
{
    std::vector<size_t> cols(n_bits);

    for (size_t i=0; i < rows.size(); ++i)
    {
        for (size_t j=0; j < n_bits; ++j)
        {
            cols[j] += (rows[i][j] ? 1 : 0);
        }
    }
    return cols;
}

bool col_weight_acc(const matrix& rows, const size_t n_bits)
{
    const matrix trs = transpose(rows, n_bits);

    for (size_t i = 1; i < trs.size(); ++i)
        if (row_weight(trs[i-1]) < row_weight(trs[i]))
            return false;

    return true;
}

size_t row_weight(const code_word& r)
{
    return r.count();
}

bool row_weight_dec(const matrix& rows)
{
    for (size_t i = 1; i < rows.size(); ++i)
        if (row_weight(rows[i-1]) > row_weight(rows[i]))
            return false;

    return true;
}

matrix transpose(const matrix& m, const size_t n_bits)
{
    matrix tp;
    for (size_t i = 0; i < n_bits; ++i)
    {
        code_word wd = 0;
        for (size_t j = 0; j < m.size(); ++j)
        {
            wd <<= 1;
            wd |= ((m[j] >> i) & BS1);
        }
        tp.push_back(wd);
    }
    std::reverse(tp.begin(), tp.end());
    return tp;
}

matrix identity(const size_t n)
{
    matrix id(n); // 
    for (size_t i = 0; i < n; ++i)
        id[i].set(i);

    std::reverse(id.begin(), id.end());
    return id;
}

// A * B
matrix multiply(const matrix& A, const size_t bitsA,
                const matrix& B, const size_t bitsB)
{
    matrix product(B.size());

    if (bitsA != B.size())
        std::cout << "All bets are off, we are going to seg-fault!";

    matrix B_trans = transpose(B, bitsB);
    std::reverse(B_trans.begin(), B_trans.end());
    matrix A_rev = A;
    std::reverse(A_rev.begin(), A_rev.end());

    for (size_t r = 0; r < A_rev.size(); ++r)
    {
        for (size_t c = 0; c < B_trans.size(); ++c)
        {
            size_t dot = row_dot(A_rev[r], B_trans[c]);
            if (1 == dot)
                product[r].set(c);
        }
    }
    std::reverse(product.begin(), product.end());

    return product;
}
// we assume M is square
matrix invert(const matrix& M)
{
    matrix A = M;
    const size_t dimn = A.size();
    matrix id = identity(dimn);
    // we are going to do everything backwards to make the indices nicer
    std::reverse(A.begin(), A.end());
    std::reverse(id.begin(), id.end());

    // set the ith bit
    for (size_t i = 0; i < dimn; ++i)
    {
        // find the first word where the ith bit is non-zero
        size_t pivot = 0;
        for (size_t j = i; j < dimn; ++j)
        {
            if (A[j][i])
            {
                pivot = j;
                break;
            }
        }
        // swap the pivot row with the ith row
        if (i != pivot)
        {
            std::iter_swap(A.begin() + pivot,   A.begin() + i);
            std::iter_swap(id.begin() + pivot, id.begin() + i);
        }

        // use the pivot row cancel the ith bit of all subsequent rows
        for (size_t j = 0; j < dimn; ++j)
        {
            if (i == j)
                continue;

            if (A[j][i])
            {
                A[j] = row_add(A[j],A[i]);
                id[j] = row_add(id[j],id[i]);
            }
        }
    }
    // put back the right way around
    std::reverse(A.begin(), A.end());
    std::reverse(id.begin(), id.end());
    return id;
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

bool row_order(const code_word& lhs, const code_word& rhs)
{
    if (row_weight(lhs) != row_weight(rhs))
        return row_weight(lhs) < row_weight(rhs);

    return lhs.to_ullong() > rhs.to_ullong();
}

matrix canonise(const matrix& cw)
{
    matrix canon = cw;
    const size_t n = canon.size();

    for (size_t i = 0; i < 2*n; ++i)
    {
        canon = transpose(canon, n);
        std::sort( canon.begin(), canon.end(), row_order);

        canon = transpose(canon, n);
        std::sort( canon.begin(), canon.end(), row_order);
    }
    return canon;
}

void recursively_build(matrix rows,
                       size_t depth,
                       const code_word max_row,
                       std::vector<matrix>& matrices,
                       const bool verbose)
{
    if (depth == 0)
    {
        rows = canonise(rows);
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

void recursively_build_new(matrix rows,
                           size_t depth,
                           const size_t cur_index,
                           const std::vector<code_word> odd_words,
                           std::vector<matrix>& matrices,
                           const bool verbose)
{
    if (depth == 0)
    {
        rows = canonise(rows);
        if (verbose)
            print_matrix(rows);
        matrices.push_back(rows);
        return;
    }

    const size_t last_row_wt = row_weight(rows.back());

    for (size_t i = cur_index + 1; i < odd_words.size(); ++i)
    {
        code_word rw = odd_words[i];

        bool ok = (row_weight(rw) >= last_row_wt);

        if (ok)
        {
            for (uint64_t j = 0; j < rows.size(); ++j)
            {
                if (1 == row_dot(rows[j], rw))
                {
                    ok = false;
                    break;
                }
            }
        }

        if (ok)
        {
            matrix r_copy = rows;
            r_copy.push_back(rw);
            recursively_build_new(r_copy, depth - 1, i, odd_words,
                                  matrices, verbose);
        }
    }
    return;
}

std::vector<matrix> all(const uint64_t n, const bool verbose)
{
    std::vector<matrix> matrices;
    std::vector<size_t> breaks;

    std::vector<code_word> odd_words = words_with_odd_weight(n, breaks);

    // up to permurations all first lines are equivalent upto weight
    code_word max = 0;
    for (size_t i = 0; i < n; ++i)
        max.set(i);

    code_word first = 1;

    size_t block = 0;
    while (first.to_ullong() + 1 < max.to_ullong())
    {
clock_t startTime = clock();
        matrix row_1 = { first };
        if (verbose)
        {
            std::cout << "First code_word: ";
            print_matrix(row_1);
            std::cout << std::endl;
        }

        recursively_build_new(row_1, n - 1, breaks[block], odd_words,
                              matrices, verbose);

        // shift up by 2 bits then set the new bits to 1.
        first <<= 2;
        first.set(0);
        first.set(1);

std::cout << "["<< block << "] *****"
          << " (" << matrices.size() << ") "
          << double( clock() - startTime ) / (double)CLOCKS_PER_SEC
          << " seconds." << std::endl;
        ++block;
    }
    return matrices;
}

matrix find(const size_t n, const size_t bits)
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

matrix find(const size_t n)
{
    matrix m = identity(n);

    // do 100 loops
    for (size_t step = 0; step < 100; ++step)
    {
        for (size_t row = 0; row < n; ++row)
        {
            size_t c = rand() % n;// do this properly later
            while(c == row)
                c = rand() % n;

            m[row] = row_add(m[row], m[c]);
        }
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

std::vector<code_word> words_with_at_most_n_bits(const size_t n,
                                                 const size_t max_bits)
{
    std::vector<code_word> words;
    std::vector<size_t> ns; // ns = [0,1,2,..,max_bits - 1]
    for(size_t i = 0; i < max_bits; ++i)
        ns.push_back(i);

    for (size_t m = 1; m <= n; ++m)
    {
        std::vector<std::vector<size_t>> cs;
        std::vector<size_t> comb;
        combs(cs, ns, comb, 0, m);

        for (size_t i = 0; i < cs.size(); ++i)
            words.push_back(vec_to_code_word(cs[i]));
    }
    return words;
}

std::vector<code_word> words_with_odd_weight(const size_t max_bits,
                                             std::vector<size_t>& breaks)
{
    std::vector<code_word> words;
    std::vector<size_t> ns; // ns = [0,1,2,..,max_bits - 1]
    for(size_t i = 0; i < max_bits; ++i)
        ns.push_back(i);

    for (size_t m = 1; m <= max_bits; m += 2)
    {
        std::vector<std::vector<size_t>> cs;
        std::vector<size_t> comb;
        combs(cs, ns, comb, 0, m);

        for (size_t i = 0; i < cs.size(); ++i)
            words.push_back(vec_to_code_word(cs[i]));

        breaks.push_back(breaks.size());
    }
    return words; 
}

void combs(std::vector<std::vector<size_t>>& cs, const std::vector<size_t>& ss,
           std::vector<size_t>& combination,
           const size_t offset, const size_t k) {
  if (k == 0)
  {
    cs.push_back(combination);
    return;
  }
  for (size_t i = offset; i <= ss.size() - k; ++i)
  {
    combination.push_back(ss[i]);
    combs(cs, ss, combination, i+1, k-1);
    combination.pop_back();
  }
  return;
}
