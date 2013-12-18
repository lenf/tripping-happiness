#ifndef MagneticField_H
#define MagneticField_H

#include "G4UniformMagField.hh"

class G4FieldManager;

class MagneticField: public G4UniformMagField
{
  public:
  
   MagneticField(G4ThreeVector fieldVector);  //  Make field equals to fieldVector
   MagneticField();               //  A zero field
  ~MagneticField();  
      
   void SetMagFieldValue(G4ThreeVector fieldVector);
      
   G4ThreeVector GetConstantFieldValue();

  protected:

      // Find the global Field Manager
      G4FieldManager* GetGlobalFieldManager();   // static 
};

#endif
