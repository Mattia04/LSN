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
#include <functional>
#include <iostream>
#include <fstream>
#include <string>
#include "random.h"

using namespace std;

struct Option
{
	Option() = default;
	Option(double call, double put) : call(call), put(put) {};

	double call, put;
};

void calculate_price(double &price, const double &delta_t, const double &interest, const double &volatility, Random &rnd)
{
	price *= exp((interest - 1 / 2. * volatility * volatility) * delta_t + volatility * rnd.Gauss(0, 1) * sqrt(delta_t));
}

void calculate_data_blocking(const unsigned int &N, const unsigned int &L, const function<Option()> &func, ofstream &out_c, ofstream &out_p)
{
	Option S_T;
	double sum_c, ave_c;				  // sum and average for the blocks
	double sum_p, ave_p;				  // sum and average for the blocks
	double sum_ave_c = 0, sum_ave2_c = 0; // Sums of all averages and averages (sum over blocks)
	double sum_ave_p = 0, sum_ave2_p = 0; // Sums of all averages and averages (sum over blocks)
	double cum_err_c = 0;				  // Current cumulative error
	double cum_err_p = 0;				  // Current cumulative error
	// Note: sum_ave and sum_ave2 are the SUMS of averages and averages^2,
	//		 NOT the average of the averages and averages^2
	// 		 so there is an extra factor n and n^2 in the denominators to convert to average
	for (int n = 1; n <= N; n++)
	{
		sum_c = 0;
		sum_p = 0;
		for (int l = 0; l < L; l++)
		{
			S_T = func();
			sum_c += S_T.call;
			sum_p += S_T.put;
		}

		ave_c = sum_c / L;
		ave_p = sum_p / L;
		sum_ave_c += ave_c;
		sum_ave_p += ave_p;
		sum_ave2_c += ave_c * ave_c;
		sum_ave2_p += ave_p * ave_p;

		cum_err_c = sqrt((sum_ave2_c / n - sum_ave_c * sum_ave_c / (n * n)) / (n - 1));
		cum_err_p = sqrt((sum_ave2_p / n - sum_ave_p * sum_ave_p / (n * n)) / (n - 1));

		out_c << sum_ave_c / n << " " << cum_err_c << "\n";
		out_p << sum_ave_p / n << " " << cum_err_p << "\n";
	}

	cout << "Call price: " << sum_ave_c / N << " +/- " << cum_err_c << endl;
	cout << "Put price: " << sum_ave_p / N << " +/- " << cum_err_p << endl;
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

	constexpr double interestRF = 0.1; // Risk Free Interest
	constexpr double volatility = 0.25;
	constexpr double initial_price = 100.;
	constexpr double strike_price = 100.;
	constexpr double start_time = 0;
	constexpr double delivery_time = 1.;

	constexpr unsigned int n_simulations = 10000;
	constexpr unsigned int n_blocks = 100;

	constexpr unsigned int throws_per_block = n_simulations / n_blocks;

	constexpr unsigned int n_intervals = 100.;

	double uneven_intervals[n_intervals + 1];

	// Get uneven time intervals
	uneven_intervals[0] = start_time;
	for (int i = 1; i <= n_intervals; i++)
	{
		uneven_intervals[i] = rnd.Rannyu(start_time, delivery_time);
	}
	uneven_intervals[n_intervals] = delivery_time;
	sort(uneven_intervals, uneven_intervals + n_intervals);

	// Direct sample
	function<Option()> func_1 = [&initial_price, &delivery_time, &start_time, &interestRF, &volatility, &rnd]()
	{
		double current_price = initial_price;
		calculate_price(current_price,
						delivery_time - start_time,
						interestRF,
						volatility,
						rnd);
		double inflation = exp(-interestRF * (delivery_time - start_time));
		double price_difference = current_price - strike_price;
		return Option(inflation * max(0., price_difference), inflation * max(0., -price_difference));
	};
	ofstream f_out_1c("averages1_call.dat");
	ofstream f_out_1p("averages1_put.dat");
	calculate_data_blocking(n_blocks, throws_per_block, func_1, f_out_1c, f_out_1p);
	f_out_1c.close();
	f_out_1p.close();

	// discrete samples
	function<Option()> func_2 = [&initial_price, &delivery_time, &start_time, &interestRF, &volatility, &n_intervals, &uneven_intervals, &rnd]()
	{
		double current_price = initial_price;
		for (int i = 0; i < n_intervals; i++)
			calculate_price(current_price,
							uneven_intervals[i + 1] - uneven_intervals[i],
							interestRF,
							volatility,
							rnd);
		double inflation = exp(-interestRF * (delivery_time - start_time));
		double price_difference = current_price - strike_price;
		return Option(inflation * max(0., price_difference), inflation * max(0., -price_difference));
	};
	ofstream f_out_2c("averages2_call.dat");
	ofstream f_out_2p("averages2_put.dat");
	calculate_data_blocking(n_blocks, throws_per_block, func_2, f_out_2c, f_out_2p);
	f_out_2c.close();
	f_out_2p.close();

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
