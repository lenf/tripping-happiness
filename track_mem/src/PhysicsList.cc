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
#include "G4UniversalFluctuation.hh"
#include "G4PenelopeIonisationModel.hh"
#include "G4LivermoreIonisationModel.hh"

void PhysicsList::ConstructParticle()
{
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
  ConstructEM();
}

void PhysicsList::ConstructEM()
{  	
	G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
	
	/*const G4RegionStore* theRegionStore = G4RegionStore::GetInstance();
	G4Region* film = theRegionStore->GetRegion("thinFilm");*/
	  
	theParticleIterator->reset();
  	while( (*theParticleIterator)() )
	{
    	G4ParticleDefinition* particle = theParticleIterator->value();
    	G4ProcessManager* pmanager = particle->GetProcessManager();
    	G4String particleName = particle->GetParticleName();
   
   		cout << particleName << endl;
		
		if (particleName == "GenericIon") { 
			       
      		G4ionIonisation* theionIonisation = new G4ionIonisation();
      		theionIonisation->SetEmModel(new G4IonParametrisedLossModel());
      		
      		/*G4PAIModel* pai = new G4PAIModel(particle, "G4PAI");//thin film model for em
      		theionIonisation->SetEmModel(pai);//(0, pai, pai, film);*/
      		
      		theionIonisation->SetStepFunction(0.1, 1*um); // TO DO :: think about step
			ph->RegisterProcess(theionIonisation, particle);
      
    	}else if (particleName == "e-") {
				
			G4eMultipleScattering * theMultipleScattering = new G4eMultipleScattering();
			ph->RegisterProcess(theMultipleScattering, particle);
			
			G4eIonisation* theIonisation = new G4eIonisation();
		    
		    /*theIonisation->SetStepFunction(0.2, 0.1*nm);
		    G4VEmModel* theIoniPenelope = new G4PenelopeIonisationModel();
		    theIoniPenelope->SetHighEnergyLimit(1*MeV);
            theIonisation->AddEmModel(0, theIoniPenelope, new G4UniversalFluctuation());
			*/
			
			//theIonisation->SetEmModel(new G4MollerBhabhaModel());
			
			G4LivermoreIonisationModel* eIoniModel = new G4LivermoreIonisationModel();
			eIoniModel->SetHighEnergyLimit(1*GeV); 
			theIonisation->AddEmModel(0, eIoniModel, new G4UniversalFluctuation() );
			
			ph->RegisterProcess(theIonisation, particle);
			
			G4eBremsstrahlung * theBremsstrahlung = new G4eBremsstrahlung();
			ph->RegisterProcess(theBremsstrahlung, particle);
		
		}else if (particleName == "e+") {
				
			G4eMultipleScattering * theMultipleScattering = new G4eMultipleScattering();
			ph->RegisterProcess(theMultipleScattering, particle);
			
			G4eIonisation* theIonisation = new G4eIonisation();
		    /*theIonisation->SetStepFunction(0.2, 0.1*nm);
		    G4VEmModel* theIoniPenelope = new G4PenelopeIonisationModel();
		    theIoniPenelope->SetHighEnergyLimit(1*MeV);
            theIonisation->AddEmModel(0, theIoniPenelope, new G4UniversalFluctuation());
			*/
			
			theIonisation->SetEmModel(new G4MollerBhabhaModel());
			
			//theIonisation->SetEmModel(new G4LivermoreIonisationModel());
			
			ph->RegisterProcess(theIonisation, particle);
			
			G4eBremsstrahlung * theBremsstrahlung = new G4eBremsstrahlung();
			ph->RegisterProcess(theBremsstrahlung, particle);
	
    	}else if (particleName == "gamma") {
			G4PhotoElectricEffect * thePhotoElectricEffect = new G4PhotoElectricEffect();      
  			G4ComptonScattering * theComptonScattering = new G4ComptonScattering();
  			G4GammaConversion* theGammaConversion = new G4GammaConversion();
			ph->RegisterProcess(thePhotoElectricEffect, particle);
			ph->RegisterProcess(theComptonScattering, particle);
			ph->RegisterProcess(theGammaConversion, particle);
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
}

PhysicsList::~PhysicsList()
{ 
}


void PhysicsList::SetCuts()
{
  // set cut values for gamma at first and for e- second and next for e+,
  // because some processes for e+/e- need cut values for gamma
  SetCutValue(1*nm, "gamma");//cutForGamma
  SetCutValue(1*nm, "e-");//cutForElectron
  SetCutValue(1*nm, "e+");//cutForElectron
  SetCutValue(1*nm, "GenericIon");//cutForGenericIon
  G4VUserPhysicsList::SetCuts();
  G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(30*eV, 10*GeV);
  //DumpCutValuesTable();

}  

