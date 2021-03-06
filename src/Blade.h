#ifndef BLADE_H
#define	BLADE_H

#include <vector>
#include <string>
#include <sstream>
#include <math.h>
#include <iostream>

namespace gca
{

typedef std::vector<unsigned long> ebase_t;

class Blade {
public:

   Blade() : _e() {
      _v = 0;
   }

   Blade(double v) {
      _v = v;
   }

   Blade(double v, unsigned long e) {
       _e.push_back(e);
       _v = v;
   }

   Blade(double v, const ebase_t &e) {
      _e = e;
      _v = v;
   }

   Blade(const Blade& orig) {
      _v = orig._v;
      _e = orig._e;
   }

   virtual ~Blade() {

   }

   unsigned int grade() const {
      return _e.size();
   }

   void set(double v) {
      _v = v;
   }

   double get(void) const {
      return _v;
   }

   Blade& inner(const Blade &B) const {
      double v = _v * B._v;

      // If one of the numbers is scalar, we can speed up
      // calculation
      if (_e.empty()) {
         return *(new Blade(v, B._e));
      } else if (B._e.empty()) {
         return *(new Blade(v,_e));
      }

      int sign;
      bool common = false;
      ebase_t e = this->emult(_e, B._e, &sign, &common);

      if (common) {
         return *(new Blade(v * sign, e));
      } else {
         return *(new Blade());
      }
   }

   Blade& outer(const Blade &B) const {
      double v = _v * B._v;

      // If one of the numbers is scalar, we can speed up
      // calculation
      if (_e.empty() || B._e.empty()) {
         return *(new Blade());
      }

      int sign;
      bool common = true;
      ebase_t e = this->emult(_e, B._e, &sign, &common);

      if (common) {
         return *(new Blade());
      } else {
         return *(new Blade(v * sign, e));
      }
   }

   Blade& conj(void) const {
      int sign = 1;
      unsigned int k = this->grade();
      if (k > 0) {
         k = k * (k - 1) / 2;

         if (k % 2 == 1) {
            sign = -1;
         }
      }

      return *(new Blade(_v * sign, _e));
   }

   double mag(void) const {
      /* Magnitude of a blade, it's the inner product
         of a blade and its conjugate
         mag = A&(~A) */

      Blade result = this->inner(this->conj());
      return result._v;
   }

   Blade& inv(void) const {
      /* Inverse of a blade, it's the conjugate of 
         a blade divided by its magnitude 
         Ainv = ~A/(A&(~A)) */
      return *(new Blade(this->conj() / this->mag()));
   }

   std::string toString() const {
      std::stringstream ss;

      ss.precision(4);
      double v = ceil(_v * 10000);
      v = v / 10000;


      ss << v << " ";

      ebase_t::const_iterator eA_iter = _e.begin();

      while (eA_iter != _e.end()) {
         ss << "e" << *eA_iter;
         eA_iter++;
         if (eA_iter != _e.end()) {
            ss << "^";
         }
      }

      return ss.str();
   }   
   
   Blade operator&(const Blade& B) const {
      return this->inner(B);
   }

   Blade operator^ (const Blade& B) const {
      return this->outer(B);
   }

   Blade operator/(double x) const {
      return Blade(_v / x, _e);
   }

   Blade operator*(double x) const {
      return Blade(_v * x, _e);
   }

   Blade operator~(void) const {
      return this->conj();
   }

   Blade& operator=(const Blade& B) {
      if (this != &B) {
         _e = B._e;
         _v = B._v;
      }
      return *this;

   }

   bool operator==(const Blade& b) const {

      /* Comparison of two blades - check if blade bases are identical,
         the blade value are ignored */
      if (this->grade() != b.grade()) {
         return false;
      }

      for (size_t i = 0; i < _e.size(); i++) {
         if (_e[i] != b._e[i]) {
            return false;
         }
      }

      return true;
   }

   bool operator<(const Blade& b) const {

      if (this->grade() < b.grade()) {
         return true;
      } else if (this->grade() > b.grade()) {
         return false;
      }

      for (size_t i = 0; i < _e.size(); i++) {
         if (_e[i] < b._e[i]) {
            return true;
         } else if (_e[i] > b._e[i]) {
            return false;
         }
      }

      return false;
   }

   unsigned int at(const unsigned int gIndex) const {
        return _e[gIndex];
   }
   
   unsigned int operator[] (const unsigned int gIndex) const {
        return this->at(gIndex);
   }

   friend std::ostream& operator<<(std::ostream &out, const Blade &b) {
      out << b.toString();
      return out;
   }

private:

   ebase_t& emult(const ebase_t &eA,
           const ebase_t &eB,
           int *sign,
           bool *common) const {

      bool outer = *common;
      *common = false;
      
      ebase_t *eC = new ebase_t();
      eC->reserve(eA.size() + eB.size());

      unsigned int iA = eA.size();

      *sign = 1;

      ebase_t::const_iterator eA_iter = eA.begin();
      ebase_t::const_iterator eB_iter = eB.begin();

      while (true) {
         if (eA_iter == eA.end()) {
            eC->insert(eC->end(), eB_iter, eB.end());
            break;
         } else if (eB_iter == eB.end()) {
            eC->insert(eC->end(), eA_iter, eA.end());
            break;
         } else if (*eA_iter < *eB_iter) {
            eC->push_back(*eA_iter);
            iA--;
            eA_iter++;
         } else if (*eB_iter < *eA_iter) {
            unsigned int flip = iA % 2;
            if (flip != 0) {
               *sign = -(*sign);
            } 
            eC->push_back(*eB_iter);
            eB_iter++;
         } else {
            *common = true;
            if (outer) {
                eC->clear();
                break;
            }
            if(iA) {
                unsigned int flip = --iA % 2;
                if (flip != 0) {
                   *sign = -(*sign);
                }
            }
            eA_iter++;
            eB_iter++;
         }
      }

      // If doing inner product, then we're done - it will be zero
      if(!outer && !(*common)) {
          eC->clear();
      }

      return *eC;      
   }

protected:
   ebase_t _e;
   double _v;

};
}

#endif