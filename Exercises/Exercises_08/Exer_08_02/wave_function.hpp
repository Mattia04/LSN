#include <cmath>

class WaveFunction
{
    double m_mu = 0;
    double m_sigma = 1;

public:
    WaveFunction() = default;
    WaveFunction(const double mu, const double sigma) : m_mu(mu), m_sigma(sigma) {};

    double psi(const double x) const
    {
        return std::exp(-std::pow(x - m_mu, 2) / (2. * m_sigma * m_sigma)) + std::exp(-std::pow(x + m_mu, 2) / (2. * m_sigma * m_sigma));
    }
    double norm2(const double x) const { return std::pow(psi(x), 2); }

    double potential(const double x) const
    {
        // return V(x) psi
        return (std::pow(x, 4) - 5. / 2. * std::pow(x, 2)) * psi(x);
    }
    double kinetic(const double x) const
    {
        // returns - hbar^2 / 2m * (deriv[2]_x of psi(x))
        return -0.5 * ((x * x + m_mu * m_mu - m_sigma * m_sigma) * psi(x) - 2. * x * m_mu * (std::exp(-std::pow(x - m_mu, 2) / (2. * m_sigma * m_sigma)) - std::exp(-std::pow(x + m_mu, 2) / (2. * m_sigma * m_sigma)))) / std::pow(m_sigma, 4);
    }

    double H_psi(const double x) const
    {
        // returns the term (H psi)/psi
        return (kinetic(x) + potential(x)) / psi(x);
    }

    double get_mu() { return m_mu; }
    double get_sigma() { return m_sigma; }
    void set_mu(const double mu) { m_mu = mu; }
    void set_sigma(const double sigma) { m_sigma = sigma; }
    void set_params(const double mu, const double sigma)
    {
        m_mu = mu;
        m_sigma = sigma;
    }
};
