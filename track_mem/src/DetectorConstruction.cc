#include <iostream>
using namespace std;

#include "DetectorConstruction.hh"
#include "MagneticField.hh"
#include "TrackerSD.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4SDManager.hh"
#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"
#include "G4PVPlacement.hh"

#include "G4UserLimits.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

#include "G4Region.hh"


DetectorConstruction::DetectorConstruction()
:solidWorld(0),  logicWorld(0),  physiWorld(0),
 solidTarget(0), logicTarget(0), physiTarget(0), 
 solidTracker(0),logicTracker(0),physiTracker(0), 
 TargetMater(0),  stepLimit(0), fpMagField(0),
 fWorldLength(0.),  fTargetLength(0.), fTrackerLength(0.)
{
  fpMagField = new MagneticField();
}
 
DetectorConstruction::~DetectorConstruction()
{
  delete fpMagField;
  delete stepLimit;
  delete filmRegion;      
}
 
G4VPhysicalVolume* DetectorConstruction::Construct()
{
	
//--------- Material definition ---------
  
  G4double a, z;
  G4double density, densityMembran, molar = 0;
  G4int nel, natoms, ncomponents; // number of elements
  
  //Air
  G4Element* N = new G4Element("Nitrogen", "N", z=7., a= 14.01*g/mole);
  G4Element* O = new G4Element("Oxygen"  , "O", z=8., a= 16.00*g/mole);
   
  G4Material* Air = new G4Material("Air", density= 0.*mg/cm3, nel=2);
  Air->AddElement(N, 70*perCent);
  Air->AddElement(O, 30*perCent);

  //PVDF - target material
  //G4NistManager* man = G4NistManager::Instance();
  //G4Material* Pvdf = man->FindOrBuildMaterial("G4_POLYVINYLIDENE_FLUORIDE");
  
  G4Element* H = new G4Element("Hydrogen", "H", z=1., a= 1.01*g/mole);
  G4Element* C = new G4Element("Carbon"  , "C", z=12., a= 12.01*g/mole);
  
  G4Material* PMMA = new G4Material("PMMA", densityMembran= 1.18*g/cm3, ncomponents=3);
  PMMA -> AddElement( H, natoms= 8);// (C5O2H8)
  molar = molar + natoms * H-> GetA() / kg * mole;

  PMMA -> AddElement( C, natoms = 5);
  molar = molar + natoms * C-> GetA() / kg * mole;
 
  PMMA -> AddElement( O, natoms = 2);
  molar = molar + natoms * O-> GetA() / kg * mole;
  
  PMMA -> SetChemicalFormula("C_50_2H_8");
  //Print all the materials defined.
  //G4cout << G4endl << "The materials defined are : " << G4endl << G4endl;
  //G4cout << *(G4Material::GetMaterialTable()) << G4endl;

//--------- Sizes of the principal geometrical components (solids)  ---------
  
  fTargetLength  = 5.0 * cm;                        // Full length of Target
  TargetMater  = PMMA;								// Target material
  fWorldLength= 20 * fTargetLength;					// Full length of world
  G4double targetSize  = 0.5 * fTargetLength;    	// Half length of Target
  G4double fTargetWidth  = 12.5 * um;//1.e-6 * m;        // Half width of Target
      
//--------- Definitions of Solids, Logical Volumes, Physical Volumes ---------
  
  // World
  G4double HalfWorldLength = 0.5*fWorldLength;
/* 
  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(fWorldLength);
  G4cout << "Computed tolerance = "
         << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
         << " mm" << G4endl;
*/
  solidWorld= new G4Box("world",HalfWorldLength,HalfWorldLength,HalfWorldLength);
  logicWorld= new G4LogicalVolume( solidWorld, Air, "World", 0, 0, 0);
  
  //  Must place the World Physical volume unrotated at (0,0,0).
  physiWorld = new G4PVPlacement(0,               // no rotation
                                 G4ThreeVector(), // at (0,0,0)
                                 logicWorld,      // its logical volume
                                 "World",         // its name
                                 0,               // its mother  volume
                                 false,           // no boolean operations
                                 0);              // copy number
                                 
  // Target
  G4ThreeVector positionTarget = G4ThreeVector(0,0,12.5 *um);// (0,0,-(targetSize+trackerSize));
  solidTarget = new G4Box("target",targetSize,targetSize,fTargetWidth); //,targetSize,targetSize,targetSize/10);
  logicTarget = new G4LogicalVolume(solidTarget,TargetMater,"Target",0,0,0);
  physiTarget = new G4PVPlacement(0,               // no rotation
                                  positionTarget,  // at (x,y,z)
                                  logicTarget,     // its logical volume                                  
                                  "Target",        // its name
                                  logicWorld,      // its mother  volume
                                  false,           // no boolean operations
                                  0);              // copy number 


  filmRegion = new G4Region("thinFilm");  
  filmRegion->AddRootLogicalVolume(logicTarget);
/*
  G4cout << "Target is " << fTargetLength/cm << " cm of " 
         << TargetMater->GetName() << G4endl;
*/
 
  // Sensitive detectors
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  G4String trackerSDname = "TrackerSD";
  TrackerSD* aTrackerSD = new TrackerSD(trackerSDname, densityMembran / kg * m3, molar, 2*fTargetWidth);
  SDman->AddNewDetector(aTrackerSD);
  logicTarget->SetSensitiveDetector(aTrackerSD);

//--------- Visualization attributes -------------------------------

  G4VisAttributes* BoxVisAtt= new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
  logicWorld -> SetVisAttributes(BoxVisAtt);  
  logicTarget -> SetVisAttributes(BoxVisAtt);
  
//--------- User Limits -------------------------------

  // Sets a max Step length in the tracker region, with G4StepLimiter
  G4double maxStep = 0.1*nm; // try to set max step to 10^-9 of world's lenght (about 1 nm)
  stepLimit = new G4UserLimits(maxStep); // doesn't work now 
  //stepLimit->SetUserMinEkine(1000*eV);
  logicTarget->SetUserLimits(stepLimit);
  
  return physiWorld;
}

