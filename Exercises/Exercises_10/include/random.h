/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/

/**
 * @file random.h
 * @author D.E. Galli
 *
 * @brief This header file defines the Random class, which contains functions for generating random numbers.
 */

#ifndef __Random__
#define __Random__

/**
 * @class Random
 * @brief A class for generating random numbers.
 *
 * The Random class provides methods for generting random numbers using uniform and Gaussian distributions.
 * It also includes methods for setting the seed and saving it to a file.
 *
 * @author D.E. Galli
 */
class Random
{

private:
  int m1, m2, m3, m4, l1, l2, l3, l4, n1, n2, n3, n4;

protected:
public:
  // Default constructor
  Random();
  // Destructor
  ~Random();
  /// Method to set the seed for the RNG
  void SetRandom(int *, int, int);
  /// Method to save the seed to a file
  void SaveSeed();
  /// Method to generate a random number in the range [0,1)
  double Rannyu(void);
  /// Method to generate a random number in the range [min,max)
  double Rannyu(double min, double max);
  /// Method to generate a random number with a Gaussian distribution
  double Gauss(double mean, double sigma);
};

#endif // __Random__

/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/
