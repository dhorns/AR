//--Author	D Hornidge   October 2025
//
// PDG codes of particles generlly observed MAMI-C
// kElectron 11,     kPositron -11
// kMuonMinus 13     kMuonPlus -13      kGamma 22
// kPi0 111          kPiPlus 211        kPiMinus -211       kEta 221
// kProton 2212      kNeutron 2112
// 

#ifndef __TA2CATSPhysics_h__
#define __TA2CATSPhysics_h__

#include "TAcquRoot.h"
#include "TAcquFile.h"
#include "TA2Physics.h"
#include "TA2Analysis.h"
#include "TA2Tagger.h"
#include "TA2Ladder.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBrowser.h"
#include "TH2.h"
#include "TRandom.h"
#include <iostream>
#include <fstream>

// CATS
#include "TA2GenericApp.h"
#include "TA2GenericApp_CATSCore.h"
#include "TA2GenericApp_CATSAnnulus.h"
#include "TA2GenericApp_CATSShield.h"

#define CATS_MAX_HITS	100

class TA2Tagger;

class TA2CATSPhysics : public TA2Physics {

	protected:

// Begin by initialising Detectors

// Tagger
	TA2Tagger*		fTAGG;	// Glasgow photon tagger
	TA2Ladder*		fLADD;	// Ladder

	UInt_t fTaggerPhotonNhits;                              // number of photons in the tagger
	Int_t* fTaggerPhotonHits;                               // pointer to the Tagger photon hits
	Double_t* fTaggerPhotonEnergy;                          // pointer to the Tagger photon energy array
	Double_t* fTaggerPhotonTime;                            // pointer to the Tagger photon time array
	Double_t* fTaggerTimeCut;                            // pointer to the Tagger photon time array

// CATS
	TA2GenericApp* fCATS;							// CATS
	TA2GenericApp_CATSCore* fCATSCore;			// CATS Core
	TA2GenericApp_CATSAnnulus* fCATSAnnulus;	// CATS Annulus
	TA2GenericApp_CATSShield* fCATSShield;		// CATS Shield

// CATS Core
	Int_t			nCATSCoreADCs;
	Int_t			nCATSCoreTDCs;
	Int_t			nCATSCoreHits;

	Double_t	fCATSCoreADCAverage;
	Double_t	fCATSCoreEnergy;

	Double_t	fCATSTime;

// CATS Annulus
	Int_t		nCATSAnnulusADCs;
	Double_t	fCATSAnnulusEnergy;

// CATS Shield
	Int_t			nCATSShieldADCs;

// CATS Total
	Double_t	fCATSEnergy;
	Double_t	fCATSEnergyNoShield;

// Tree Files
	TFile* 		fFile;
	TTree* 		fTree;
	Int_t			fProduceTreeFile;
	Char_t   	fTreeFileName[256];

// Pi0Compton Class Variables
	UInt_t 		i, j;

//	Int_t *rhits;

// Cuts from the CATSPhysics.dat file
	Int_t		fPhotTimePL;
	Int_t		fPhotTimePR;
	Int_t		fPhotTimeRL1;
	Int_t		fPhotTimeRR1;
	Int_t		fPhotTimeRL2;
	Int_t		fPhotTimeRR2;
	Int_t 	fMissingEnergyCut1;
	Int_t		fMissingEnergyCut2;

	Double_t	fPromptRandomRatio;

//	For Smearing the MC Data
	Double_t fESmear;

// CATS Calibration Stuff from Jakob

	Double_t fECore[3];

	Double_t	fCorePed[7];
	Double_t	fCoreGD[7];
	Double_t fCoreGM_A0[7];
	Double_t fCoreGM_A1[7];
	Double_t fCoreGM_A2[7];

	Double_t fAnnPed[6];
	Double_t fAnnM[6];
	Double_t fAnnB[6];

	Double_t fTaggTDCOffset[328];

	// Particle Information

	TA2Particle* 	fTAGGParticles;	   	// Particles from Tagger

	UInt_t	fTAGGNParticle; 	// # Particle from Tagger
	UInt_t	fNTagg;
	UInt_t	fNPrompt;
	UInt_t	fNRandom;

	// Particle Arrays
	Double_t* fCATSEnergyNoShieldPrompt;
	Double_t* fCATSEnergyNoShieldRandom;
	Double_t* fEmissP;
	Double_t* fEmissR;
	Double_t* fEmissCutP;
	Double_t* fEmissCutR;
	Double_t* fCATSTaggerTime;

	Int_t* fTaggerPhotonHitsNSP;
	Int_t* fTaggerPhotonHitsNSR;

	public:

	TA2CATSPhysics( const char*, TA2Analysis* );

	Double_t MCEnergySmear( Double_t unsmearedE);

	// Jakob's functions	
	Double_t SubtractPedestal( Int_t channel, Int_t adc_raw);
	Double_t ApplyGainMatch( Int_t channel, Double_t adc_corr);
	Double_t ApplyEnergyCalibration( Double_t adc_gain_avg);
	Double_t AnnulusCalibration( Int_t channel, Int_t adc_raw);
	Double_t TaggerTDCCalibration( Int_t channel, Int_t time_raw);
	Double_t ComptonScattering( Double_t taggedEnergy);

	virtual ~TA2CATSPhysics();
	virtual void LoadVariable();					// variables for display/cuts
	virtual void LoadCalibration();				// ADC, TDC calibration information, pedestals, etc.
	virtual void PostInit( );						// init after parameter input
	virtual void SetConfig(Char_t*, Int_t);
	virtual void Reconstruct();					// reconstruct detector info
	virtual TA2DataManager* CreateChild( const char*, Int_t ){ return NULL;}
	virtual void CloseTrees();

	ClassDef(TA2CATSPhysics,1)

};

// ----------------------------------------------------------------------------

inline void TA2CATSPhysics::CloseTrees() {

	if ( fProduceTreeFile == 1)
	{
   	fFile->cd();
		fTree->Write();
		fFile->Close();
	}
}	

#endif
