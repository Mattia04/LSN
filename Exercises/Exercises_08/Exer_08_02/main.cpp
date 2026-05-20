#include <iostream>
#include <fstream>
#include <string>

#include "random.h"
#include "wave_function.hpp"
#include "data_blocking.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    // ================================================================================
    // Load random number generator
    Random rnd;
    int seed[4];
    int p1, p2;
    ifstream Primes("Primes");
    if (Primes.is_open())
        Primes >> p1 >> p2;
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

    // ================================================================================

    // =============== Exercise 8.2 ===============

    // Parameters for the simulation
    constexpr unsigned int SA_L = 3000;   // Number of Simulated Annealing steps
    constexpr unsigned int N = 1000;      // Number of blocks
    constexpr unsigned int L = 500;       // Number of throws in each block
    constexpr unsigned int N_BURN_IN = 2; // Skip first 2 blocks

    constexpr unsigned int M_FACT = 10;  // multiplication factor for the final evaluation of expected H
    constexpr unsigned int x_BINS = 100; // number of bins for histogram of x (from -3 to 3)

    // Initialize wavefunction with some values of mu and sigma
    WaveFunction WF_c(0.8, 0.5); // current wavefunction
    WaveFunction WF_p(0.8, 0.5); // previous wavefunction

    // acceptance of proposed changes of x and (mu, sigma)
    DataBlocking acc_x; // exploit Datablocking to calculate mean acceptance of proposed x
    DataBlocking acc_p; // exploit Datablocking to calculate mean acceptance of proposed parameters mu and sigma

    // maximum stepsizes for proposed changes of x, mu and sigma
    double gamma_x = 2.2;      // maximum stepsize of x
    double gamma_mu = 0.01;    // maximum stepsize of mu
    double gamma_sigma = 0.01; // maximum stepsize of sigma

    // The temperature is decreased at each step of the simulated annealing as T = 1 / (1 + sa * cooling_constant)
    // and after cooling_steps steps the temperature is futher cooled down by multiplying it by cooling_factor
    double T = 1.;                          // starting temperature for annealing
    const double cooling_constant = 0.1;    // controls how fast the annealing temperatures cools down
    const double cooling_factor = 0.98;     // cooles the annealing temperatures
    const unsigned int cooling_steps = 100; // number of steps after which the temperature is cooled

    double x_c, x_p = 1.; // current position, previous position
    double E_c, E_p = 0.; // current energy, previous energy

    ofstream SAparams("data/paramters.dat");
    SAparams << "# SA_step mu sigma" << endl;

    ofstream SA_H("data/expected_H_annealing.dat");
    SA_H << "# SA_step expected_H error_H" << endl;

    // Simulated Annealing loop
    for (int sa = 0; sa < SA_L; ++sa)
    {
        E_c = 0.;

        // propose non negative values of mu and sigma
        do
        {
            WF_c.set_params(WF_p.get_mu() + gamma_mu * rnd.Rannyu(-1., 1.),
                            WF_p.get_sigma() + gamma_sigma * rnd.Rannyu(-1., 1.));
        } while (WF_c.get_mu() < 0 or WF_c.get_sigma() < 0);

        // Evaluate expected value of H
        DataBlocking H;
        for (unsigned int bkl = 0; bkl < N + N_BURN_IN; ++bkl)
        {
            for (int l = 0; l < L; ++l)
            {
                x_c = x_p + gamma_x * rnd.Rannyu(-1., 1.);

                const double prob = WF_c.norm2(x_c) / WF_c.norm2(x_p);
                const bool cond = rnd.Rannyu() < min(1., prob);

                if (cond)
                    x_p = x_c;

                acc_x.add(cond ? 1. : 0.);

                if (bkl >= N_BURN_IN)
                {
                    E_c += WF_c.H_psi(x_p);
                    H.add(WF_c.H_psi(x_p));
                }
            }

            H.reset_block();
        }
        E_c /= static_cast<double>(N * L);

        //  informations to keep track of the annealing process
        if (sa % 100 == 0)
        {
            cout << "SA " << sa << ": E_c=" << fixed << setprecision(8) << E_c << " E_p=" << E_p
                 << " dE=" << (E_c - E_p) << " T=" << T << " -> T * dE=" << (E_c - E_p) / T << endl;
        }

        const double prob = WF_p.get_sigma() / WF_c.get_sigma() * exp(-(E_c - E_p) / T);
        // the factor WF_c.get_sigma() / WF_p.get_sigma() is needed to take into account
        //      the normalization factor of the wavefunction, which is proportional to 1 /sigma

        const bool cond = rnd.Rannyu() < min(1., prob);

        if (cond)
        {
            E_p = E_c;
            WF_p.set_params(WF_c.get_mu(), WF_c.get_sigma());
        }

        acc_p.add(cond ? 1. : 0.);

        // save data to file
        SAparams << sa << fixed << setprecision(8) << " " << WF_p.get_mu() << " " << WF_p.get_sigma() << endl;
        SA_H << sa << fixed << setprecision(8) << " " << H.get_ave() << " " << H.get_error() << endl;

        // cool down temperature
        T = pow(cooling_factor, static_cast<unsigned int>(sa / cooling_steps)) / (1. + sa * cooling_constant);
    }

    // Final informations about the annealing
    cout << "Final mu " << WF_p.get_mu() << endl;
    cout << "Final sigma " << WF_p.get_sigma() << endl;

    acc_p.reset_block();
    cout << "acceptance of mu and sigma: " << acc_p.get_ave() << endl;

    // =============== Last Evaluation of H (equivalent to Exercise 8.1) ===============
    // this last piece of code evaluated H using the optimized parameters and a larger number of samples
    DataBlocking DB;    // datablocking for expected H
    int hist[x_BINS]{}; // histogram for sampled psi(x)

    ofstream H_out("data/H_expected.dat");
    H_out << "# block_number expected_H error_H" << endl;

    ofstream hist_out("data/histogram.dat");
    hist_out << "# x_bin_center probability" << endl;

    // datablocking loop
    for (unsigned int bkl = 0; bkl < N * M_FACT + N_BURN_IN; ++bkl)
    {
        for (int l = 0; l < L * M_FACT; ++l)
        {
            x_c = x_p + gamma_x * rnd.Rannyu(-1., 1.);

            const double prob = WF_c.norm2(x_c) / WF_c.norm2(x_p);
            const bool cond = rnd.Rannyu() < min(1., prob);

            if (cond)
                x_p = x_c;

            acc_x.add(cond ? 1. : 0.);

            if (bkl >= N_BURN_IN)
            {
                DB.add(WF_c.H_psi(x_p));

                // fill histogram of x in the range [-3, 3]
                const int bin = static_cast<int>((x_p + 3.) / 6. * x_BINS);
                if (bin >= 0 and bin < x_BINS)
                    hist[bin]++;
            }
        }

        DB.reset_block();
        acc_x.reset_block();

        // save average and error of H for each block, skipping burn-in blocks
        if (bkl >= N_BURN_IN)
            H_out << bkl - N_BURN_IN << fixed << setprecision(8) << " " << DB.get_ave() << " " << DB.get_error() << endl;
    }

    // save histogram of x to file
    // the factor  (x_BINS / 6) / (N * L * M_FACT * M_FACT) is needed to normalize
    for (int i = 0; i < x_BINS; ++i)
        hist_out << -3. + i * 6. / x_BINS << " " << (x_BINS / 6.) * hist[i] / static_cast<double>(N * L * M_FACT * M_FACT) << endl;

    // final results
    cout << "Final value of H: " << DB.get_ave() << " ±" << DB.get_error() << endl;
    cout << "acceptance of x: " << acc_x.get_ave() << endl;

    rnd.SaveSeed();
    return 0;
}
