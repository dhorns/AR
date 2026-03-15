#ifndef __TA2GenericApp_CATSCore_h__
#define __TA2GenericApp_CATSCore_h__

#include "MCBranchID.h"
#include "TA2Detector.h"
#include <iostream>

class TA2GenericApp_CATSCore : public TA2Detector 
{

private:
protected:
  Double_t* fEnergyAll;
  UInt_t    fUseSigmaEnergy;        // Use energy resoution smearing for MC?
  UInt_t    fUseSigmaTime;          // Use time resolution smearing in MC?
  Double_t  fSigmaEnergyFactor;     // Factor in Energy Resolution Equation
  Double_t  fSigmaEnergyPower;      // Power in energy Resolution Equation
  Double_t  fSigmaTime;             // Sigma for time resolution
  Double_t  fOffsetTime;            // MC time offset
  Double_t  fSigmaTheta;            // Theta resolution for CB
  Double_t  fSigmaPhi;              // Phi Resolution for CB

public:     
  TA2GenericApp_CATSCore( const char*, TA2System* ); //Normal use
  virtual ~TA2GenericApp_CATSCore();
  virtual void LoadVariable();           //display/cut setup
  virtual void SaveDecoded();              //Save local analysis
  virtual void ReadDecoded();              //Read back previous analysis
  virtual void Decode();
  virtual void SetConfig(Char_t*, Int_t);

  Double_t GetSigmaEnergyGeV(Double_t);
  Double_t GetSigmaEnergy(Double_t);
  Double_t GetSigmaPhi(Double_t);
  Double_t GetSigmaPhiDg(Double_t);
  Double_t GetSigmaTheta();       // Return Theta res. for given Theta (radian)
  Double_t GetSigmaThetaDg();     // Return Theta res. for given Theta (degree)
  Double_t GetSigmaTime();        // Return sigma of time resolution
  Double_t* GetEnergyAll()               { return fEnergyAll; }
  Double_t  GetEnergyAll(Int_t t)        { return fEnergyAll[t]; }
      
  ClassDef(TA2GenericApp_CATSCore,1)
};

//---------------------------------------------------------------------------

inline Double_t TA2GenericApp_CATSCore::GetSigmaEnergyGeV(Double_t pEnergy)
{
  // Returns energy resolution in GeV when supplied Energy in GeV
  return (fSigmaEnergyFactor * TMath::Power(pEnergy, fSigmaEnergyPower));
}

//---------------------------------------------------------------------------

inline Double_t TA2GenericApp_CATSCore::GetSigmaEnergy(Double_t pEnergy)
{
  // Returns energy resolution in MeV when supplied Energy in MeV
  Double_t sigma, energy;
  energy = pEnergy / 1000.0;
  sigma = GetSigmaEnergyGeV(energy) * 1000.0;
  return sigma;
}

//---------------------------------------------------------------------------

inline  Double_t TA2GenericApp_CATSCore::GetSigmaThetaDg()
{
  // Gives theta resolution in degrees
  return fSigmaTheta;
}

//---------------------------------------------------------------------------

inline  Double_t TA2GenericApp_CATSCore::GetSigmaTheta()
{
  // Gives theta resolution in degrees
  return (fSigmaTheta * TMath::DegToRad());
}

//---------------------------------------------------------------------------

inline Double_t TA2GenericApp_CATSCore::GetSigmaPhiDg(Double_t pTheta)
{
  // Returns Phi resolution in degrees when given theta of
  // cluster in degrees
  return (fSigmaPhi / TMath::Sin(pTheta * TMath::DegToRad()));
}

//---------------------------------------------------------------------------

inline Double_t TA2GenericApp_CATSCore::GetSigmaPhi(Double_t pTheta)
{
  // Returns Phi resolution in degrees when given theta of
  // cluster in radian
  return (fSigmaPhi * TMath::DegToRad() / TMath::Sin(pTheta));
}

//---------------------------------------------------------------------------

inline Double_t TA2GenericApp_CATSCore::GetSigmaTime()
{
  // Returns time resolution in ns
  return fSigmaTime;
}

//---------------------------------------------------------------------------
inline void TA2GenericApp_CATSCore::Decode()
{
//    printf("\n\n\n  ****** VOR DECODE-BASIC ******\n\n");
    DecodeBasic();
//    printf("\n\n\n  ****** NACH DECODE-BASIC ******\n\n");
    for(UInt_t n=0; n<fNelement; n++)
    if((fTime[n]==EBufferEnd) || (fTime[n]==-1.0))
      fTime[n] = (Double_t)ENullHit;
}

inline void TA2GenericApp_CATSCore::ReadDecoded()
{
  // Read from MC or reduced file.

  // connect branches
  Int_t nHits = *(Int_t*)(fEvent[EI_ncatscore]);
  Int_t* hit = (Int_t*)(fEvent[EI_icatscore]);
  Float_t* energy = (Float_t*)(fEvent[EI_ecatscore]);
  Float_t* time = NULL;
  if (fIsTime) time = (Float_t*)(fEvent[EI_tcatscore]);

//  std::cout << "Core nHits = " << nHits << std::endl;
  for ( UInt_t ii = 0; ii < nHits; ii++)
  {
//	  std::cout << ii;
//	  std::cout << "  " << hit[ii];
//	  std::cout << "  " << energy[ii]*1000;
//	  std::cout << "  " << time[ii];
//	  std::cout << std::endl;

	  fEnergy[ii] = energy[ii]*1000;
	  fTime[ii] = time[ii];
  }

/*

  fNhits = 0;  // init valid hits
  Double_t total = 0;  // total energy
  for (UInt_t t=0; t<fNelem; t++)
    fEnergyAll[t] = 0.0;

  // loop over hits
  for (Int_t i = 0; i < nHits; i++)
  {
    // slightly less ugly decoding of icryst
    Int_t elem = hit[i] % 10000;

    // check spurious index
    if (elem == -1) continue;

    // check for ignored element
    if (fElement[elem]->IsIgnored()) continue;

    // convert energy and smear
    Double_t e = energy[i] * fEnergyScale * fElement[elem]->GetA1();
    if (fUseSigmaEnergy) e += fRandom->Gaus(0.0, GetSigmaEnergyGeV(e));
    e *= 1000;
    fEnergyAll[i] = e;

    // check energy thresholds
    if ((e < fElement[elem]->GetEnergyLowThr()) ||
        (e > fElement[elem]->GetEnergyHighThr())) continue;

    // convert time and smear
    Double_t t = 0;
    if (fIsTime)
    {
      t = time[i] - fElement[elem]->GetT0() - fOffsetTime;
      if (fSigmaTime > 0 && fUseSigmaTime) t += fRandom->Gaus(0.0, fSigmaTime);

      // check time range
      if ((t < fElement[elem]->GetTimeLowThr()) ||
          (t > fElement[elem]->GetTimeHighThr())) continue;
    }

    // register valid hit
    fEnergy[elem] = e;
    fEnergyOR[fNhits] = e;
    total += e;
    if (fIsTime)
    {
      fTime[elem] = t;
      fTimeOR[fNhits] = t;
    }
    fHits[fNhits] = elem;
    fNhits++;
  }

  fTotalEnergy = total;

  // terminate arrays
  fHits[fNhits] = EBufferEnd;
  fEnergyOR[fNhits] = EBufferEnd;
  if (fIsTime) fTimeOR[fNhits] = EBufferEnd;

  if(fIsRawHits)
  {
    fRawEnergyHits[0] = EBufferEnd;
    fRawTimeHits[0] = EBufferEnd;
  }
*/
}

#endif
