//____________________________________________________________________________
/*!

\class   genie::PrimaryVtxGenerator

\brief   Generates a primary interaction vertex assuming a 'liquid drop' model
         for nuclear targets so to give a 'starting point' for cascading MCs
         (simulating intranuclear effects) that are stepping the interaction
         products out of the nucleus.
         Note that the target is considered to be 'centered' at (0,0,0). When
         running the GENIE's event generation modules using the GENIE MC job
         driver, the driver would shift the vertex at a random point along the
         neutrino direction, in a volume of the specified GEANT/ROOT geometry
         containing the selected target material.

         Is a concrete implementation of the PrimaryVtxGeneratorI interface.

\author  Costas Andreopoulos <C.V.Andreopoulos@rl.ac.uk>
         CCLRC, Rutherford Appleton Laboratory

\created July 09, 2005

*/
//____________________________________________________________________________

#include "Conventions/Constants.h"
#include "Conventions/Units.h"
#include "EVGModules/PrimaryVtxGenerator.h"
#include "GHEP/GHepParticle.h"
#include "GHEP/GHepRecord.h"
#include "Interaction/Interaction.h"
#include "Messenger/Messenger.h"
#include "Numerical/RandomGen.h"

using namespace genie;
using namespace genie::constants;
using namespace genie::units;

//___________________________________________________________________________
PrimaryVtxGenerator::PrimaryVtxGenerator() :
EventRecordVisitorI()
{
  fName = "genie::PrimaryVtxGenerator";
}
//___________________________________________________________________________
PrimaryVtxGenerator::PrimaryVtxGenerator(const char * param_set) :
EventRecordVisitorI(param_set)
{
  fName = "genie::PrimaryVtxGenerator";

  this->FindConfig();
}
//___________________________________________________________________________
PrimaryVtxGenerator::~PrimaryVtxGenerator()
{

}
//___________________________________________________________________________
void PrimaryVtxGenerator::ProcessEventRecord(GHepRecord * evrec) const
{
  LOG("VtxGenerator", pDEBUG) << "Generating an interaction vertex";

  Interaction * interaction = evrec->GetInteraction();

  const InitialState & init_state = interaction->GetInitialState();
  const Target &       target     = init_state.GetTarget();

  if (!target.IsNucleus()) {
    LOG("VtxGenerator", pINFO) << "No nuclear target found - Vtx = (0,0,0)";
    return;
  }

  // compute the target radius
  double A  = (double) target.A();
  double Rn = kNucRo * TMath::Power(A, 0.3333); //use the Fermi model

  LOG("VtxGenerator", pINFO) << "A = " << A << ", Rnucl = " << Rn/m << " m";

  // generate a random position inside a spherical nucleus with radius R
  RandomGen * rnd = RandomGen::Instance();

  double R      =     Rn*(rnd->Random2().Rndm());  // [0,Rn]
  double phi    =  2*kPi*(rnd->Random2().Rndm());  // [0,2pi]
  double cos8   = -1 + 2*(rnd->Random2().Rndm());  // [-1,1]
  double sin8   = TMath::Sqrt(1-cos8*cos8);
  double cosphi = TMath::Cos(phi);
  double sinphi = TMath::Sin(phi);
  double x      = Rn * sin8 * cosphi;
  double y      = Rn * sin8 * sinphi;
  double z      = Rn * cos8;

  LOG("VtxGenerator", pINFO)
            << "R = " << R/m << " m, phi = " << phi << ", cos8 = " << cos8;
  LOG("VtxGenerator", pINFO)
        << "x = " << x/m << " m, y = " << y/m << " m, z = " << z/m << " m";

  // loop over the event record entries and set the interaction vertex

  TObjArrayIter piter(evrec);
  GHepParticle * p = 0;

  while ( (p = (GHepParticle *) piter.Next()) ) p->SetVertex(x,y,z,0);
}
//___________________________________________________________________________
