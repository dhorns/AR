#ifndef __TA2GenericApp_CATSShield_h__
#define __TA2GenericApp_CATSShield_h__

#include "MCBranchID.h"
#include "TA2Detector.h"
#include <iostream>

class TA2GenericApp_CATSShield : public TA2Detector 
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
  TA2GenericApp_CATSShield( const char*, TA2System* ); //Normal use
  virtual ~TA2GenericApp_CATSShield();
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
      
  ClassDef(TA2GenericApp_CATSShield,1)
};

//---------------------------------------------------------------------------

inline Double_t TA2GenericApp_CATSShield::GetSigmaEnergyGeV(Double_t pEnergy)
{
  // Returns energy resolution in GeV when supplied Energy in GeV
  return (fSigmaEnergyFactor * TMath::Power(pEnergy, fSigmaEnergyPower));
}

//---------------------------------------------------------------------------

inline Double_t TA2GenericApp_CATSShield::GetSigmaEnergy(Double_t pEnergy)
{
  // Returns energy resolution in MeV when supplied Energy in MeV
  Double_t sigma, energy;
  energy = pEnergy / 1000.0;
  sigma = GetSigmaEnergyGeV(energy) * 1000.0;
  return sigma;
}

//---------------------------------------------------------------------------

inline  Double_t TA2GenericApp_CATSShield::GetSigmaThetaDg()
{
  // Gives theta resolution in degrees
  return fSigmaTheta;
}

//---------------------------------------------------------------------------

inline  Double_t TA2GenericApp_CATSShield::GetSigmaTheta()
{
  // Gives theta resolution in degrees
  return (fSigmaTheta * TMath::DegToRad());
}

//---------------------------------------------------------------------------

inline Double_t TA2GenericApp_CATSShield::GetSigmaPhiDg(Double_t pTheta)
{
  // Returns Phi resolution in degrees when given theta of
  // cluster in degrees
  return (fSigmaPhi / TMath::Sin(pTheta * TMath::DegToRad()));
}

//---------------------------------------------------------------------------

inline Double_t TA2GenericApp_CATSShield::GetSigmaPhi(Double_t pTheta)
{
  // Returns Phi resolution in degrees when given theta of
  // cluster in radian
  return (fSigmaPhi * TMath::DegToRad() / TMath::Sin(pTheta));
}

//---------------------------------------------------------------------------

inline Double_t TA2GenericApp_CATSShield::GetSigmaTime()
{
  // Returns time resolution in ns
  return fSigmaTime;
}

inline void TA2GenericApp_CATSShield::Decode()
{
    DecodeBasic();
    for(UInt_t n=0; n<fNelement; n++)
    if((fTime[n]==EBufferEnd) || (fTime[n]==-1.0))
      fTime[n] = (Double_t)ENullHit;
}

inline void TA2GenericApp_CATSShield::ReadDecoded()
{
  // Read from MC or reduced file.
  // connect branches

	Int_t nHits = *(Int_t*)(fEvent[EI_ncatsshield]);
	Int_t* hit = (Int_t*)(fEvent[EI_icatsshield]);
	Float_t* energy = (Float_t*)(fEvent[EI_ecatsshield]);
	Float_t* time = nullptr;
	if (fIsTime) time = (Float_t*)(fEvent[EI_tcatsshield]);

	fNhits = 0;  // init valid hits

	for ( UInt_t i = 0; i < fNelement; i++)
	{
		fEnergy[i] = 0.0;
		if ( fIsTime) fTime[i] = (Double_t)ENullHit;
	}

	Double_t total = 0;  // total energy
	for ( Int_t i = 0; i < nHits; i++)
	{

		Int_t elem = hit[i];

		if ( elem < 0 || elem >= (Int_t)fNelement) continue;
		if ( fElement[elem]->IsIgnored()) continue;

		Double_t e = energy[i]*1000.0;
		Double_t t = fIsTime ? time[i] : 0.0;

		total += e;

		fEnergy[elem] = e; 
		if ( fIsTime) fTime[elem] = t;

		fHits[fNhits] = elem;
		fEnergyOR[fNhits] = e;
		if ( fIsTime) fTimeOR[fNhits] = t;
		fNhits++;

	}

	fTotalEnergy = total;

	fHits[fNhits] = -1;
	fEnergyOR[fNhits] = EBufferEnd;
	if ( fIsTime) fTimeOR[fNhits] = EBufferEnd;

}

#endif
