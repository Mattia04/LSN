/**
 * @file main.cpp
 * @brief Main program for the genetic algorithm solving the TSP.
 *
 * The program initializes the random number generator, the cities, and the population of chromosomes.
 * It then runs the genetic algorithm for a specified number of generations, applying selection, crossover, and mutations to evolve the population.
 * The best route and the average route length are saved to files for analysis.
 *
 * The program runs both configuration of cities: on a circle and inside a square.
 */

#include "genetic.hpp"
#include <fstream>
#include <string>

using namespace std;

// initializes the random number generator with seeds from files
void init_random(Random &rnd);

int main()
{
    Random rnd; // random number generator
    init_random(rnd);

    constexpr unsigned int GENERATIONS = 1000; // number of generations to evolve the population
    constexpr unsigned int POPULATION = 100;   // population size

    constexpr double start_p = 2.;               // initial exponent for selection pressure
    constexpr double delta_p = 5. / GENERATIONS; // to increase the exponent of selection during evolution, (increasing pressure with time)

    // main evolution function that runs the genetic algorithm for a given set of cities and saves the results to files
    auto run = [&](Cities &cities, const char *pos_file, const char *route_file, const char *best_route_file)
    {
        cities.save_positions(pos_file);

        // initialize the population of chromosomes with random routes
        // also state the probabilities for mutations (single swap, shift, permute, inversion) and crossover
        auto population = Genetic(POPULATION, cities, 0.005, 0.08, 0.08, 0.08, 0.6, rnd);

        ofstream route(route_file);           // save best L1 and average L1 for each generation
        ofstream best_route(best_route_file); // save the best route of each generation (as a sequence of UIDs)
        route << "# Best_L1 Average_L1" << endl;

        for (unsigned int i = 0; i < GENERATIONS; ++i)
        {
            // left out for performance reasons, used for debugging
            // if (!population.check_all_chromosomes(cities.get_possible_UIDs()))
            //     cout << "At lest one chromosome is invalid at generation: " << i << endl;

            // evolve the population to create a new generation of chromosomes
            // applying selection, crossover, and mutations
            population.new_generation(cities, start_p + delta_p * i, rnd);

            // DEBUG
            // ofstream debug("data/debug" + to_string(i) + ".dat");
            // for (unsigned int j = 0; j < POPULATION; ++j)
            //     population.save_chromosome(j, debug);
            // debug.close();

            unsigned int idx_best = population.best_L1_index();

            route << population.best_L1() << " "
                  << population.average_L1() << endl;

            population.save_chromosome(idx_best, best_route);
        }
    };

    // run both configuration of cities
    auto cities_circle = Cities::Initialize_Cities_Circle(34, 1., rnd);
    run(cities_circle, "data/map_circle.dat", "data/route_L1_circle.dat", "data/best_route_circle.dat");

    auto cities_square = Cities::Initialize_Cities_Square(34, 1., rnd);
    run(cities_square, "data/map_square.dat", "data/route_L1_square.dat", "data/best_route_square.dat");

    return 0;
}

void init_random(Random &rnd)
{
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
}
