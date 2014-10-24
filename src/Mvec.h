/* 
 * File:   Mvec.h
 * Author: lechszym
 *
 * Created on 16 October 2014, 9:09 PM
 */

#ifndef MVEC_H
#define	MVEC_H

#include "Blade.h"
#include <list>
#include <sstream>
#include <math.h>
#include <iostream>

#ifdef EIGEN_ENABLED
#include <Eigen/Dense>
#endif

namespace gca {

typedef std::list<Blade> blades_t;

#ifndef GCA_PRECISION
#define GCA_PRECISION  1e-12
#endif

   class Mvec {
   public:

      Mvec() {
      }

      Mvec(const Blade &b) {
         _blades.push_back(b);
      }

      Mvec(double v) {
         _blades.push_back(Blade(v));
      }

      Mvec(double v, unsigned long e) {
         _blades.push_back(Blade(v,e));
      }

      Mvec(const blades_t& blades) {
         _blades = blades;
      }

#ifdef EIGEN_ENABLED

      Mvec(Eigen::Matrix<double, Eigen::Dynamic, 1> &v) {
         for (unsigned int i = 0; i < v.size(); i++) {
            _blades.push_back(Blade(v(i, 0), i + 1));
         }
         this->prune();
      }

      Mvec(Eigen::Matrix<double, 1, Eigen::Dynamic> &v) {
         for (unsigned int i = 0; i < v.size(); i++) {
            _blades.push_back(Blade(v(0, i), i + 1));
         }
         this->prune();
      }
#endif

      Mvec(const Mvec& orig) {
         _blades = orig._blades;
      }

      virtual ~Mvec() {

      }

      Mvec& dot(const Mvec &m) const {
         Mvec *result = new Mvec();

         blades_t::const_iterator i;
         blades_t::const_iterator j;

         for (i = _blades.begin(); i != this->_blades.end(); i++) {
            for (j = m._blades.begin(); j != m._blades.end(); j++) {
               result->_blades.push_back((*i)&(*j));
            }
         }
         result->prune();
         return *result;
      }

      Mvec& wedge(const Mvec &m) const {
         Mvec *result = new Mvec();

         blades_t::const_iterator i;
         blades_t::const_iterator j;

         for (i = _blades.begin(); i != this->_blades.end(); i++) {
            for (j = m._blades.begin(); j != m._blades.end(); j++) {
               result->_blades.push_back((*i)^(*j));
            }
         }
         result->prune();
         return *result;
      }

      Mvec& mul(const Mvec &m) const {
         Mvec *result = new Mvec();

         blades_t::const_iterator i;
         blades_t::const_iterator j;

         for (i = _blades.begin(); i != this->_blades.end(); i++) {
            for (j = m._blades.begin(); j != m._blades.end(); j++) {
               //std::cout << *i << " & " << *j << " = ";
               result->_blades.push_back((*i)&(*j));
               //std::cout << result->_blades.back() << std::endl;
               //std::cout << *i << " ^ " << *j << " = ";
               result->_blades.push_back((*i)^(*j));
               //std::cout << result->_blades.back() << std::endl;
            }
         }
         result->prune();
         return *result;
      }

      Mvec& mul(const double x) const {
         Mvec *result = new Mvec();

         blades_t::const_iterator i;

         for (i = _blades.begin(); i != this->_blades.end(); i++) {
            result->_blades.push_back((*i) * x);
         }
         result->prune();
         return *result;
      }

      Mvec& div(const double x) const {
         Mvec *result = new Mvec();

         blades_t::const_iterator i;

         for (i = _blades.begin(); i != this->_blades.end(); i++) {
            result->_blades.push_back((*i) / x);
         }
         result->prune();
         return *result;
      }

      Mvec& div(const Mvec& m) const {
         double mMag = m.mag();
         Mvec mInv = m.conj();

         Mvec *result = new Mvec(this->mul(mInv.div(mMag)));

         result->prune();
         return *result;
      }

      Mvec& add(const Mvec& m) const {
         Mvec *result = new Mvec(_blades);
         result->_blades.insert(result->_blades.end(), m._blades.begin(), m._blades.end());

         result->prune();
         return *result;
      }

