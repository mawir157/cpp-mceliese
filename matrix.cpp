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
