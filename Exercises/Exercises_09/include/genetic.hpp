/**
 * @file genetic.hpp
 * @brief stores the class Genetic
 */

#include <numeric>
#include <algorithm>

#include "chromosome.hpp"

/**
 * @class Genetic
 * @brief represents a genetic algorithm for solving the traveling salesman problem.
 *
 * The class manages a population of chromosomes, where each chromosome represents a possible solution to the TSP.
 * The class provides methods for evolving the population through selection, crossover, and mutation.
 * The parameters for mutation and crossover probabilities can be set in the constructor, and are stored as member variables.
 *
 * The fitness of each chromosome is calculated based on the total length of the route it represents, using the L1 distance.
 * The class also provides methods for retrieving the best solution and the average fitness of the best half of the population.
 *
 * The class is designed to work alongside the Cities class, which provides the necessary information about the cities and their distances.
 */
class Genetic
{
    /// @brief the number of chromosomes in the population.
    const unsigned int m_population;

    /// @brief the population of chromosomes, each chromosome represents a possible solution to the TSP.
    std::vector<Chromosome> m_chromosomes;
    /// @brief a dummy vector of chromosomes used for optimization (to avoid creating temporary chromosomes)
    /// @note any value in this vector is considered to be discardable.
    std::vector<Chromosome> m_dummy_chromosomes;
    /// @brief the fitness of each chromosome.
    /// The fitness is cached to be reused by sorting, best_L1_index, best_L1, and average_L1 methods without needing to recalculate it.
    /// The index in the fitness vector corresponds to the index of the chromosome in the m_chromosomes vector.
    std::vector<double> m_fitness;
    /// @brief a dummy vector of fitness values used for optimization (to avoid creating temporary fitness vectors)
    /// @note any value in this vector is considered to be discardable.
    std::vector<double> m_dummy_fitness;

    const double m_p_m_swap = 0.05;   /// probability to mutate swap (applied N times)
    const double m_p_m_shift = 0.1;   /// probability to mutate shift
    const double m_p_m_permute = 0.1; /// probability to mutate permute
    const double m_p_m_invert = 0.05; /// probability to mutate invert
    const double m_p_crossover = 0.6; /// probability to crossover

    /// @brief computes the fitness of each chromosome in the population based on the total length of the route it represents, using the L1 distance.
    /// @param map the map containing the city coordinates and distances
    void calculate_fitness(const Cities &map);
    /// @brief sorts the chromosomes in the population based on their fitness in ascending order (best fitness first).
    void sort_chromosomes();

public:
    /// @brief constructor for the Genetic class.
    /// The probabilities for mutations and crossover are set to default values.
    /// @param population the number of chromosomes in the population.
    /// @param map the map containing the city coordinates and distances.
    /// @param rnd the random number generator.
    Genetic(const unsigned int &population, const Cities &map, Random &rnd);
    /// @brief constructor for the Genetic class.
    /// The probabilities for mutations and crossover are set to the provided values.
    /// @param population the number of chromosomes in the population.
    /// @param map the map containing the city coordinates and distances.
    /// @param prob_swap the probability of performing a swap mutation.
    /// @param prob_shift the probability of performing a shift mutation.
    /// @param prob_permute the probability of performing a permute mutation.
    /// @param prob_invert the probability of performing an invert mutation.
    /// @param prob_crossover the probability of performing a crossover.
    /// @param rnd the random number generator.
    Genetic(const unsigned int &population, const Cities &map, const double &prob_swap, const double &prob_shift, const double &prob_permute, const double &prob_invert, const double &prob_crossover, Random &rnd);

    /// @brief creates a new generation of chromosomes.
    /// The new generation is found by:
    /// 1. Sorting the chromosomes based on their fitness (best fitness first).
    /// 2. Selecting two random parents from the current population using the selection method discussed below.
    /// 3. Recombining the selected parents to create two offspring using the crossover method.
    /// 4. Mutating the offspring.
    /// 5. Repeating from step 2 until the new population is the same size as the original.
    /// 6. Replacing the old population with the new one.
    /// 7. Recomputing the fitness of the new population.
    ///
    /// The selection method is based on the fitness of the chromosomes.
    /// The individuals are selected using the formula:
    ///     \f[j = \text{population_size} \cdot \text{floor}(r ^ p)\f]
    /// where \f$r\f$ is a random number in the range [0,1) and \f$p\f$ is an exponent that can be tuned to increase or decrease the selection pressure.
    /// Since the fitness is sorted from best to worst, a higher value of \f$p\f$ will increase the selection pressure.
    ///
    /// @param map the map containing the city coordinates and distances.
    /// @param exponent the exponent used in the selection method to tune selection pressure.
    /// @param rnd the random number generator.
    void new_generation(const Cities &map, const double &exponent, Random &rnd);

    /// @brief checks if all chromosomes in the population are valid.
    /// @param possible_UIDs a vector containing the UIDs of all possible cities.
    /// @return true if all chromosomes are valid, false otherwise.
    bool check_all_chromosomes(const std::vector<unsigned int> &possible_UIDs) const;

    /// @brief gets the average L1 distance of the best half of the population.
    /// @return the average L1 distance of the best half of the population.
    double average_L1() const;

    /// @brief gets the index of the chromosome with the best (lowest) L1 distance in the population.
    /// @return the index of the chromosome with the best L1 distance.
    unsigned int best_L1_index() const;
    /// @brief gets the best (lowest) L1 distance in the population.
    /// @return the best L1 distance in the population.
    double best_L1() const;

    /// @brief gets a specific chromosome from the population by its index.
    /// @param idx the index of the chromosome to retrieve.
    /// @return a reference to the requested chromosome.
    Chromosome &get_chromosome(const unsigned int &idx) { return m_chromosomes[idx]; }

    /// @brief saves a specific chromosome from the population to a file.
    void save_chromosome(const unsigned int &idx, std::ofstream &fout) const;
};
