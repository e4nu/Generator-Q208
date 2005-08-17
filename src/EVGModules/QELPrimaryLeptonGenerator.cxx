//____________________________________________________________________________
/*!

\class   genie::QELPrimaryLeptonGenerator

\brief   Generates the final state primary lepton in v QEL interactions.

         Is a concrete implementation of the EventRecordVisitorI interface.

\author  Costas Andreopoulos <C.V.Andreopoulos@rl.ac.uk>
         CCLRC, Rutherford Appleton Laboratory

\created October 03, 2004

*/
//____________________________________________________________________________

#include "EVGModules/QELPrimaryLeptonGenerator.h"
#include "GHEP/GHepRecord.h"
#include "Interaction/Interaction.h"

using namespace genie;

//___________________________________________________________________________
QELPrimaryLeptonGenerator::QELPrimaryLeptonGenerator() :
PrimaryLeptonGenerator()
{
  fName = "genie::QELPrimaryLeptonGenerator";
}
//___________________________________________________________________________
QELPrimaryLeptonGenerator::QELPrimaryLeptonGenerator(const char * param_set):
PrimaryLeptonGenerator(param_set)
{
  fName = "genie::QELPrimaryLeptonGenerator";

  this->FindConfig();
}
//___________________________________________________________________________
QELPrimaryLeptonGenerator::~QELPrimaryLeptonGenerator()
{

}
//___________________________________________________________________________
void QELPrimaryLeptonGenerator::ProcessEventRecord(GHepRecord * evrec) const
{
// This method generates the final state primary lepton

  //-- Get the interaction & initial state objects
  Interaction * interaction = evrec->GetInteraction();
  const InitialState & init_state = interaction->GetInitialState();

  //-- Figure out the Final State Lepton PDG Code
  int pdgc = interaction->GetFSPrimaryLepton()->PdgCode();

  //-- QEL Kinematics: Compute the lepton energy and the scattering
  //   angle with respect to the incoming neutrino

  //auxiliary params:
  TLorentzVector * p4nu = init_state.GetProbeP4(kRfStruckNucAtRest);

  double Ev   = p4nu->Energy();
  double Q2   = interaction->GetScatteringParams().Q2();
  double M    = init_state.GetTarget().StruckNucleonMass();
  double ml   = interaction->GetFSPrimaryLepton()->Mass();
  double M2   = M*M;
  double ml2  = ml*ml;
  double s    = M2 + 2*M*Ev;
  double W2   = M2; // QEL: W ~= M
  double tmp  = s+ml2-W2;
  double tmp2 = tmp*tmp;

  //Compute outgoing lepton scat. angle with respect to the incoming v

  double cThSc = (tmp - (ml2+Q2)*2*s/(s-M2)) / (TMath::Sqrt(tmp2-4*s*ml2));
  assert( TMath::Abs(cThSc) <= 1 );

  //Compute outgoing lepton energy

  double El = Ev / ( 1 + (Ev/M) * (1-cThSc) );

  //-- Rotate its 4-momentum to the nucleon rest frame
  //   unit' = R(Theta0,Phi0) * R(ThetaSc,PhiSc) * R^-1(Theta0,Phi0) * unit

  TLorentzVector * p4l = P4InNucRestFrame(evrec, cThSc, El);

  //-- Boost it to the lab frame

  TVector3 * beta = NucRestFrame2Lab(evrec);
  p4l->Boost(*beta); // active Lorentz transform

  //-- Create a GHepParticle and add it to the event record
  //   (use the insertion method at the base PrimaryLeptonGenerator visitor)

  this->AddToEventRecord(evrec, pdgc, p4l);

  delete p4nu;
  delete p4l;
}
//___________________________________________________________________________
