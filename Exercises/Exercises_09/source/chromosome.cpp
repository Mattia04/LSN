/**
 * @file chromosome.cpp
 * @brief Implements the modules of the class Chromosome.
 */

#include "chromosome.hpp"

void Chromosome::mutate_swap(const unsigned int &idx1, const unsigned int &idx2, const double &prob, Random &rnd)
{
    // this piece of code is used only when debugging
    // for optimization reasons we assume the indices are always valid.
    // if (idx1 >= m_string.size() or idx2 >= m_string.size())
    // {
    //     std::cout << "Mutation index not valid." << std::endl;
    //     exit(12);
    // }

    if (rnd.Rannyu() < prob)
        std::swap(m_string[idx1], m_string[idx2]);
}

void Chromosome::mutate_swap_all(const double &prob, Random &rnd)
{
    for (unsigned int i = 1; i < m_string.size(); ++i)
    {
        unsigned int j = 0;
        // uniform swap (not uniform shuffle) for mutation
        j = rnd.Rannyu(1, m_string.size());

        mutate_swap(i, j, prob, rnd);
    }
}

void Chromosome::mutate_swap_init(Random &rnd)
{
    for (unsigned int i = 1; i < m_string.size(); ++i)
    {
        unsigned int j = 0;
        // Fisher–Yates shuffle (uniform shuffle) for chromosome creation
        j = rnd.Rannyu(i, m_string.size());

        mutate_swap(i, j, 1., rnd);
    }
}

void Chromosome::mutate_shift(const double &prob, Random &rnd)
{
    // this method makes a shift mutation
    // the probability of a mutation is uniform in the length of the shifted block.

    if (rnd.Rannyu() > prob)
        return;

    const unsigned int N = m_string.size();

    // need at least 3 elements for a valid shift
    if (N < 3)
        return;

    const unsigned int length = rnd.Rannyu(1, N);             // block length in [1, N)
    const unsigned int start = rnd.Rannyu(1, N - length + 1); // block start in [1, N-length]
    const unsigned int shift = rnd.Rannyu(1, length);         // shift in [1, length]

    // Rotate the block to the right by 'shift' positions
    std::rotate(m_string.begin() + start,
                m_string.begin() + start + shift,
                m_string.begin() + start + length);
}

void Chromosome::mutate_permute(const double &prob, Random &rnd)
{
    // this method makes a permutation mutation,
    // the probability of a mutation is triangular in the length of the permuted segment (shorter segments have higher probability).

    if (rnd.Rannyu() > prob)
        return;

    const unsigned int N = m_string.size();

    // need at least 3 elements for a valid permutation
    if (N < 3)
        return;

    // Sample 2 cut points in [1, N] uniformly, sort them
    unsigned int startA = static_cast<int>(rnd.Rannyu(1, N));
    unsigned int startB = static_cast<int>(rnd.Rannyu(1, N));

    if (startA > startB)
        std::swap(startA, startB);

    const unsigned int max_len = std::min(startB - startA, N - startB);

    if (max_len < 1)
        return; // nothing to permute

    // Sample length in [1, max_len]
    const unsigned int len = rnd.Rannyu(1, max_len + 1);

    // Swap the two contiguous blocks
    std::swap_ranges(m_string.begin() + startA,
                     m_string.begin() + startA + len,
                     m_string.begin() + startB);
}

void Chromosome::mutate_invert(const double &prob, Random &rnd)
{
    // this method makes an inversion mutation,
    // the probability of a mutation is uniform in the length of the inverted segment.

    if (rnd.Rannyu() > prob)
        return;

    // need at least 3 elements for a valid inversion
    const int N = m_string.size();
    if (N < 3)
        return;

    const unsigned int n = rnd.Rannyu(2, N);             // sample length in [2, N)
    const unsigned int start = rnd.Rannyu(1, N - n + 1); // sample start in [1, N-n]

    std::reverse(m_string.begin() + start, m_string.begin() + start + n);
}

Chromosome::Chromosome(std::vector<unsigned int> possible_UIDs, Random &rnd)
{
    m_string.reserve(possible_UIDs.size());

    // set first element to minimum UID to reduce symmetry of the problem
    m_string.push_back(*std::min_element(possible_UIDs.begin(), possible_UIDs.end()));

    // fill the rest of the chromosome with the remaining UIDs in order
    for (auto &uid : possible_UIDs)
    {
        if (uid == m_string[0])
            continue;

        m_string.push_back(uid);
    }

    // randomly shuffle the chromosome to create a random route
    mutate_swap_init(rnd);
}

Chromosome::Chromosome(std::vector<unsigned int> possible_UIDs, std::vector<unsigned int> string)
{
    // initialize the chromosome with the given string
    m_string.reserve(string.size());
    for (auto &uid : string)
        m_string.push_back(uid);

    if (!check_valid_string(possible_UIDs))
    {
        std::cout << "Input string for Chromosome is invalid." << std::endl;
        exit(10);
    }
}

void Chromosome::mutate(const double &p_swap, const double &p_shift, const double &p_permute, const double &p_invert, Random &rnd)
{
    mutate_swap_all(p_swap, rnd);
    mutate_shift(p_shift, rnd);
    mutate_permute(p_permute, rnd);
    mutate_invert(p_invert, rnd);
}

