/****************************************************************
*****************************************************************
	_/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include "random.h"

using namespace std;

void do_step(double &x, double &y, double &z, Random &rnd, const double a)
{
	int direction = rnd.Rannyu(0, 6);
	switch (direction)
	{
	case 0:
		x += a;
		break;
	case 1:
		x -= a;
		break;
	case 2:
		y += a;
		break;
	case 3:
		y -= a;
		break;
	case 4:
		z += a;
		break;
	case 5:
		z -= a;
		break;

	default:
		exit(1);
		break;
	}
}

void do_step_continuos(double &x, double &y, double &z, Random &rnd, const double a)
{
	double theta = rnd.Rannyu(0, M_PI);
	double phi = rnd.Rannyu(0, 2 * M_PI);

	x += a * sin(theta) * cos(phi);
	y += a * cos(theta) * cos(phi);
	z += a * sin(phi);
}

double norm2(const double &x, const double &y, const double &z)
{
	return x * x + y * y + z * z;
}

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
	constexpr unsigned int M = 10000; // Number of simulations
	constexpr unsigned int N = 100;	  // Number of throws per block
	constexpr unsigned int I = 100;	  // Number of steps per simulation

	constexpr unsigned int L = M / N; // Number of blocks

	constexpr double a = 1.; // Lattice constant

	double sum_pos[I];		 // Sum of norm of positions
	double sum_ave[I] = {};	 // Sum of averages over blocks
	double sum_ave2[I] = {}; // Sum of averages squared over blocks

	double cum_err;

	for (int l = 0; l < L; l++)
	{
		for (int i = 0; i < I; i++)
			sum_pos[i] = 0.;

		for (int n = 0; n < N; n++)
		{
			double x = 0, y = 0, z = 0; // Coordinates
			for (int i = 0; i < I; i++)
			{
				do_step(x, y, z, rnd, a);

				sum_pos[i] += norm2(x, y, z);
			}
		}

		for (int i = 0; i < I; i++)
		{
			// Convert sums to averages
			sum_pos[i] = sqrt(sum_pos[i] / N);

			sum_ave[i] += sum_pos[i] / L;
			sum_ave2[i] += sum_pos[i] * sum_pos[i] / L;
		}
	}

	ofstream f_out("steps_discrete.dat");
	f_out << 0 << " " << "nan" << "\n";
	for (int i = 0; i < I; i++)
	{
		cum_err = sqrt((sum_ave2[i] - sum_ave[i] * sum_ave[i]) / (L - 1));

		f_out << sum_ave[i] << " " << cum_err << "\n";
	}
	f_out.close();

	// ========== Part 2 ==========

	// Reset arrays
	for (int i = 0; i < I; i++)
	{
		sum_ave[i] = 0.;
		sum_ave2[i] = 0.;
	}

	for (int l = 0; l < L; l++)
	{
		for (int i = 0; i < I; i++)
			sum_pos[i] = 0.;

		// execute one block
		for (int n = 0; n < N; n++)
		{
			double x = 0, y = 0, z = 0; // Coordinates

			// Run one simulation
			for (int i = 0; i < I; i++)
			{
				do_step_continuos(x, y, z, rnd, a);

				sum_pos[i] += norm2(x, y, z);
			}
		}

		for (int i = 0; i < I; i++)
		{
			// Add square root and convert sums to averages
			sum_pos[i] = sqrt(sum_pos[i] / N);

			sum_ave[i] += sum_pos[i] / L;
			sum_ave2[i] += sum_pos[i] * sum_pos[i] / L;
		}
	}

	ofstream f_out_2("steps_continuos.dat");
	f_out_2 << 0 << " " << "nan" << "\n";
	for (int i = 0; i < I; i++)
	{
		cum_err = sqrt((sum_ave2[i] - sum_ave[i] * sum_ave[i]) / (L - 1));

		f_out_2 << sum_ave[i] << " " << cum_err << "\n";
	}
	f_out_2.close();

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
