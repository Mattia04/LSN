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
#include <functional>
#include <string>
#include "random.h"

using namespace std;

void calculate_integral(const unsigned int N, const unsigned int L, const function<double()> &func, ofstream &out)
{
	double sum, ave;				  // Integral sum and average for the blocks
	double sum_ave = 0, sum_ave2 = 0; // Sums of all averages and averages (sum over blocks)
	double cum_err = 0;				  // Current cumulative error
	// Note: sum_ave and sum_ave2 are the SUMS of averages and averages^2,
	//		 NOT the average of the averages and averages^2
	// 		 so there is an extra factor n and n^2 in the denominators to convert to average
	for (int n = 1; n <= N; n++)
	{
		sum = 0;
		for (int l = 0; l < L; l++)
			sum += func();

		ave = (double)sum / L;
		sum_ave += ave;
		sum_ave2 += ave * ave;

		cum_err = sqrt((sum_ave2 / n - sum_ave * sum_ave / (n * n)) / (n - 1));

		out << sum_ave / n << " " << cum_err << "\n";
	}

	cout << "The integral was evaluated as: " << sum_ave / N << " +/- " << cum_err << endl;
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

	// ========== Part 1 ==========
	constexpr unsigned int M = 100000; // Total number of throws
	constexpr unsigned int N = 100;	   // Number of blocks

	// Number of throws in each block
	constexpr unsigned int L = M / N;

	ofstream f_out_1("averages1.dat");
	function<double()> integral_1 = [&rnd]()
	{ return rnd.Rannyu(); };
	calculate_integral(N, L, integral_1, f_out_1);
	f_out_1.close();

	// ========== Part 2 ==========
	ofstream f_out_2("averages2.dat");
	function<double()> integral_2 = [&rnd]()
	{ return pow(rnd.Rannyu() - 0.5, 2); };
	calculate_integral(N, L, integral_2, f_out_2);
	f_out_2.close();

	// ========== Part 3 ==========
	ofstream f_out_3("chi_sqrd.dat");
	constexpr unsigned int E = 100;			   // Number of experiments
	constexpr unsigned int I = 100;			   // Number of intevals
	constexpr unsigned int J = 10000;		   // Number extractions per inteval
	double y;								   // Extracted value
	unsigned int observed[I] = {0};			   // Number of values inside the interval
	constexpr double expected = (double)J / I; // Expected value
	double chi_2;							   // Chi squared for each experiment

	for (int e = 0; e < E; e++)
	{
		chi_2 = 0;

		for (int j = 0; j < J; j++)
		{
			y = rnd.Rannyu();

			observed[int(y * I)]++;
		}

		for (int i = 0; i < I; i++)
		{
			chi_2 += pow(observed[i] - expected, 2) / expected;
			observed[i] = 0;
		}

		f_out_3 << chi_2 << "\n";
	}

	f_out_3.close();

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
