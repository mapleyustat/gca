#include "test_genb.h"

#include <cstdlib>
#include <algorithm>

using namespace std;
using namespace gca;

Blade generate_blade(unsigned int maxGrade) {
   
   vector<unsigned long> e;
   
   unsigned long grade = rand() % (maxGrade+1);

   unsigned long i=0;
   while(i < grade) {
      unsigned long base = rand() % maxGrade;
      base++;
      vector<unsigned long>::iterator ei = std::find(e.begin(),e.end(),base);
      if(ei == e.end()) {
         e.push_back(base);
         i++;
      }
   }
   std::sort(e.begin(),e.end());

   double v = (double) (rand() % 200) - 100;      
   
   return Blade(v, e);
}

Mvec generate_mvec(unsigned int maxBlades, unsigned int maxGrade) {
   
   blades_t blades;
   
   unsigned long nblades = (rand() % (maxBlades))+1;

   unsigned long i=0;
   while(i < nblades) {
      Blade b = generate_blade(maxGrade);
      blades_t::iterator bi = std::find(blades.begin(),blades.end(),b);
      if(bi == blades.end()) {
         blades.push_back(b);
         i++;
      }
   }
   return Mvec(blades);
}