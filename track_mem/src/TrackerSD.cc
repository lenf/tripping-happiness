#include <iostream>
using namespace std;
#include <stdlib.h>
#include "TrackerSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "TFile.h"
#include "TSystem.h"
#include "TNtuple.h"

TrackerSD::TrackerSD(G4String name, G4double Adensity, G4double Amolar, G4double Awidth)
:G4VSensitiveDetector(name)
{
  f = new TFile("energy_dep.root","RECREATE");
  ntuple1 = new TNtuple("Ni","Demo ntuple","z:r:edep");
  ntuple2 = new TNtuple("Electron","Demo ntuple","z:r:amount");
  ntuple3 = new TNtuple("ElectronEnd","Demo ntuple","z:r:amount");
  ntuple4 = new TNtuple("Info","Demo ntuple","amount:den:molar:width");
  entireZ = 0;
  entireX = 0;
  entireY = 0;
  entireR = 0;
  first = 0;
  second = 0;
  endZ = 0;
  endR = 0;
  number = 0;
  trackId = 0;
  entireEdep = 0;
  written = false;
  launchAmount = 0;
  wasElectron = false;
  wasfirst = false;
  density = Adensity;
  molar = Amolar;
  width = Awidth;
  //electronAmount=1;
  //goodElectron=1;
  //stepElectron=0;
  
}

TrackerSD::~TrackerSD()
{ 
	ntuple1 -> Write();
	ntuple2 -> Write();
	ntuple3 -> Write();
	ntuple4 -> Fill (launchAmount, density, molar, width/um);
	ntuple4 -> Write();
    f -> Close(); 
    cout << first << " " << second << "   "<< (second * 1.0)/(first + second) << endl;
	delete f;	
}

void TrackerSD::Initialize(G4HCofThisEvent*)
{
}

G4bool TrackerSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{
  G4double edep = aStep->GetTotalEnergyDeposit();
  
  if(edep==0.) return false;
  
  G4int currentTrackId = (aStep->GetTrack())->GetTrackID();
  G4ThreeVector position =  (aStep->GetTrack()) -> GetPosition();
  
  if ((aStep->GetTrack())->GetDefinition()->GetPDGMass() > 56 )
	   nickel=true;
  else 
	   nickel=false;
	   
 	   
  if (nickel) {
		if (currentTrackId == trackId) {
			wasElectron = false;
			G4double z = position.z() - start_pos.z();	
			G4double r = sqrt ((position.x() - start_pos.x()) * (position.x() - start_pos.x()) + (position.y() - start_pos.y()) * (position.y() - start_pos.y()));
			array[number][0] = z / nm;
			array[number][1] = r / nm;
			number ++;
			ntuple1 -> Fill (z / um , r / nm , edep / eV);
		}
		else {
			launchAmount ++;
			/*for (int i=0; i< GLOBAL; i++){
				array[i][0] = 0;
				array[i][1] = 0;
			}*/
			trackId = currentTrackId;
			number = 1; 
			array[0][0] = 0;
			array[0][1] = 0;
			start_pos.setX(position.x());
			start_pos.setY(position.y());
			start_pos.setZ(position.z());
		}
  }
  else {
	  
	    
	    //cout << trackId << "  " << currentTrackId << endl;
		if (currentTrackId != trackId){
			//cout << " +++++++++++++++++++++++++++++++++++ " << endl;
			/*electronAmount++;
			if((electronAmount%100000)==0){
				cout << 1.*electronAmount/goodElectron << endl;
			}*/
			//cout << trackId << endl;
			//cout << number << endl;
			// writing information about previous electron ( ElectronEnd)
			if ((wasElectron) && (written)){
				ntuple3 -> Fill (endZ / um, endR);
				wasElectron = false;
				if (wasfirst)
					first ++;
				else
					second++;
				//cout << diedElectron/eV << endl;
				
			}
			/*else
				wasElectron = true;	*/  
			if (aStep->GetTrack()->GetDefinition()->GetParticleName()== "e-"){
				//ntuple -> Fill (z / 1000.0, abs (r/nm - array[closest_num][1]) , edep / eV);
				if ((aStep->GetTrack())->GetParentID() == 1)
					wasfirst = true;
				else
					wasfirst = false;
				entireZ = position.z() - start_pos.z();
				entireY = position.y() - start_pos.y();
				entireX = position.x() - start_pos.x();
				entireR = getCorrectRadius(position);
				trackId = currentTrackId;
				written = false;
				wasElectron = true;
			} 
		}
		else{
			//diedElectron = (aStep->GetTrack())->GetKineticEnergy();
			if ((aStep->GetTrack()->GetDefinition()->GetParticleName()== "e-") && (!written)){
				G4double z = position.z() - start_pos.z()- entireZ;
				G4double y = position.y() - start_pos.y()- entireY;
				G4double x = position.x() - start_pos.x()- entireX;
				//cout << " ----------------------------" << sqrt (z*z+y*y+x*x)/nm << "  " << lengthThreshold << "  "  << GLOBAL <<  endl;
				if (sqrt (z*z+y*y+x*x) > lengthThreshold){
					ntuple2 -> Fill (entireZ / um, entireR);
					written = true;
					//goodElectron++;
					//cout << " accepted " << endl;
					
				}
			}
			if ((aStep->GetTrack()->GetDefinition()->GetParticleName()== "e-") && (written)){
				endZ = position.z() - start_pos.z();
				endR = getCorrectRadius(position);
				//diedElectron = (aStep->GetTrack())->GetKineticEnergy();
			}
		}
		//stepElectron++;
	    //diedElectron = (aStep->GetTrack())->GetKineticEnergy();
	    //cout << diedElectron/eV << endl;
  }
  return true;
}

float TrackerSD::getCorrectRadius (G4ThreeVector position){
	G4double z = position.z() - start_pos.z();
	G4double r = sqrt ((position.x() - start_pos.x()) * (position.x() - start_pos.x()) + (position.y() - start_pos.y()) * (position.y() - start_pos.y()));
	int min = 0;
	int max = number-1;
	int num = max/2;
	float eps = 10;
	float z1 = array [num][0];
	float closest = z1;
	int closest_num = num;
	z = z/nm;
	while (abs(z1 - z) > eps){
		if (z1 > z){
			max = num;
			num = int ((max - min) / 2) + min;
		}
		else{
			min = num;
			num += int ((max - min) / 2);
		}
		z1 = array[num][0];
		if (abs(z1 - z) < abs(closest - z)){
			closest = z1;
			closest_num = num;
		}
		if (max - min <= 1)
			break;
	}
	return abs (r/nm - array[closest_num][1]);
}

void TrackerSD::EndOfEvent(G4HCofThisEvent*)
{
}

