#ifndef TrackerSD_h
#define TrackerSD_h 1 

#include "G4VSensitiveDetector.hh"

class G4Step;
class G4HCofThisEvent;
class TFile;
class TH1F;
class TNtuple;
const int GLOBAL = 200000;
const float lengthThreshold = 5*nm;

class TrackerSD : public G4VSensitiveDetector
{
  public:
      TrackerSD(G4String, G4double Adensity, G4double Amolar, G4double Awidth);
     ~TrackerSD();

      void Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*, G4TouchableHistory*);
      void EndOfEvent(G4HCofThisEvent*);
      float getCorrectRadius (G4ThreeVector position);

  private:
	  
      float array[GLOBAL][2];
      int num;
      int number;
      TFile *f;
      TNtuple *ntuple1;
      TNtuple *ntuple2;
      TNtuple *ntuple3;
      TNtuple *ntuple4;
      bool nickel;
	  G4int trackId;
	  G4ThreeVector start_pos; 
	  G4double entireZ;
	  G4double entireY;  
	  G4double entireX;
	  G4double entireR;
	  G4double entireEdep;
	  G4double endZ;
	  G4double endR; 
	  G4double lengthThreshold; 
	  bool written;
	  G4int launchAmount;
	  bool wasElectron;
	  G4double density;
	  G4double molar;
	  G4double width;

};

#endif

