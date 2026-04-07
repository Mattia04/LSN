/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include "random.h"

using namespace std;

double get_random_circle(Random &rnd)
{
    // https://mathworld.wolfram.com/CirclePointPicking.html
    double x1, x2;
    do
    {
        x1 = rnd.Rannyu(-1, 1);
        x2 = rnd.Rannyu(-1, 1);
    } while (x1 * x1 + x2 * x2 > 1);

    return x1 / sqrt(x1 * x1 + x2 * x2);
}

int main(int argc, char *argv[])
{
    // set the random seed
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

    constexpr double L = 1.f;  // Length of the needle
    constexpr double d = 1.1f; // Lines distance

    constexpr unsigned int M = 10000; // Number of blocks
    constexpr unsigned int N = 10000; // Throws per block

    double pi_block;

    unsigned int hits;
    double sum_pi = 0, sum_pi2 = 0;

    ofstream f_pi("pi.dat");
    f_pi << std::fixed << std::setprecision(10);

    double y1, y2;
    for (int m = 1; m <= M; m++)
    {
        hits = 0;
        for (int n = 0; n < N; n++)
        {
            y1 = rnd.Rannyu(0, d);
            y2 = y1 + L * get_random_circle(rnd);

            if (y2 <= 0 or y2 >= d)
                hits++;

            if (y2 > 0 and y1 == 0)
                hits++;
        }

        pi_block = 2 * L * N / (d * hits);
        sum_pi += pi_block;
        sum_pi2 += pi_block * pi_block;

        f_pi << sum_pi / m << " " << sqrt((sum_pi2 - sum_pi * sum_pi / (double)m) / ((m - 1) * m)) << "\n";
    }

    f_pi.close();

    rnd.SaveSeed();
    return 0;
}

/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/
