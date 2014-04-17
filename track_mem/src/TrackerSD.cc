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
#include "G4SystemOfUnits.hh"

TrackerSD::TrackerSD(G4String name, G4double Adensity, G4double Amolar, G4double Awidth)
:G4VSensitiveDetector(name)
{
  f = new TFile("energy_dep.root","RECREATE");
  ntuple1 = new TNtuple("Ni","Demo ntuple","z:r:edep");
  ntuple2 = new TNtuple("Electron","Demo ntuple","z:r:amount");
  ntuple3 = new TNtuple("ElectronEnd","Demo ntuple","z:r:amount");
  ntuple4 = new TNtuple("Info","Demo ntuple","amount:den:molar:width");
  f1 = new TFile("MolerBhabha.root","RECREATE");
  f1->cd();
  ntuple5 = new TNtuple("Second_electron_energy","Demo ntuple","energy");  
  f2 = new TFile("Allelectronliver.root","RECREATE");
  f2->cd();
  ntuple6 = new TNtuple("Allelectron","Demo ntuple","energy");  
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

  
}

TrackerSD::~TrackerSD()
{ 
	ntuple1 -> Write();
	ntuple2 -> Write();
	ntuple3 -> Write();
	ntuple4 -> Fill (launchAmount, density, molar, width/um);
	ntuple4 -> Write();
    f -> Close();
    ntuple5 -> Write();
    f1 ->Close(); 
    ntuple6 -> Write();
    f2 ->Close(); 
    cout << "first_e = " << first << " second_e = " << second << "   "<< (second * 1.0)/(first + second) << endl;
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
	  	if ((aStep->GetTrack()) -> GetDefinition() ->GetParticleName()!= "e-")
			cout << "Detected particle: " << aStep->GetTrack() -> GetDefinition()->GetParticleName()  << endl;
			
					
	  	if (currentTrackId != trackId){
			if ((wasElectron) && (written)){
				ntuple3 -> Fill (endZ / um, endR);
				wasElectron = false;
				if (wasfirst)
					first ++;
				else{
					second++;
					ntuple5 -> Fill(kenergy);
					
				}
			}
			
			if (aStep->GetTrack()->GetDefinition()->GetParticleName()== "e-"){
				kinenergy = aStep->GetTrack()->GetKineticEnergy() / eV;
				ntuple6 -> Fill(kinenergy);
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
			if ((aStep->GetTrack()->GetDefinition()->GetParticleName()== "e-") && (!written)){
				G4double z = position.z() - start_pos.z()- entireZ;
				G4double y = position.y() - start_pos.y()- entireY;
				G4double x = position.x() - start_pos.x()- entireX;
				if (sqrt (z*z+y*y+x*x) > lengthThreshold){
					ntuple2 -> Fill (entireZ / um, entireR);
					written = true;
				}
			}
			if ((aStep->GetTrack()->GetDefinition()->GetParticleName()== "e-") && (written)){
				endZ = position.z() - start_pos.z();
				endR = getCorrectRadius(position);
				kenergy = kinenergy;
			}
		}
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

