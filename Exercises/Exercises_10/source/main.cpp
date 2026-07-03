#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "mpi.h"
#include "genetic.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    int size, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    Random rnd;
    int seed[4];
    int p1, p2;
    ifstream Primes("Primes");
    if (Primes.is_open())
    {
        for (int i = 0; i <= rank; ++i)
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

    // cout << "I am rank:" << rank << " and I loaded p1=" << p1 << " p2=" << p2 << endl;

    // ================================================================================
    constexpr unsigned int GENERATIONS = 3000; // number of generations to evolve the population
    constexpr double delta = 5. / GENERATIONS; // to increase p of 5 points
    constexpr unsigned int MIGRATIONS = 100;   // number of generations between migrations
    constexpr unsigned int POPULATION = 1000;  // number of individuals in the population

    // Default values
    double prob_swap = 0.0007;
    double prob_shift = 0.03;
    double prob_permute = 0.07;
    double prob_invert = 0.05;
    double prob_crossover = 0.70;

    if (rank == 0)
    {
        if (argc < 6)
        {
            cout << "Usage: mpirun -np <num_processes> ./main <prob_swap> <prob_shift> <prob_permute> <prob_invert> <prob_crossover>" << endl;
            cout << "Using default values:" << endl;
            cout << "prob_swap = " << prob_swap << endl;
            cout << "prob_shift = " << prob_shift << endl;
            cout << "prob_permute = " << prob_permute << endl;
            cout << "prob_invert = " << prob_invert << endl;
            cout << "prob_crossover = " << prob_crossover << endl;
        }
        else
        {
            prob_swap = stod(argv[1]);
            prob_shift = stod(argv[2]);
            prob_permute = stod(argv[3]);
            prob_invert = stod(argv[4]);
            prob_crossover = stod(argv[5]);

            cout << "Using provided values:" << endl;
            cout << "prob_swap = " << prob_swap << endl;
            cout << "prob_shift = " << prob_shift << endl;
            cout << "prob_permute = " << prob_permute << endl;
            cout << "prob_invert = " << prob_invert << endl;
            cout << "prob_crossover = " << prob_crossover << endl;
        }
    }

    string s_best = "data/best_string_rank" + to_string(rank) + ".dat";
    ofstream f_best(s_best);

    string s_route = "data/route_rank" + to_string(rank) + ".dat";
    ofstream f_route(s_route);
    f_route << "# best_L1 average_L1" << endl;

    auto cities = Cities::Initialize_From_File("cap_prov_ita.dat");

    auto population = Genetic(POPULATION, cities, prob_swap, prob_shift, prob_permute, prob_invert, prob_crossover, rnd);

    for (unsigned int i = 0; i < GENERATIONS; ++i)
    {
        population.new_generation(cities, 2 + delta * i, rnd);

        const unsigned int idx_best = population.best_L1_index();
        Chromosome best_chrm = population.get_chromosome(idx_best);

        population.save_chromosome(idx_best, f_best);

        f_route << population.best_L1() << " "
                << population.average_L1() << endl;

        if ((i + 1) % MIGRATIONS == 0)
        {
            // rank 0 chooses a cycle of continents
            vector<unsigned int> order(size);
            if (rank == 0)
            {
                iota(order.begin(), order.end(), 0);

                for (int i = 0; i < size - 1; ++i)
                {
                    const unsigned int j = rnd.Rannyu(i, size);
                    swap(order[i], order[j]);
                }
            }

            MPI_Bcast(order.data(), order.size(), MPI_UNSIGNED, 0, MPI_COMM_WORLD); // blocking broadcast

            std::vector<unsigned int> string_send = best_chrm.get_string();
            std::vector<unsigned int> string_receive = best_chrm.get_string(); // dummy values

            // Find the position of rank in the vector
            auto it = find(order.begin(), order.end(), rank);

            unsigned int idx_rank = it - order.begin();

            unsigned int idx_send = (idx_rank + 1) % size;
            unsigned int idx_receive = (idx_rank - 1 + size) % size;

            MPI_Request send_req, rec_req;
            MPI_Status stat;

            MPI_Isend(string_send.data(), string_send.size(), MPI_UNSIGNED, order[idx_send], 0, MPI_COMM_WORLD, &send_req);
            MPI_Irecv(string_receive.data(), string_receive.size(), MPI_UNSIGNED, order[idx_receive], 0, MPI_COMM_WORLD, &rec_req);

            MPI_Wait(&send_req, &stat);
            MPI_Wait(&rec_req, &stat);

            population.overwrite_first_chromosome(string_receive);
            population.calculate_fitness_first(cities); // to correct the fitness and order after overwriting the first chromosome
        }

        // cout << population.best_L1() << " "
        //      << population.average_L1() << endl;
    }

    MPI_Finalize();

    return 0;
}
