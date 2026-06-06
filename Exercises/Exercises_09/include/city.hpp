/**
 * @file city.hpp
 * @brief Stores the class City, and the class Cities.
 */

#include "position.hpp"
#include "random.h"
#include <vector>
#include <algorithm>
#include <fstream>

/**
 * @class City
 * @brief represents a unique city.
 *
 * The City class contains the Position of the ciy and a unique identifier (UID).
 *
 * The UID is generated automatically by the class, and is used to identify the city.
 * Two cities will always have different UIDs, even if they have the same position.
 */
class City
{
    static unsigned int s_next_uid;
    const Position m_pos;
    const unsigned int UID;

public:
    /// @brief constructor
    /// @param pos the position of the city
    City(const Position &pos) : m_pos(pos), UID(s_next_uid++) {};

    /// @brief gets the unique identifier of the city
    unsigned int get_UID() const { return UID; }

    /// @brief gets the position of the city
    Position get_pos() const { return m_pos; }
    /// @brief gets the x coordinate of the city
    double get_x() const { return m_pos.get_x(); }
    /// @brief gets the y coordinate of the city
    double get_y() const { return m_pos.get_y(); }
};

/**
 * @class Cities
 * @brief represents a collection of cities.
 *
 * The Cities class contains a vector of City objects,
 * and a vector of possible UIDs (the UIDs of the cities in the collection).
 *
 * The Cities class contains a map that associates each possible UID
 * to the index of the city in the vector, to optimize data retrieval.
 *
 * The Cities class also contains a distance matrix,
 * which contains the precomputed distances between all cities for optimization purposes.
 *
 * The Cities class is designed to be immutable.
 */
class Cities
{
    const std::vector<City> m_cities;
    const std::vector<unsigned int> m_possible_UIDs;
    std::unordered_map<unsigned int, unsigned int> m_uid_to_index; // to optimize data retrival
    std::vector<std::vector<double>> m_distance_matrix;            // to optimize distance calculations

    Cities() = delete;

    // internal constructor
    Cities(const std::vector<City> &cities, const std::vector<unsigned int> &UIDs);

public:
    /**
     * @brief Contructs a collection of cities with random positions on a circle.
     * @param N the number of cities to generate.
     * @param radius the radius of the circle on which the cities will be generated.
     * @param rnd a random number generator.
     * @return a Cities instance.
     */
    static Cities Initialize_Cities_Circle(unsigned int N, double radius, Random &rnd);
    /**
     * @brief Contructs a collection of cities with random positions inside a square.
     * @param N the number of cities to generate.
     * @param length the length of the sides of the square inside which the cities will be generated.
     * @param rnd a random number generator.
     * @return a Cities instance.
     */
    static Cities Initialize_Cities_Square(unsigned int N, double length, Random &rnd);

    /// @brief gets all the UIDs of the cities in the collection.
    const std::vector<unsigned int> &get_possible_UIDs() const { return m_possible_UIDs; };

    /// @brief gets a city by its index.
    /// @param idx the index of the city to retrieve.
    /// @return a reference to the requested city instance.
    const City &get_city(unsigned int idx) const;
    /// @brief gets the distance between two cities by their UIDs.
    /// @param uid1 the UID of the first city.
    /// @param uid2 the UID of the second city.
    /// @return the distance between the two cities.
    const double &get_distance(const unsigned int &uid1, const unsigned int &uid2) const;

    /// @brief saves the positions of the cities in a file.
    /// @param filename the name of the file where to save the positions.
    void save_positions(const char *filename);
};
