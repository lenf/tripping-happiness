#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "G4VUserPhysicsList.hh"
#include "G4VModularPhysicsList.hh"

#include "globals.hh"

class PhysicsList: public G4VModularPhysicsList
{
public:
  PhysicsList();
  virtual ~PhysicsList();

  virtual void SetCuts();
protected:
  void ConstructParticle();
  void ConstructProcess();
  void ConstructEM();
  void ConstructGeneral();
};

#endif

 
