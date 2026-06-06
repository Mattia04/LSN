/**
 * @file genetic.cpp
 * @brief Implementation of the Genetic class methods.
 */

#include "genetic.hpp"

void Genetic::calculate_fitness(const Cities &map)
{
    for (unsigned int i = 0; i < m_population; ++i)
        m_fitness[i] = m_chromosomes[i].L1(map);
}

void Genetic::sort_chromosomes()
{
    // Create indices and sort them by precomputed fitness (not the chromosomes)
    std::vector<unsigned int> indexes(m_population);
    std::iota(indexes.begin(), indexes.end(), 0); // same as numpy's np.arange()

    // Sort the population using precomputed fitness
    auto comparator = [this](unsigned int a, unsigned int b)
    { return m_fitness[a] < m_fitness[b]; };
    std::sort(indexes.begin(), indexes.end(), comparator);

    m_dummy_chromosomes.clear(); // size = 0, capacity = m_population
    m_dummy_fitness.clear();     // size = 0, capacity = m_population

    for (unsigned int idx : indexes)
    {
        m_dummy_chromosomes.push_back(m_chromosomes[idx]);
        m_dummy_fitness.push_back(m_fitness[idx]);
    }

    m_chromosomes.swap(m_dummy_chromosomes);
    m_fitness.swap(m_dummy_fitness);

    return;
    // // Reorder chromosomes in-place using sorted indices
    // std::vector<Chromosome> temp_chromosomes;
    // temp_chromosomes.reserve(m_population);

    // // Reorder fitness in-place using sorted indices
    // std::vector<double> temp_fitness;
    // temp_fitness.reserve(m_population);

    // for (unsigned int idx : indexes)
    // {
    //     temp_chromosomes.push_back(m_chromosomes[idx]);
    //     temp_fitness.push_back(m_fitness[idx]);
    // }

    // // Save sorted data to the population
    // m_chromosomes.swap(temp_chromosomes);
    // m_fitness.swap(temp_fitness);
}

Genetic::Genetic(const unsigned int &population, const Cities &map, Random &rnd) : m_population(population)
{
    m_chromosomes.reserve(m_population);
    m_fitness.resize(m_population);

    m_dummy_chromosomes.reserve(m_population);
    m_dummy_fitness.resize(m_population);

    // initialize the population of chromosomes with random routes inplace
    for (unsigned int i = 0; i < population; ++i)
        m_chromosomes.emplace_back(map.get_possible_UIDs(), rnd);

    // compute the fitness of the initial population
    this->calculate_fitness(map);
    this->sort_chromosomes();
}

Genetic::Genetic(const unsigned int &population, const Cities &map, const double &prob_swap, const double &prob_shift, const double &prob_permute, const double &prob_invert, const double &prob_crossover, Random &rnd)
    : m_population(population), m_p_m_swap(prob_swap), m_p_m_shift(prob_shift), m_p_m_permute(prob_permute), m_p_m_invert(prob_invert), m_p_crossover(prob_crossover)
{
    m_chromosomes.reserve(m_population);
    m_fitness.resize(m_population);

    m_dummy_chromosomes.reserve(m_population);
    m_dummy_fitness.resize(m_population);

    // initialize the population of chromosomes with random routes inplace
    for (unsigned int i = 0; i < population; ++i)
        m_chromosomes.emplace_back(map.get_possible_UIDs(), rnd);

    // compute the fitness of the initial population
    this->calculate_fitness(map);
    this->sort_chromosomes();
}

