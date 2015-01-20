//____________________________________________________________________________
/*!

\class   genie::GEVGPool

\brief   A pool of GEVGDriver objects with an initial state key

\author  Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
         STFC, Rutherford Appleton Laboratory

\created May 24, 2005

\cpright  Copyright (c) 2003-2015, GENIE Neutrino MC Generator Collaboration
          For the full text of the license visit http://copyright.genie-mc.org
          or see $GENIE/LICENSE
*/
//____________________________________________________________________________

#ifndef _GEVG_DRIVER_POOL_H_
#define _GEVG_DRIVER_POOL_H_

#include <map>
#include <string>
#include <ostream>

using std::map;
using std::string;
using std::ostream;

namespace genie {

class GEVGDriver;
class InitialState;

class GEVGPool : public map<string, GEVGDriver *> {

public :

  GEVGPool();
  ~GEVGPool();

  GEVGDriver * FindDriver (const InitialState & init) const;
  GEVGDriver * FindDriver (string init)               const;

  void Print (ostream & stream) const;

  friend ostream & operator << (ostream & stream, const GEVGPool & pool);
};

}      // genie namespace

#endif // _GEVG_DRIVER_POOL_H_
