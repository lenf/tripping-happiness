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

// Geant4-DNA MODELS

#include "G4DNAElastic.hh"
#include "G4DNAChampionElasticModel.hh"
#include "G4DNAScreenedRutherfordElasticModel.hh"

#include "G4DNAExcitation.hh"
#include "G4DNAMillerGreenExcitationModel.hh"
#include "G4DNABornExcitationModel.hh"

#include "G4DNAIonisation.hh"
#include "G4DNABornIonisationModel.hh"
#include "G4DNARuddIonisationModel.hh"

#include "G4DNAChargeDecrease.hh"
#include "G4DNADingfelderChargeDecreaseModel.hh"

#include "G4DNAChargeIncrease.hh"
#include "G4DNADingfelderChargeIncreaseModel.hh"

#include "G4DNAAttachment.hh"
#include "G4DNAMeltonAttachmentModel.hh"

#include "G4DNAVibExcitation.hh"
#include "G4DNASancheExcitationModel.hh"


#include "G4LossTableManager.hh"
#include "G4UrbanMscModel.hh"
#include "G4EmConfigurator.hh"
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
			
			/*G4LivermoreIonisationModel* eIoniModel = new G4LivermoreIonisationModel();
			eIoniModel->SetHighEnergyLimit(1*GeV); 
			theIonisation->AddEmModel(0, eIoniModel, new G4UniversalFluctuation() );
			*/
			
			/* G4DNAIonisation* dnaioni = new G4DNAIonisation("e-_G4DNAIonisation");
			 dnaioni->SetEmModel(new G4DummyModel(),1); 
			 pmanager->AddDiscreteProcess(dnaioni);*/
			 
			 // ------------------------------DNA-----------------------------------------
			 
			 G4EmConfigurator* em_config = G4LossTableManager::Instance()->EmConfigurator();

			  G4VEmModel* mod;

			  // *** e-

			  // ---> STANDARD EM processes are inactivated below 1 MeV
			  
			  mod =  new G4UrbanMscModel();
			  mod->SetActivationLowEnergyLimit(1*MeV);
			  em_config->SetExtraEmModel("e-","msc",mod,"thinFilm");
			  
			  mod = new G4MollerBhabhaModel();
			  mod->SetActivationLowEnergyLimit(1*MeV);
			  em_config->SetExtraEmModel("e-","eIoni",mod,"thinFilm",0.0,100*TeV, new G4UniversalFluctuation());

			  // ---> DNA processes activated

			  mod = new G4DNAChampionElasticModel();
			  em_config->SetExtraEmModel("e-","e-_G4DNAElastic",mod,"thinFilm",0.0,1*MeV);
			  
			  mod = new G4DNABornIonisationModel();
			  em_config->SetExtraEmModel("e-","e-_G4DNAIonisation",mod,"thinFilm",11*eV,1*MeV);
			  
			  mod = new G4DNABornExcitationModel();
			  em_config->SetExtraEmModel("e-","e-_G4DNAExcitation",mod,"thinFilm",9*eV,1*MeV);
			  
			  mod = new G4DNAMeltonAttachmentModel();
			  em_config->SetExtraEmModel("e-","e-_G4DNAAttachment",mod,"thinFilm",4*eV,13*eV);
			  
			  mod = new G4DNASancheExcitationModel();
			  em_config->SetExtraEmModel("e-","e-_G4DNAVibExcitation",mod,"thinFilm",2*eV,100*eV);
			 			 
			 // ------------------------------DNA-----------------------------------------
			 
			
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
			
			//theIonisation->SetEmModel(new G4MollerBhabhaModel());
			
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
  G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(10*eV, 10*GeV);
  //DumpCutValuesTable();

}  

