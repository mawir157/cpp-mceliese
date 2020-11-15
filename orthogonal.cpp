#include "orthogonal.h"

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
            recursively_build(r_copy, depth - 1, i, odd_words,
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
        matrix row_1 = { first };
        if (verbose)
        {
            std::cout << "First code_word: ";
            print_matrix(row_1);
            std::cout << std::endl;
        }

        recursively_build(row_1, n - 1, breaks[block], odd_words,
                          matrices, verbose);

        // shift up by 2 bits then set the new bits to 1.
        first <<= 2;
        first.set(0);
        first.set(1);

        ++block;
    }
    return matrices;
}

std::vector<code_word> basis_span(const matrix& generator)
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

    std::sort(all_vectors.begin(), all_vectors.end(),
              [](const code_word& c1, const code_word& c2)
                 { return row_weight(c1) <  row_weight(c2); });

    return all_vectors;
}

std::vector<size_t> v_space_sig(const matrix& generator)
{
    std::vector<size_t> sizes;
    std::vector<code_word> all_vectors = basis_span(generator);

    for (size_t i = 0; i < all_vectors.size(); ++i)
        sizes.push_back(row_weight(all_vectors[i]));

    return sizes;
}

std::map<size_t, size_t> sig_table(const matrix& generator)
{
    std::vector<size_t> counts = v_space_sig(generator);
    std::map<size_t, size_t> f_table;
    for (size_t i = 0; i < counts.size(); ++i)
        f_table[counts[i]] += 1;

    return f_table;
}

// (m1, m2) -> | m1 00 |
//             | 00 m2 |
matrix semi_direct_product(const matrix& m1, const matrix& m2,
                           const size_t n_bits)
{
    matrix product = m1;
    for (size_t i = 0; i < product.size(); ++i)
        product[i] <<= n_bits;

    for (size_t i = 0; i < m2.size(); ++i)
        product.push_back(m2[i]);

    return product;
}

std::vector<matrix> all_divide_and_conquer(const uint64_t n,
                                           const bool verbose)
{
    std::vector<matrix> matrices;
    std::vector<matrix> products;
    std::vector<matrix> irreducibles;

    for (size_t tl_dimn = 1; tl_dimn <= (n / 2); ++tl_dimn)
    {
        std::vector<matrix> mats1 = all_divide_and_conquer(tl_dimn, false);
        std::vector<matrix> mats2 = all_divide_and_conquer(n - tl_dimn, false); 

        for (size_t i = 0; i < mats1.size(); ++i)
        {
            for (size_t j = 0; j < mats2.size(); ++j)
            {
                matrix prod = semi_direct_product(mats1[i],mats2[j], n - tl_dimn);
                products.push_back(canonise(prod));
            }
        }
      
    }

    std::vector<size_t> breaks;
    std::vector<code_word> odd_words = words_with_odd_weight(n, breaks);

    // up to permurations all first lines are equivalent upto weight
    code_word max = 0;
    for (size_t i = 0; i < n; ++i)
        max.set(i);

    code_word first = 1;
    first <<= 2;    // i think we cannot have a row with weight exactly 1  
    first.set(0);   // so the next smallest weight is 0..00111
    first.set(1);    

    size_t block = 1;
    while (first.to_ullong() + 1 < max.to_ullong())
    {
// clock_t startTime = clock();
        matrix row_1 = { first };
        if (verbose)
        {
            std::cout << "First code_word: ";
            print_matrix(row_1);
            std::cout << std::endl;
        }

        recursively_build(row_1, n - 1, breaks[block], odd_words,
                          matrices, verbose);

        // shift up by 2 bits then set the new bits to 1.
        first <<= 2;
        first.set(0);
        first.set(1);

// std::cout << "["<< block << "] *****"
//           << " (" << matrices.size() << ") "
//           << double( clock() - startTime ) / (double)CLOCKS_PER_SEC
//           << " seconds." << std::endl;
        ++block;
    }

    matrices.insert( matrices.end(), products.begin(), products.end() );
    return matrices;
}
