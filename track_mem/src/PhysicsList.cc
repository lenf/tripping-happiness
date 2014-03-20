#include <iostream>
using namespace std;

#include "globals.hh"
#include "PhysicsList.hh"

#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4SystemOfUnits.hh"
#include "G4IonConstructor.hh"
#include "G4GenericIon.hh"
#include "G4IonParametrisedLossModel.hh"

#include "G4ionIonisation.hh"

#include <G4eMultipleScattering.hh>
#include <G4eIonisation.hh>
#include <G4eBremsstrahlung.hh>

#include <G4ComptonScattering.hh>
#include <G4GammaConversion.hh>
#include <G4PhotoElectricEffect.hh>

#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4PAIModel.hh"

#include "G4Region.hh"
#include "G4RegionStore.hh"

#include "G4MollerBhabhaModel.hh"


void PhysicsList::ConstructParticle()
{
	// create A LOT OF particles
  	/*G4PhysConstVector::iterator itr;
   	for (itr = physicsVector->begin(); itr != physicsVector->end(); ++itr) {
     		(*itr)->ConstructParticle();
   	}*/

	// Electron & Positron
	G4Electron::ElectronDefinition();
	G4Positron::PositronDefinition();
	// Gamma
	G4Gamma :: GammaDefinition();
	// Ion
   	G4GenericIon::GenericIonDefinition();

}

void PhysicsList::ConstructProcess()
{
  AddTransportation();
  /*G4PhysConstVector::iterator itr;
  for (itr = physicsVector->begin(); itr != physicsVector->end(); ++itr) {
  	(*itr)->ConstructProcess();
  }*/

  ConstructEM();
  //ConstructOptical();
  ConstructGeneral();
  //AddStepMax();
}

void PhysicsList::ConstructEM()
{  	
	//G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
	//think about that helper. what for it should be used or not
	
	const G4RegionStore* theRegionStore = G4RegionStore::GetInstance();
	G4Region* film = theRegionStore->GetRegion("thinFilm");
	  
	theParticleIterator->reset();
  	while( (*theParticleIterator)() )
	{
    	G4ParticleDefinition* particle = theParticleIterator->value();
		G4ProcessManager* pmanager = particle->GetProcessManager();
    	G4String particleName = particle->GetParticleName();
    	
    	/*
    	G4ProcessVector* proclist = pmanager->GetProcessList();
		for (int itr = 0; itr < proclist->length(); ++itr) {
			cout << (*proclist)[itr]->GetProcessName() << endl;
		}*/
		//to print using processes of concret particle use statetment above
		
	
		cout << particleName << endl;
		
		if (particleName == "GenericIon") {        
      			//Main idea to make this code perfect
      			
      			G4ionIonisation* theionIonisation = new G4ionIonisation();
      			theionIonisation->SetEmModel(new G4IonParametrisedLossModel());
      			
      			/*G4PAIModel* pai = new G4PAIModel(particle, "G4PAI");//thin film model for em
      			theionIonisation->SetEmModel(pai);//(0, pai, pai, film);*/
      			
      			theionIonisation->SetStepFunction(0.1, 1*um); // TO DO :: think about step
					pmanager->AddProcess(theionIonisation,         -1, 1, 1); //ионизация
					//ph->RegisterProcess(theionIonisation, particle);
      
    		} else if ((particleName == "e-") || (particleName == "e+")) {
				
			G4eMultipleScattering * theMultipleScattering = new G4eMultipleScattering();
			pmanager->AddProcess(theMultipleScattering, -1, 1, 1); //многократное рассеяние
			
			G4eIonisation * theIonisation = new G4eIonisation();
			theIonisation->SetEmModel(new G4MollerBhabhaModel());
			pmanager->AddProcess(theIonisation,  -1, 2, 2); //ионизация
			
			
			G4eBremsstrahlung * theBremsstrahlung = new G4eBremsstrahlung();
			pmanager->AddProcess(theBremsstrahlung,  -1, 3, 3); //тормозное излучение
			
// * The general interface to a physics process allows 3 stage of interactions: *
// * AtRest, AlongTheStep, PostStep.						*
// * Each of 3 integer numbers shows the order of the process. 			*
// * Negative number means that the process is inactive at the give stage	*

      				
      				
      				
      				
      				
      	
      				
      				
      				
    		} else if (particleName == "gamma") {
			G4PhotoElectricEffect * thePhotoElectricEffect = new G4PhotoElectricEffect();      
  			G4ComptonScattering * theComptonScattering = new G4ComptonScattering();
  			G4GammaConversion* theGammaConversion = new G4GammaConversion();
				pmanager->AddDiscreteProcess(thePhotoElectricEffect); //Фотоэффект
				pmanager->AddDiscreteProcess(theComptonScattering); //Эффект Комптона
				pmanager->AddDiscreteProcess(theGammaConversion); //
    		}
	}
}

void PhysicsList::ConstructGeneral()
{
	// Add Decay Process
	G4Decay* theDecayProcess = new G4Decay();
	theParticleIterator->reset();
	while ((*theParticleIterator)())
	{
       		G4ParticleDefinition* particle = theParticleIterator->value();
       		G4ProcessManager* pmanager = particle->GetProcessManager();
       		if (theDecayProcess->IsApplicable(*particle)) 
		{
         		pmanager ->AddProcess(theDecayProcess);
         		// set ordering for PostStepDoIt and AtRestDoIt
         		pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
         		pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
       		}
   	}
}	

PhysicsList::PhysicsList() : G4VModularPhysicsList()
{
  SetVerboseLevel(1);
  defaultCutValue = 1*nm;
  // Default physics
  //RegisterPhysics(new G4DecayPhysics());

  // Radioactive decay
  //RegisterPhysics(new G4RadioactiveDecayPhysics());

  // EM physics
  //RegisterPhysics(new G4EmStandardPhysics());
}

PhysicsList::~PhysicsList()
{ 
}


void PhysicsList::SetCuts()
{
  //G4cout << GetDefaultCutValue()/nm << G4endl;
  //G4VUserPhysicsList::SetCuts();
  G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(10*eV, 10*GeV);
  
  // set cut values for gamma at first and for e- second and next for e+,
  // because some processes for e+/e- need cut values for gamma
  SetCutValue(1*nm, "gamma");
  SetCutValue(1*nm, "e-");
  SetCutValue(1*nm, "e+");
  G4VUserPhysicsList::SetCuts();
  //DumpCutValuesTable();
  /*SetCutValue(1*m, "gamma");//cutForGamma
  SetCutValue(1*m, "e-");//cutForElectron
  SetCutValue(1*m, "e+");//cutForElectron
  SetCutValue(1*m, "GenericIon");//cutForGenericIon
*/}  

