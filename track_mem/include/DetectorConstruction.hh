#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "MagneticField.hh"
#include "G4Region.hh"

class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4UserLimits;
class G4Region;


class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
  
     DetectorConstruction();
    ~DetectorConstruction();

  public:
  
     G4VPhysicalVolume* Construct();  
     
     const G4VPhysicalVolume* GetTracker() {return physiTracker;};
     G4double GetTrackerFullLength() {return fTrackerLength;};
     G4double GetTargetFullLength()  {return fTargetLength;};
     G4double GetWorldFullLength()   {return fWorldLength;}; 
     
     void setTargetMaterial (G4String);
     void SetMagField(G4double);
     void SetMaxStep (G4double);     
     
  private:

     G4Box*             solidWorld;    
     G4LogicalVolume*   logicWorld;
     G4VPhysicalVolume* physiWorld;  
          
     G4Box*             solidTarget;  
     G4LogicalVolume*   logicTarget;  
     G4VPhysicalVolume* physiTarget;   
               
     G4Box*             solidTracker;  
     G4LogicalVolume*   logicTracker;
     G4VPhysicalVolume* physiTracker; 
     
     G4Material*         TargetMater;  

     G4UserLimits* stepLimit;           

     MagneticField* fpMagField;   
     G4Region * filmRegion;
     G4double fWorldLength;            
     G4double fTargetLength;          
     G4double fTrackerLength;        
};

#endif
