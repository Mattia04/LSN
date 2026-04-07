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

   constexpr unsigned int N_SAMPLES = 1000;

   // uniform
   constexpr unsigned int N[4] = {1, 2, 10, 100};
   double x, sum;
   ofstream unif_out("uniform.dat");
   for (int i = 0; i < N_SAMPLES; i++)
   {
      for (unsigned int n : N)
      {
         sum = 0;
         for (int j = 0; j < n; j++)
         {
            x = rnd.Rannyu();
            sum += x;
         }
         unif_out << sum / n << " ";
      }
      unif_out << endl;
   }

   unif_out.close();

   // exponential
   ofstream exp_out("exponential.dat");
   for (int i = 0; i < N_SAMPLES; i++)
   {
      for (unsigned int n : N)
      {
         sum = 0;
         for (int j = 0; j < n; j++)
         {
            x = rnd.Exponential(1);
            sum += x;
         }
         exp_out << sum / n << " ";
      }
      exp_out << endl;
   }

   exp_out.close();

   // Cauchy-Lorentz
   ofstream lorentz_out("lorentz.dat");
   for (int i = 0; i < N_SAMPLES; i++)
   {
      for (unsigned int n : N)
      {
         sum = 0;
         for (int j = 0; j < n; j++)
         {
            x = rnd.Lorentz(0, 1);
            sum += x;
         }
         lorentz_out << sum / n << " ";
      }
      lorentz_out << endl;
   }

   lorentz_out.close();

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
