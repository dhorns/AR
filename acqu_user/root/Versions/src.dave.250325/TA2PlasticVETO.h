//--Author	JRM Annand   30th Sep 2003
//--Rev 	JRM Annand...15th Oct 2003 ReadDecoded...MC data
//--Rev 	JRM Annand... 5th Feb 2004 3v8 compatible
//--Rev 	JRM Annand...19th Feb 2004 User code
//--Rev 	JRM Annand...16th May 2005 ReadDecoded (bug G3 output)
//--Update	JRM Annand... 2nd Jun 2005 ReadRecoded bug fix (D.Glazier)
//--Description
//                *** Acqu++ <-> Root ***
// Online/Offline Analysis of Sub-Atomic Physics Experimental Data 
//
// TA2PlasticVETO
//
// Internal Particle Identification Detector for the Crystal Ball
// Cylindrical array of plastic scintillators
//

#ifndef __TA2PlasticVETO_h__
#define __TA2PlasticVETO_h__

#include "MCBranchID.h"
#include "TA2Detector.h"

class TA2PlasticVETO : public TA2Detector {
 private:
 public:
  TA2PlasticVETO( const char*, TA2System* );// Normal use
  virtual ~TA2PlasticVETO();
  //  virtual void SetConfig( char*, int );// decode and load setup info
  virtual void LoadVariable( );            // display/cut setup
  //  virtual void PostInit( );            // initialise using setup info
  virtual void Decode( );              // hits -> energy procedure
  virtual void SaveDecoded( );             // save local analysis
  virtual void ReadDecoded( );             // read back previous analysis

  ClassDef(TA2PlasticVETO,1)
};

//---------------------------------------------------------------------------
inline void TA2PlasticVETO::ReadDecoded( )
{
  // Read back...
  //   either previously analysed data from Root Tree file
  //   or MC simulation results, assumed to have the same data structure
  // Bug fix remove fNhits-- line, (DG 27/5/05, implemented JRMA 2/6/05)

  UInt_t i,j,k;
  Double_t total = 0.0;
  Double_t E;
  fNhits = *(Int_t*)(fEvent[EI_nvtaps]);
  Float_t* energy = (Float_t*)(fEvent[EI_evtaps]);
  Int_t* index = (Int_t*)(fEvent[EI_ivtaps]);
  Double_t EthrLow,EthrHigh;                           // energy thresholds

  for( i=0,k=0; i<fNhits; i++ ){
    j = *index++;
    
    if( !j ){                     // is it s real hit
      //      fNhits--;
      i--;
      energy++;
      printf("No real hit\n");
      continue;
    }
    
    // IDs in simulation: 1 to 384, here 0 to 383
    j--;
    
    if (fElement[j]->IsIgnored()) continue;            // check ignored elements
 
    E = (*energy++) * 1000.;

    EthrLow = fElement[j]->GetEnergyLowThr();          // low thresh
    EthrHigh = fElement[j]->GetEnergyHighThr();        // high thresh
    
    if( (E < EthrLow) || (E > EthrHigh) ) {            // energy inside thresh?
      //if(fIsTime) *time++;
      continue;
    }
  
    fEnergy[j] = E;
    fEnergyOR[k] = E;
    if(fIsTime){
      //fTime[j] = (*time++);
      //fTimeOR[k] = fTime[j];
    }     
    fHits[k] = j;
    total += E;
    k++;
  }

  fHits[k] = EBufferEnd;
  fEnergyOR[k] = EBufferEnd;
  fNhits = k;
  if(fIsTime)fTimeOR[k] = EBufferEnd;
  fTotalEnergy = total;
}

//---------------------------------------------------------------------------
inline void TA2PlasticVETO::Decode( )
{
  // Run basic TA2Detector decode, then anything else

  DecodeBasic();
  // Anything else here           
}

#endif