      Mvec& add(double x) const {
         Mvec *result = new Mvec(_blades);


         blades_t::iterator bi = result->_blades.begin();
         while (bi != result->_blades.end()) {
            bi->set(bi->get() + x);
            bi++;
         }
         result->prune();
         return *result;
      }

      Mvec& sub(const Mvec& m) const {
         Mvec *result = new Mvec(m.mul(-1));

         result->_blades.insert(result->_blades.end(), _blades.begin(), _blades.end());

         result->prune();
         return *result;
      }

      Mvec& sub(double x) const {
         Mvec *result = new Mvec(_blades);

         blades_t::iterator bi = result->_blades.begin();
         while (bi != result->_blades.end()) {
            bi->set(bi->get() - x);
            bi++;
         }
         result->prune();
         return *result;
      }

      double mag(void) const {
         double m = 0;
         blades_t::const_iterator i;
         for (i = this->_blades.begin(); i != this->_blades.end(); i++) {
            m += i->mag();
         }
         return m;
      }

      Mvec& conj() const {
         Mvec *result = new Mvec();

         blades_t::const_iterator i;

         for (i = _blades.begin(); i != this->_blades.end(); i++) {
            result->_blades.push_back(i->conj());
         }
         result->prune();
         return *result;

      }

      std::string toString() {
         std::stringstream ss;
         bool beg = true;

         if (_blades.empty()) {
            ss << "0";
         } else {
            _blades.sort();
            blades_t::iterator i;
            for (i = _blades.begin(); i != _blades.end(); i++) {
               if (!beg) {
                  ss << " ";
                  if (i->get() >= 0) {
                     ss << "+";
                  }
               }
               beg = false;
               ss << *i;
            }
         }
         return ss.str();
      }

      Mvec operator&(const Mvec& m) const {
         return this->dot(m);
      }

      Mvec operator^ (const Mvec& m) const {
         return this->wedge(m);
      }

      Mvec operator*(const Mvec& m) const {
         return this->mul(m);
      }

      Mvec operator*(const double x) const {
         return this->mul(x);
      }

      Mvec operator/(const Mvec& m) const {
         return this->div(m);
      }

      Mvec operator/(double x) const {
         return this->div(x);
      }

      Mvec operator+(const Mvec& m) const {
         return this->add(m);
      }

      Mvec operator+(const double x) const {
         return this->add(x);
      }

      Mvec operator-(const Mvec& m) const {
         return this->add(m);
      }

      Mvec operator-(double x) const {
         return this->add(x);
      }

      Mvec& operator=(const Mvec& m) {
         if (this != &m) {
            _blades = m._blades;
         }
         return *this;

      }

      Mvec& operator~(void) {
        return this->conj();
      }
      
      Mvec operator[] (const unsigned int nIndex) const {
        Mvec result;

        blades_t::const_iterator i;
        for(i=_blades.begin();i!=_blades.end();i++) {
            if(i->grade()== nIndex) {
                result._blades.push_back(*i);
            }
        }
        return result;
      }
     

      friend std::ostream& operator<<(std::ostream &out, Mvec &v) {
         out << v.toString();
         return out;
      }

   private:

      void prune() {
         blades_t::iterator i;
         blades_t::iterator j;
         blades_t::iterator k;

         i = this->_blades.begin();

         while (i != this->_blades.end()) {
            double v = i->get();
            if (v < GCA_PRECISION && v > (-GCA_PRECISION)) {
               k = i;
               k++;
               this->_blades.erase(i);
               i = k;
               continue;
            }

            j = i;
            j++;
            while (j != this->_blades.end()) {
               if ((*i) == (*j)) {
                  i->set(i->get() + j->get());
                  k = j;
                  k++;
                  this->_blades.erase(j);
                  j = k;
               } else {
                  j++;
               }
            }
            v = i->get();
            if (v < GCA_PRECISION && v > (-GCA_PRECISION)) {
               k = i;
               k++;
               this->_blades.erase(i);
               i = k;
            } else {
               i++;
            }
         }
      }

   protected:
      blades_t _blades;

   };
}

#endif	/* MVEC_H */

