@mainpage My Genetic Algorithm for Travelling Salesman Problem

# My Genetic Algorithm for Travelling Salesman Problem

This project aims to solve the travelling salesman problem (TSP) using an implementation of a genetic algorithm (GA), the genetic algorithm is implemented with parallelization.

---

## Algorithm Overview

### Initialization

Creates a collection of Cities, one can choose to create it from a file or generate a random configuration.

Then for each rank of the program generate a population with $N$ individuals.

### Evolution Loop

For each generation (and each rank):

1. Orders the individual in the generation from best to worst.
2. Until the generation has a full population:
    1. *Selection*: chooses 2 random parents to generate 2 offsprings
    2. *Crossover*: applies with a certain probability the crossover to the offsprings.
    3. *Mutations*: applies 4 mutations (swap, shift, permute, invert) each with a certain probability.
    4. Saves the offsprings to the new generation, until the new population is full.
3. Saves the best path to a file and the best path length and average path length to another file.

Every $M$ generations a migration happens: the best individual of each rank is sent to another random one.

---

## Building

### Compilation

Inside the root directory:
```bash
make
```

Note: the compilation requires `mpicxx` and `c++20`.

### Executing

```bash
mpirun -n 7 ./main.exe
```

You can modify the number of cores used at your liking.

---

## Minimal Working Example

```cpp
#include "genetic.hpp"
#include "random.h"
Random rnd; // setup the random number generator.
const unsigned int N_CITIES = 34;
const unsigned int N_INDIVIDUALS = 100;
const unsigned int N_GENERATIONS = 500;
// Create a random map on the unit circle.
auto cities_circle = Cities::Initialize_Cities_Circle(N_CITIES, 1., rnd);
// Create a random population of N_INDIVIDUALS
auto population = Genetic(N_INDIVIDUALS, cities, rnd);
// Compute the new generation N_GENERATIONS times
for (unsigned int i = 0; i < N_GENERATIONS; ++i)
population.new_generation(cities, 2., rnd);
```

---

## Author

Mattia Ballico
