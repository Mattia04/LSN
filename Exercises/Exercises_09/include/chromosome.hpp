/**
 * @file chromosome.hpp
 * @brief Stores class Chromosome
 */

#include <unordered_set>

#include "city.hpp"

/**
 * @class Chromosome
 * @brief Represents a chromosome in the genetic algorithm.
 *
 * The chromosome is represented as a vector of unsigned integers,
 * where each integer corresponds to the UID of a city.
 *
 * The order of the integers in the vector represents the order in which the cities are visited in the TSP route.
 *
 * The class provides a method to calculate the total length of the route represented by the chromosome.
 *
 * The class provides methods for mutating the chromosome (swap, shift, permute, invert)
 * and for recombining two chromosomes to produce offspring with a crossover.
 *
 * The class also provides a method to check if the chromosome is valid.
 */
class Chromosome
{
    std::vector<unsigned int> m_string; // route of cities UIDs to visit

    /// @brief Swaps the cities at the given indices with a given probability.
    /// @param idx1 index of the first city to swap
    /// @param idx2 index of the second city to swap
    /// @param prob probability of performing the swap
    /// @param rnd random number generator
    void mutate_swap(const unsigned int &idx1, const unsigned int &idx2, const double &prob, Random &rnd);

    /// @brief applies mutate_swap() multiple times.
    /// Each city has a probability 'prob' of being swapped with another random city.
    /// @param prob probability of performing one swap
    /// @param rnd random number generator
    void mutate_swap_all(const double &prob, Random &rnd);

    /// @brief applies mutate_swap() to all city to have a uniform chromosome.
    /// @note this method is used only for chromosome initialization.
    /// @param rnd random number generator
    void mutate_swap_init(Random &rnd);

    /// @brief Shifts a contiguous segment of the chromosome to the right with a given probability.
    /// The shift is performed by selecting a block, rotating its elements to the right, and inserting it back into the chromosome.
    /// The rotation of a block is performed like: ABCDEF -> EFABCDE for a shift of 2.
    /// The size of the block is distributed with a uniform distribution.
    /// The shift of the block is distributed with a uniform distribution.
    /// @param prob probability of performing the shift
    /// @param rnd random number generator
    void mutate_shift(const double &prob, Random &rnd);

    /// @brief Permutes two contiguous segments of the chromosome with a given probability.
    /// The permutation is performed by selecting two blocks and swapping them.
    /// The size of the blocks is distributed with a triangular distribution, so shorter blocks are more likely.
    /// @param prob probability of performing the permutation
    /// @param rnd random number generator
    void mutate_permute(const double &prob, Random &rnd);

    /// @brief Inverts a contiguous segment of the chromosome with a given probability.
    /// The inversion is performed by selecting a block and reversing its order.
    /// The size of the block is distributed with a uniform distribution.
    /// @param prob probability of performing the inversion
    /// @param rnd random number generator
    void mutate_invert(const double &prob, Random &rnd);

    Chromosome() = delete;

public:
    /// @brief Constructs a chromosome with a random route based on the provided possible UIDs.
    /// The constructor initializes the chromosome with a valid route that includes all the possible UIDs exactly once.
    /// The first element of the chromosome is set to the minimum UID to reduce symmetry of the problem.
    /// @param possible_UIDs a vector of possible UIDs to include in the chromosome
    /// @param rnd a random number generator
    Chromosome(std::vector<unsigned int> possible_UIDs, Random &rnd);
    /// @brief Constructs a chromosome with a given route.
    /// The constructor checks if the provided route is valid (contains all possible UIDs exactly once and starts with the minimum UID).
    /// @param possible_UIDs a vector of possible UIDs to include in the chromosome
    /// @param string a vector representing the route of cities to visit
    Chromosome(std::vector<unsigned int> possible_UIDs, std::vector<unsigned int> string);

    /// @brief copy constructor
    /// @param other another chromosome to copy from
    Chromosome(const Chromosome &other) : m_string(other.m_string) {};
    /// @brief copy assignment operator
    /// @param other another chromosome to assign from
    /// @return a reference to the assigned chromosome
    Chromosome &operator=(const Chromosome &other)
    {
        m_string = other.m_string;
        return *this;
    }

    /// @brief mutates the chromosome by applying various mutation operators with given probabilities
    /// Mutation operators include swap, shift, permute, and invert, which are applied sequentially.
    /// @param p_swap probability of performing a swap mutation
    /// @param p_shift probability of performing a shift mutation
    /// @param p_permute probability of performing a permute mutation
    /// @param p_invert probability of performing an invert mutation
    /// @param rnd random number generator
    void mutate(const double &p_swap, const double &p_shift, const double &p_permute, const double &p_invert, Random &rnd);

    /// @brief recombines two parent chromosomes to produce two offspring chromosomes with a given crossover probability.
    /// The crossover is applied with probability 'prob', otherwise the offspring are copies of the parents.
    /// @param parent1 the first parent chromosome
    /// @param parent2 the second parent chromosome
    /// @param offspring1 the first offspring chromosome
    /// @param offspring2 the second offspring chromosome
    /// @param prob the probability of performing the crossover
    /// @param rnd random number generator
    static void recombine(const Chromosome &parent1, const Chromosome &parent2, Chromosome &offspring1, Chromosome &offspring2, double prob, Random &rnd);

    /// @brief checks if the chromosome is valid.
    /// A chromosome is valid if it contains all possible UIDs exactly once and starts with the minimum UID.
    /// @param possible_UIDs a vector of possible UIDs to check against
    /// @return true if the chromosome is valid, false otherwise
    bool check_valid_string(std::vector<unsigned int> possible_UIDs) const;

    /// @brief calculates the total distance of the route represented by the chromosome.
    /// @param map the map containing the city coordinates
    /// @return the total length of the route, using the l1 distance.
    [[nodiscard]] double L1(const Cities &map) const;

    /// @brief gets the route represented by the chromosome.
    /// @return a vector representing the route of cities to visit
    std::vector<unsigned int> get_string() const { return m_string; }

    /// @brief saves the route represented by the chromosome to a file.
    /// The route is saved as a sequence of UIDs, one per line.
    /// @param fout an output file stream to write the route to
    void save(std::ofstream &fout) const;
};