void Chromosome::recombine(const Chromosome &parent1, const Chromosome &parent2, Chromosome &offspring1, Chromosome &offspring2, double prob, Random &rnd)
{
    if (rnd.Rannyu() > prob)
        return;

    // copy (no crossover)
    offspring1 = parent1;
    offspring2 = parent2;

    const unsigned int N = parent1.m_string.size();

    // cut point in [1, N-2] so both parts are non-empty
    const unsigned int cut = rnd.Rannyu(1, N - 1);

    // keep track of which UIDs are already in the offspring to avoid duplicates
    std::unordered_set<unsigned int> in_offspring1(N), in_offspring2(N);

    for (unsigned int i = 0; i < cut; ++i)
    {
        in_offspring1.insert(parent1.m_string[i]);
        in_offspring2.insert(parent2.m_string[i]);
    }

    // fill the rest of the chromosomes after the cut
    unsigned int idx_s = cut;
    unsigned int idx_d = cut;

    for (unsigned int i = 0; i < N; ++i)
    {
        // fill offspring1 with genes from parent2
        if (!in_offspring1.contains(parent2.m_string[i]))
        {
            offspring1.m_string[idx_s] = parent2.m_string[i];
            in_offspring1.insert(parent2.m_string[i]);
            idx_s++;
        }

        // fill offspring2 with genes from parent1
        if (!in_offspring2.contains(parent1.m_string[i]))
        {
            offspring2.m_string[idx_d] = parent1.m_string[i];
            in_offspring2.insert(parent1.m_string[i]);
            idx_d++;
        }
    }
}

bool Chromosome::check_valid_string(std::vector<unsigned int> possible_UIDs) const
{
    if (m_string.empty())
        return 0;

    if (m_string.size() != possible_UIDs.size())
        return 0;

    const unsigned int min = *std::min_element(possible_UIDs.begin(), possible_UIDs.end());
    if (m_string[0] != min)
        return 0;

    if (!std::is_permutation(m_string.begin(), m_string.end(), possible_UIDs.begin()))
        return 0;

    return 1;
}

double Chromosome::L1(const Cities &map) const
{
    double L1 = 0.;
    const unsigned int N = m_string.size() - 1; // i + 1 overflows mod(N)
    for (unsigned int i = 0; i < N; ++i)
        L1 += map.get_distance(m_string[i], m_string[i + 1]);
    L1 += map.get_distance(m_string[0], m_string[N]); // close cycle

    return L1;
}

void Chromosome::save(std::ofstream &fout) const
{
    for (const auto &allele : m_string)
        fout << allele << " ";
    fout << m_string[0] << std::endl;
}

// -------------------- OLD CODE --------------------
// void Chromosome::mutate_permute(const double &prob, Random &rnd)
// {
//     // this method makes a permutation mutation
//     // the probability of a mutation is uniform in the length of the permuted blocks

//     if (rnd.Rannyu() > prob)
//         return;

//     const unsigned int N = m_string.size();

//     // need at least 3 elements for a valid permutation
//     if (N < 3)
//         return;

//     const unsigned int length = rnd.Rannyu(1, (N - 1) / 2 + 1);              // block length in [1, (N-1)/2]
//     const unsigned int startA = rnd.Rannyu(1, N - 2 * length + 1);           // start of first block in [1, N-2length]
//     const unsigned int startB = rnd.Rannyu(startA + length, N - length + 1); // start of second block in [startA+length, N-length]

//     // Swap the two contiguous blocks
//     std::swap_ranges(m_string.begin() + startA,
//                      m_string.begin() + startA + length,
//                      m_string.begin() + startB);
// }

// void Chromosome::mutate_invert(const double &prob, Random &rnd)
// {
//     // this method makes an inversion mutation,
//     // the probability of a mutation is triangular in the length of the inverted segment (shorter segments have higher probability).

//     if (rnd.Rannyu() > prob)
//         return;

//     // need at least 3 elements for a valid inversion
//     const int N = static_cast<int>(m_string.size());
//     if (N < 3)
//         return;

//     // Sample 2 cut points in [1, N] and sort them
//     unsigned int start = rnd.Rannyu(1, N);
//     unsigned int end = rnd.Rannyu(1, N);
//     if (start > end)
//         std::swap(start, end);

//     const unsigned int n = end - start;

//     if (n < 2)
//         return; // nothing to invert

//     std::reverse(m_string.begin() + start, m_string.begin() + start + n);
// }

// void Chromosome::mutate_shift(const double &prob, Random &rnd)
// {
//     // this method makes a shift mutation
//     // the probability of a mutation is triangular in the length of the shifted segment (shorter segments have higher probability).

//     if (rnd.Rannyu() > prob)
//         return;

//     const unsigned int N = m_string.size();

//     if (N < 3)
//         return; // need at least 3 elements for a valid shift

//     // Sample 2 cut points in [1, N] uniformly, sort them
//     unsigned int start = static_cast<int>(rnd.Rannyu(1, N));
//     unsigned int end = static_cast<int>(rnd.Rannyu(1, N));

//     if (start > end)
//         std::swap(start, end);

//     const unsigned int length = end - start;

//     if (length < 2)
//         return; // nothing to shift

//     // Sample shift in [1, length] (integers only)
//     const unsigned int shift = rnd.Rannyu(1, length + 1);

//     std::rotate(m_string.begin() + start, m_string.begin() + start + shift, m_string.begin() + start + length);
// }
