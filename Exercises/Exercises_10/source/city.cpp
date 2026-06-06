/**
 * @file city.cpp
 * @brief Implements the modules of the class City, and the class Cities.
 *
 */

#include "city.hpp"

// initializes the static member variable that generates the UIDs of the cities.
unsigned int City::s_next_uid = 1;

// Internal constructor of the Cities class, used by the static initialization methods.
Cities::Cities(const std::vector<City> &cities, const std::vector<unsigned int> &UIDs) : m_cities(cities), m_possible_UIDs(UIDs)
{
    const unsigned int N = m_cities.size();

    // Build the index map
    for (unsigned int i = 0; i < N; ++i)
        m_uid_to_index[m_cities[i].get_UID()] = i;

    // Compute the distance matrix once
    m_distance_matrix.assign(N, std::vector<double>(N, 0.0));

    for (unsigned int i = 0; i < N; ++i)
        for (unsigned int j = i + 1; j < N; ++j)
        {
            double d = Position::l1(m_cities[i].get_pos(), m_cities[j].get_pos());
            m_distance_matrix[i][j] = d;
            m_distance_matrix[j][i] = d; // distance is symmetric
        }
}

Cities Cities::Initialize_Cities_Circle(unsigned int N, double radius, Random &rnd)
{
    std::vector<City> cities;
    std::vector<unsigned int> UIDs;

    cities.reserve(N); // cities does not have a default constructor cannot be resized
    UIDs.reserve(N);

    for (unsigned int i = 0; i < N; ++i)
    {
        const double theta = rnd.Rannyu(0., 2. * M_PI);

        cities.emplace_back(Position{radius * std::cos(theta), radius * std::sin(theta)}); // construct in place of new element
        UIDs.push_back(cities[i].get_UID());
    }

    return {cities, UIDs}; // use the internal constructor of the Cities class
}

Cities Cities::Initialize_Cities_Square(unsigned int N, double length, Random &rnd)
{
    std::vector<City> cities;
    std::vector<unsigned int> UIDs;

    cities.reserve(N); // cities does not have a default constructor cannot be resized
    UIDs.reserve(N);
    for (unsigned int i = 0; i < N; ++i)
    {
        double x = rnd.Rannyu(-length / 2., length / 2.);
        double y = rnd.Rannyu(-length / 2., length / 2.);

        cities.emplace_back(Position{x, y});
        UIDs.push_back(cities[i].get_UID());
    }

    return {cities, UIDs}; // use the internal constructor of the Cities class
}

Cities Cities::Initialize_From_File(const char *filename)
{
    std::ifstream f_in(filename);
    if (!f_in.is_open())
    {
        std::cout << "Could not open the file provided." << std::endl;
        exit(16);
    }

    std::vector<City> cities;
    std::vector<unsigned int> UIDs;

    double x, y;
    while (f_in >> x >> y)
    {

        cities.push_back(Position{x, y});
        UIDs.push_back(cities.back().get_UID());
    }

    return {cities, UIDs};
}

const City &Cities::get_city(const unsigned int idx) const
{
    auto it = m_uid_to_index.find(idx);
    if (it == m_uid_to_index.end())
    {
        std::cout << "UID not found." << std::endl;
        exit(9);
    }
    return m_cities[it->second];
}

const double &Cities::get_distance(const unsigned int &uid1, const unsigned int &uid2) const
{
    const unsigned int i1 = m_uid_to_index.at(uid1);
    const unsigned int i2 = m_uid_to_index.at(uid2);
    return m_distance_matrix[i1][i2];
}

void Cities::save_positions(const char *filename)
{
    std::ofstream fout(filename);

    fout << "UID x y" << std::endl;
    for (const auto &city : m_cities)
    {
        fout << city.get_UID() << " " << city.get_x() << " " << city.get_y() << std::endl;
    }
}