void Genetic::new_generation(const Cities &map, const double &exponent, Random &rnd)
{
    m_dummy_chromosomes.clear(); // size = 0, capacity = m_population

    const unsigned int half = m_population / 2;
    for (unsigned int idx = 0; idx < half; ++idx)
    {
        // extract two (different) indexes
        const unsigned int i = m_population * std::pow(rnd.Rannyu(), exponent);
        unsigned int j = (m_population - 1) * std::pow(rnd.Rannyu(), exponent);
        if (j >= i)
            ++j;

        // chromosomes to be crossed and mutated
        m_dummy_chromosomes.push_back(m_chromosomes[i]); // offspring1
        m_dummy_chromosomes.push_back(m_chromosomes[j]); // offspring2

        // crossover
        Chromosome::recombine(m_chromosomes[i], m_chromosomes[j], m_dummy_chromosomes[2 * idx], m_dummy_chromosomes[2 * idx + 1], m_p_crossover, rnd);

        // mutations
        m_dummy_chromosomes[2 * idx].mutate(m_p_m_swap, m_p_m_shift, m_p_m_permute, m_p_m_invert, rnd);
        m_dummy_chromosomes[2 * idx + 1].mutate(m_p_m_swap, m_p_m_shift, m_p_m_permute, m_p_m_invert, rnd);
    }

    // copy new chromosomes to the population
    m_chromosomes.swap(m_dummy_chromosomes);

    // recompute fitness and sort
    this->calculate_fitness(map);
    this->sort_chromosomes();

    return;

    // // TEST FOR ELITARISM, it was not a big impact, so it was left out
    // // // elitarism save the best 2
    // // temp_chromosomes[0] = m_chromosomes[0];
    // // temp_chromosomes[1] = m_chromosomes[1];

    // // ==================== Offspring creation ====================
    // std::vector<Chromosome> temp_chromosomes;
    // temp_chromosomes.reserve(m_population);

    // // son and daughter have fixed index so they are not overwritten
    // const unsigned int half = m_population / 2;
    // for (unsigned int idx = 0; idx < half; ++idx)
    // {
    //     // extract two (different) indexes
    //     const unsigned int i = m_population * std::pow(rnd.Rannyu(), exponent);
    //     unsigned int j = (m_population - 1) * std::pow(rnd.Rannyu(), exponent);
    //     if (j >= i)
    //         ++j;

    //     temp_chromosomes.push_back(m_chromosomes[i]); // son
    //     temp_chromosomes.push_back(m_chromosomes[j]); // daughter

    //     // crossover
    //     // temp_chromosomes will be overwritten
    //     Chromosome::recombine(m_chromosomes[i], m_chromosomes[j], temp_chromosomes[2 * idx], temp_chromosomes[2 * idx + 1], m_p_crossover, rnd);

    //     // mutations
    //     temp_chromosomes[2 * idx].mutate(m_p_m_swap, m_p_m_shift, m_p_m_permute, m_p_m_invert, rnd);
    //     temp_chromosomes[2 * idx + 1].mutate(m_p_m_swap, m_p_m_shift, m_p_m_permute, m_p_m_invert, rnd);
    // }

    // // copy new chromosomes to the population
    // m_chromosomes.swap(temp_chromosomes);

    // // recompute fitness
    // this->calculate_fitness(map);
    // this->sort_chromosomes();
}

bool Genetic::check_all_chromosomes(const std::vector<unsigned int> &possible_UIDs) const
{
    for (const auto &chromosome : m_chromosomes)
        if (!chromosome.check_valid_string(possible_UIDs))
            return false;

    return true;
}

double Genetic::average_L1() const
{
    double sum = std::accumulate(m_fitness.begin(), m_fitness.begin() + m_fitness.size() / 2, 0.0);
    return 2 * sum / static_cast<double>(m_population);
}

unsigned int Genetic::best_L1_index() const
{
    // min_element returns an iterator to the minimum element, so we subtract the beginning of the vector to get the index
    return std::min_element(m_fitness.begin(), m_fitness.end()) - m_fitness.begin();
}

double Genetic::best_L1() const
{
    return m_fitness[this->best_L1_index()];
}

void Genetic::save_chromosome(const unsigned int &idx, std::ofstream &fout) const
{
    m_chromosomes[idx].save(fout);
}
