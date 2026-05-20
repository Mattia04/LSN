/**
 * Note: this code has been left here only as a reference,
 * for the full commented solution of the exercise please refer to Exer_08_02/main.cpp
 */

#include <iostream>
#include <fstream>
#include <string>
#include "random.h"

using namespace std;

class WaveFunction
{
    double m_mu = 0;
    double m_sigma = 1;

public:
    WaveFunction() = default;
    WaveFunction(const double mu, const double sigma) : m_mu(mu), m_sigma(sigma) {};

    double psi(const double x) const
    {
        return exp(-pow(x - m_mu, 2) / (2. * m_sigma * m_sigma)) + exp(-pow(x + m_mu, 2) / (2. * m_sigma * m_sigma));
    }
    double norm2(const double x) const { return pow(psi(x), 2); }

    double potential(const double x) const
    {
        // return V(x) psi
        return (pow(x, 4) - 5. / 2. * pow(x, 2)) * psi(x);
    }
    double kinetic(const double x) const
    {
        // returns - hbar^2 / 2m * (deriv[2]_x of psi(x))
        return -0.5 * ((x * x + m_mu * m_mu - m_sigma * m_sigma) * psi(x) - 2. * x * m_mu * (exp(-pow(x - m_mu, 2) / (2. * m_sigma * m_sigma)) - exp(-pow(x + m_mu, 2) / (2. * m_sigma * m_sigma)))) / pow(m_sigma, 4);
    }

    double H_applied(const double x) const
    {
        // return H psi
        return kinetic(x) + potential(x);
    }
    double H_psi(const double x) const
    {
        // returns the term (H psi)/psi
        return H_applied(x) / psi(x);
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

class DataBlocking
{
    unsigned int value_counter = 0;
    unsigned int block_counter = 0;

    double block_sum = 0;
    double last_block_ave;

    double running_ave = 0;  // cumulative mean of block averages
    double running_ave2 = 0; // cumulative mean of squared block averages

public:
    DataBlocking() = default;

    void add(const double value)
    {
        block_sum += value;
        value_counter++;
    }
    void reset_block()
    {
        if (value_counter == 0)
            return;

        double block_ave = block_sum / static_cast<double>(value_counter);

        block_counter++;
        running_ave += (block_ave - running_ave) / static_cast<double>(block_counter);
        running_ave2 += (block_ave * block_ave - running_ave2) / static_cast<double>(block_counter);

        last_block_ave = block_ave;

        block_sum = 0;
        value_counter = 0;
    }

    double get_ave() const { return running_ave; }
    double get_ave2() const { return running_ave2; }
    double get_error() const
    {
        if (block_counter <= 1)
            return 0.;
        else
            return sqrt(fabs(running_ave2 - running_ave * running_ave) / static_cast<double>(block_counter - 1));
    }
    double get_ave_blk() const { return last_block_ave; }
};

int main(int argc, char *argv[])
{

    Random rnd;
    int seed[4];
    int p1, p2;
    ifstream Primes("Primes");
    if (Primes.is_open())
    {
        Primes >> p1 >> p2;
    }
    else
        cerr << "PROBLEM: Unable to open Primes" << endl;
    Primes.close();

    ifstream input("seed.in");
    string property;
    if (input.is_open())
    {
        while (!input.eof())
        {
            input >> property;
            if (property == "RANDOMSEED")
            {
                input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
                rnd.SetRandom(seed, p1, p2);
            }
        }
        input.close();
    }
    else
        cerr << "PROBLEM: Unable to open seed.in" << endl;

    // ========== Part 1 ==========
    constexpr unsigned int M = 100000;     // Total number of throws
    constexpr unsigned int N = 1000;       // Number of blocks
    constexpr unsigned int N_BURN_IN = 10; // Skip first 2 blocks

    // Number of throws in each block
    constexpr unsigned int L = M / N;

    WaveFunction WF(0.8, 0.5);

    DataBlocking DB;
    DataBlocking acc;

    double gamma = 2.2;

    double x_old = 1.;

    ofstream fout("hamiltonian.dat");
    fout << "# block Bkl_ave average error" << endl;

    for (int i = 0; i < N + N_BURN_IN; i++)
    { // loop over blocks
        for (int j = 0; j < L; j++)
        { // loop over steps in a block
            const double x_new = x_old + gamma * rnd.Rannyu(-1., 1.);

            const double alpha = min(1., WF.norm2(x_new) / WF.norm2(x_old));

            const double y = rnd.Rannyu();

            if (y < alpha)
                x_old = x_new;

            acc.add(y < alpha ? 1. : 0.);

            if (i >= N_BURN_IN)
                DB.add(WF.H_psi(x_old));
        }

        acc.reset_block();
        cout << acc.get_ave_blk() << " " << acc.get_ave() << endl;

        if (i < N_BURN_IN)
            continue;

        DB.reset_block();
        fout << i - N_BURN_IN + 1 << " " << DB.get_ave_blk() << " " << DB.get_ave() << " " << DB.get_error() << endl;
    }

    cout << "Value of H: " << DB.get_ave() << " +/- " << DB.get_error() << endl;

    rnd.SaveSeed();
    return 0;
}
