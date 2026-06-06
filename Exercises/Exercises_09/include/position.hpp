/**
 * @file position.hpp
 * @brief stores the Position class.
 */

#include <iostream>
#include <cmath>

/**
 * @class Position
 * @brief represents a position in a 2D space.
 *
 * The Position class has two member variables, m_x and m_y, which represent the x and y coordinates of the position.
 */
class Position
{
    double m_x, m_y;

public:
    /// @brief default constructor, initializes the position to (0, 0).
    Position() : m_x(0), m_y(0) {};
    /// @brief constructor that initializes the position to the given x and y coordinates.
    /// @param x The x coordinate.
    /// @param y The y coordinate.
    Position(double x, double y) : m_x(x), m_y(y) {};
    /// @brief copy constructor
    /// @param pos Another instance of Position.
    Position(const Position &pos) : m_x(pos.m_x), m_y(pos.m_y) {};

    /** @brief calculates the l1 distance between two positions.
     * @param pos1 The first position.
     * @param pos2 The second position.
     * @return The l1 distance between the two positions.
     *
     * The l1 distance is calculated as:
     * \f[L_1 = \sqrt{(x_2 - x_1)^2 + (y_2 - y_1)^2} \f]
     */
    [[nodiscard]] static double l1(const Position &pos1, const Position &pos2)
    {
        return std::sqrt(std::pow(pos2.m_x - pos1.m_x, 2) + std::pow(pos2.m_y - pos1.m_y, 2));
    };

    /// @brief gets the x coordinate of the position.
    const double &get_x() const { return m_x; }
    /// @brief gets the y coordinate of the position.
    const double &get_y() const { return m_y; }
};
