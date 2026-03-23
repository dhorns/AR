//--Author	David Hornidge   October 2025   CATS Analysis

#include "TA2CATSPhysics.h"

enum {
	EComptonPromptWindows = 1000,
	EComptonRandomWindows,
	ECATSCorePedestals,
	ECATSCoreGainDrift,
	ECATSCoreECal,
	ECATSESmearing,
	ECATSEShift,
	EMissingEnergyCut,
	EProduceTreeFile,
	ETreeFileName
};

static const Map_t kInputs[] = {
	{"Compton-Prompt-Windows:",		EComptonPromptWindows},
	{"Compton-Random-Windows:",		EComptonRandomWindows},
	{"CATS-Core-Pedestals:",			ECATSCorePedestals},
	{"CATS-Core-GainDrift:",			ECATSCoreGainDrift},
	{"CATS-Core-Energy-Calibration:",ECATSCoreECal},
	{"CATS-E-Smearing:",					ECATSESmearing},
	{"CATS-E-Shift:",						ECATSEShift},
	{"Missing-Energy-Cut:",				EMissingEnergyCut},
	{"Produce-Tree-File:",				EProduceTreeFile},
	{"Tree-File-Name:",					ETreeFileName},
	{NULL,          -1}
};


//-----------------------------------------------------------------------------
TA2CATSPhysics::TA2CATSPhysics( const char* name, TA2Analysis* analysis )
	:TA2Physics( name, analysis ) 
{

// Initialise Detectors

// Tagger
	fTAGG			= NULL;

	fNTagg			= 0;
	fNPrompt			= 0;
	fNRandom			= 0;

	fTaggerPhotonNhits  = 0;
	fTaggerPhotonHits   = NULL;
	fTaggerPhotonTime   = NULL;
	fTaggerPhotonEnergy = NULL;

	fTaggerTimeCut   = NULL;

// Particle Counters

// CATS
	fCATS				= NULL;	// CATS
	fCATSCore		= NULL;	// CATS Core
	fCATSAnnulus	= NULL;	// CATS Annulus
	fCATSShield		= NULL;	// CATS Shield

// CATS
	nCATSCoreADCs		= 0;
	nCATSCoreTDCs		= 0;
	nCATSCoreHits		= 0;
	nCATSAnnulusADCs	= 0;
	nCATSShieldADCs	= 0;

	fCATSTime		= 0;

// Particle Arrays

// Missing Energy Stuff
	fCATSEnergyNoShieldPrompt = NULL;
	fCATSEnergyNoShieldRandom = NULL;
	fEmissP = NULL;
	fEmissR = NULL;
	fEmissCutP = NULL;
	fEmissCutR = NULL;

	fCATSTaggerTime   = NULL;
	fTaggerPhotonHitsNSP = NULL;
	fTaggerPhotonHitsNSR = NULL;

	fRandom = new TRandom();

	AddCmdList(kInputs);
}


//-----------------------------------------------------------------------------
TA2CATSPhysics::~TA2CATSPhysics()
{

// Delete Tree Files
//
//	delete fTree;
    if ( fTree) delete fTree;
//	delete fFile;
    if ( fFile) delete fFile;

}
	
//-----------------------------------------------------------------------------
void TA2CATSPhysics::SetConfig(Char_t* line, Int_t key)
{

	// Any special command-line input for analysis

	switch (key){
		case EComptonPromptWindows:
			//  Compton Prompt Windows
			if( sscanf( line, "%d %d\n", &fPhotTimePL, &fPhotTimePR ) != 2 )
			{
				PrintError( line, "<Error: Compton Prompt Windows not set correctly>");
				return;
			}
		break;
		case EComptonRandomWindows:
			//  Compton Random Windows
			if( sscanf( line, "%d %d %d %d\n", &fPhotTimeRL1, &fPhotTimeRR1, &fPhotTimeRL2, &fPhotTimeRR2 ) != 4 )
			{
				PrintError( line, "<Error: Compton Random Windows not set correctly>");
				return;
			}
		break;
		case ECATSCorePedestals:
			//  CATS Core Pedestals
			if( sscanf( line, "%lf %lf %lf %lf %lf %lf %lf\n", &fCorePed[0], &fCorePed[1], &fCorePed[2], &fCorePed[3], &fCorePed[4], &fCorePed[5], &fCorePed[6] ) != 7 )
			{
				PrintError( line, "<Error: CATS Core Pedestals not set correctly>");
				return;
			}
		break;
		case ECATSCoreGainDrift:
			//  CATS Core GainDrift
			if( sscanf( line, "%lf %lf %lf %lf %lf %lf %lf\n", &fCoreGD[0], &fCoreGD[1], &fCoreGD[2], &fCoreGD[3], &fCoreGD[4], &fCoreGD[5], &fCoreGD[6] ) != 7 )
			{
				PrintError( line, "<Error: CATS Core GainDrift not set correctly>");
				return;
			}
		break;
		case ECATSCoreECal:
			//  CATS Core Energy Calibration Parameters
			if( sscanf( line, "%lf %lf %lf\n", &fECore[0], &fECore[1], &fECore[2]) != 3 )
			{
				PrintError( line, "<Error: CATS Core Energy Calibration Parameters not set correctly>");
				return;
			}
		break;
		case ECATSESmearing:
			//  CATS Simulation Energy Smearing
			if( sscanf( line, "%lf\n", &fESmear) != 1 )
			{
				PrintError( line, "<Error: CATS Energy Smearing Value not set correctly>");
				return;
			}
		break;
		case ECATSEShift:
			//  CATS Simulation Energy Smearing
			if( sscanf( line, "%lf\n", &fEShift) != 1 )
			{
				PrintError( line, "<Error: CATS Energy Smearing Value not set correctly>");
				return;
			}
		break;
		case EMissingEnergyCut:
			//  Missing Energy Cut
			if( sscanf( line, "%d %d\n", &fMissingEnergyCut1, &fMissingEnergyCut2 ) != 2 )
			{
				PrintError( line, "<Error: Missing Energy Cut not set correctly>");
				return;
			}
		break;
		case EProduceTreeFile:
			//  Pi0 Random Windows
			if( sscanf( line, "%d\n", &fProduceTreeFile) != 1 )
			{
				PrintError( line, "<Error: Tree files not turned on/off correctly>");
				return;
			}
			if(fProduceTreeFile == 1) printf("\n\nPhysics tree file enabled\n");
                        else printf("\n\nPhysics tree file disabled\n");
		break;
		case ETreeFileName:
			//  Tree File Name
			if( sscanf( line, "%s\n", fTreeFileName) != 1)
			{
				PrintError( line, "<Error: Tree file name not set correctly>");
				return;
			}
			else printf("Physics class tree file will be saved to: %s\n\n", fTreeFileName);
		break;
		default:
			// default main apparatus SetConfig()
			TA2Physics::SetConfig( line, key );
		break;
	}

}

//Define the CATS MC energy smearing function 
Double_t TA2CATSPhysics::MCEnergySmear( Double_t unsmearedE)
{
	Double_t Esmeared, Res;
	
	Res = unsmearedE*fESmear;
	Esmeared = unsmearedE + fRandom->Gaus( 0.0, Res);

	return( Esmeared);
}

//Define the CATS MC energy smearing function 
Double_t TA2CATSPhysics::MCEnergyShift( Double_t unshiftedE)
{
	Double_t Eshifted;
	
	Eshifted = unshiftedE + fEShift;

	return( Eshifted);
}

//
// Jakob's functions
//

//Define the GainMatch Function  
Double_t TA2CATSPhysics::SubtractPedestal( Int_t channel, Int_t adc_raw)
{
	Double_t x = ((adc_raw - fCorePed[channel])*fCoreGD[channel]);
	if (x < 0) {x = 0;} //ensure a non-negative output

	return x;
}

Double_t TA2CATSPhysics::ApplyGainMatch( Int_t channel, Double_t adc_corr)
{
	Double_t x = (fCoreGM_A0[channel] + fCoreGM_A1[channel]*adc_corr + fCoreGM_A2[channel]*adc_corr*adc_corr);
	if (x < 0) {x = 0;} //ensure a non-negative output
	return x;
}

//Define the energy calibration function 

Double_t TA2CATSPhysics::ApplyEnergyCalibration( Double_t adc_gain_avg)
{
	return (fECore[0] + fECore[1]*adc_gain_avg + fECore[2]*adc_gain_avg*adc_gain_avg);
}

//Define the Annulus Pedestal Function
Double_t TA2CATSPhysics::AnnulusCalibration( Int_t channel, Int_t adc_raw)
{
	Double_t x = (adc_raw-fAnnPed[channel]);
	if (x <= 0) return 0.0; //ensure a non negative input

	Double_t y =  x*fAnnM[channel] + fAnnB[channel];
	if (y < 0 ) {y = 0;} //ensure a non negative input

	return y;
}

//Define the Tagger TDC Calibration function
Double_t TA2CATSPhysics::TaggerTDCCalibration( Int_t channel, Int_t time_raw)
{
	Double_t x = (time_raw - fTaggTDCOffset[channel]);
	std::cout << "time" << time_raw << "channel " << channel << "cal_time" << x << std::endl;
	return x;
}

//Define the Compton Scattering Formula
Double_t TA2CATSPhysics::ComptonScattering( Double_t photon_energy)
{
	const Double_t kD2R = TMath::DegToRad();
	Double_t theta = 40.0*kD2R; // scattering angle
	Double_t mass = fP4target[0].M(); // carbon mass in MeV/c^2
	Double_t Eprime = photon_energy / (1 + (photon_energy / mass) * (1 - std::cos(theta)));
	return Eprime;
}

void TA2CATSPhysics::LoadCalibration()
{

	UInt_t i;
	Double_t x, y, z;
	TString name;

	name = "data/CATS-Core-Gain-Matching.dat";
	std::ifstream inFile( name);
	if ( !inFile.is_open())
	{
		std::cout << "Error opening file ";
		std::cout << name;
		std::cout << std::endl;
		exit( -1);
	}
	while ( !inFile.eof())
	{
		name.ReadLine( inFile);
		if ( name[0] != '#')
		{
			sscanf( name, "%d%lf%lf%lf", &i, &x, &y, &z);
			fCoreGM_A0[i] = x;
			fCoreGM_A1[i] = y;
			fCoreGM_A2[i] = z;
		}
	}
	std::cout << "CATS Core Gain Matching Parameters read in." << std::endl;
	inFile.close();

	name = "data/CATS-Annulus-calibration.dat";
	inFile.open( name);
	if ( !inFile.is_open())
	{
		std::cout << "Error opening file ";
		std::cout << name;
		std::cout << std::endl;
		exit( -1);
	}
	while ( !inFile.eof())
	{
		name.ReadLine( inFile);
		if ( name[0] != '#')
		{
			sscanf( name, "%d%lf%lf%lf", &i, &x, &y, &z);
			fAnnPed[i] = x;
			fAnnM[i] = y;
			fAnnB[i] = z;
		}
	}
	std::cout << "CATS Annulus Calibration Parameters read in." << std::endl;
	inFile.close();

	name = "data/Tagger-TDC-Offsets.dat";
	inFile.open( name);
	if ( !inFile.is_open())
	{
		std::cout << "Error opening file ";
		std::cout << name;
		std::cout << std::endl;
		exit( -1);
	}
	while ( !inFile.eof())
	{
		name.ReadLine( inFile);
		if ( name[0] != '#')
		{
			sscanf( name, "%d%lf", &i, &x);
			fTaggTDCOffset[i] = x;
		}
	}
	std::cout << "Tagger TDC Offsets read in.\n\n";
	inFile.close();

}

//---------------------------------------------------------------------------
void TA2CATSPhysics::PostInit()
{

	// Read in calibration parameters
	LoadCalibration();

// Introduce Detectors

	// Tagger
	fTAGG = (TA2Tagger*)((TA2Analysis*)fParent)->GetChild("TAGG");
	if ( !fTAGG) PrintError("","<No Tagger class found>",EErrFatal);
	else {  printf("Tagger included in analysis\n");
		fTAGGParticles = fTAGG->GetParticles(); }

	// Ladder
	fLADD = (TA2Ladder*)((TA2Analysis*)fParent)->GetGrandChild( "FPD");
	if ( !fLADD) PrintError( "", "<No Ladder class found>", EErrFatal);

	// CATS
	fCATS = (TA2GenericApp*)((TA2Analysis*)fParent)->GetChild( "CATSApp");
	if ( !fCATS) printf( "CATS *NOT* included in analysis\n");
	else
	{
		printf( "CATS Core included in analysis\n");
//		fCATSParticles = fCATS->GetParticles();

		// CATS Core
		fCATSCore = (TA2GenericApp_CATSCore*)((TA2Analysis*)fParent)->GetGrandChild( "CATSCore");
		if ( !fCATSCore) PrintError( "", "<No CATS Core class found>", EErrFatal);

		// CATS Annulus
		fCATSAnnulus = (TA2GenericApp_CATSAnnulus*)((TA2Analysis*)fParent)->GetGrandChild( "CATSAnnulus");
		if ( !fCATSAnnulus) printf( "CATS Annulus not included in analysis\n");
		else printf( "CATS Annulus included in analysis\n");

		// CATS Shield
		fCATSShield = (TA2GenericApp_CATSShield*)((TA2Analysis*)fParent)->GetGrandChild( "CATSShield");
		if ( !fCATSShield) printf( "CATS Shield not included in analysis\n");
		else printf( "CATS Shield included in analysis\n");

	}

	printf("\n");

// Calculate ratio of prompt to random windows

	if (gAR->GetProcessType() == EMCProcess) {
		fPromptRandomRatio	= 0.0;
	}
	else {
		fPromptRandomRatio = double(fPhotTimePR - fPhotTimePL)
			/double(fPhotTimeRR1 - fPhotTimeRL1 + fPhotTimeRR2 - fPhotTimeRL2);
 	}

// Get max # of Particles from detectors, used for defining array sizes

// Create arrays to hold Particles

	// create hit, energy and time arrays
	fTaggerPhotonHits = new Int_t[8*328];
	fTaggerPhotonEnergy = new Double_t[8*328];
	fTaggerPhotonTime = new Double_t[8*328];
	fTaggerTimeCut = new Double_t[8*328];
	fCATSEnergyNoShieldPrompt = new Double_t[8*328];
	fCATSEnergyNoShieldRandom = new Double_t[8*328];
	fEmissP = new Double_t[8*328];
	fEmissR = new Double_t[8*328];
	fEmissCutP = new Double_t[8*328];
	fEmissCutR = new Double_t[8*328];
	fCATSTaggerTime = new Double_t[8*328];
	fTaggerPhotonHitsNSP = new Int_t[8*328];
	fTaggerPhotonHitsNSR = new Int_t[8*328];

// Create Tree Files, Define Branches (if option is turned on "fProduceTreeFile ==1")

	if ( fProduceTreeFile == 1)
	{

		fFile = new TFile( fTreeFileName, "RECREATE", "Physics", 3);
		fTree = new TTree( "CATSPhysicsTree", "CATS Physics Analysis");

		fTree->Branch( "NTagg",	&fNTagg,	"NTagg/I");

		gROOT->cd();
	}

	// Default physics initialisation
	TA2Physics::PostInit();

}

//-----------------------------------------------------------------------------
void TA2CATSPhysics::LoadVariable( )
{

// Input name - variable pointer associations for any subsequent cut/histogram setup

	// Tagger
	TA2DataManager::LoadVariable( "NTagg", 			&fNTagg,				EISingleX);
	TA2DataManager::LoadVariable( "NPrompt", 			&fNPrompt,			EISingleX);
	TA2DataManager::LoadVariable( "NRandom", 			&fNRandom,			EISingleX);

	TA2DataManager::LoadVariable( "TaggerNhits",		&fTaggerPhotonNhits,	EISingleX);
	TA2DataManager::LoadVariable( "TaggerHits",		fTaggerPhotonHits,	EIMultiX);
	TA2DataManager::LoadVariable( "TaggerTime",		fTaggerPhotonTime,	EDMultiX);
	TA2DataManager::LoadVariable( "TaggerEnergy",	fTaggerPhotonEnergy,	EDMultiX);
	TA2DataManager::LoadVariable( "TaggerTimeCut",	fTaggerTimeCut,	EDMultiX);

// CATS 

	TA2DataManager::LoadVariable( "CATSEnergyNoShieldPrompt",		fCATSEnergyNoShieldPrompt,		EDMultiX);
	TA2DataManager::LoadVariable( "CATSEnergyNoShieldRandom",		fCATSEnergyNoShieldRandom,		EDMultiX);

	TA2DataManager::LoadVariable( "TaggerHitsNSP",		fTaggerPhotonHitsNSP,	EISingleX);
	TA2DataManager::LoadVariable( "TaggerHitsNSR",		fTaggerPhotonHitsNSR,	EISingleX);

	TA2DataManager::LoadVariable( "EmissP",		fEmissP,		EDMultiX);
	TA2DataManager::LoadVariable( "EmissR",		fEmissR,		EDMultiX);
	TA2DataManager::LoadVariable( "EmissCutP",	fEmissCutP,	EDMultiX);
	TA2DataManager::LoadVariable( "EmissCutR",	fEmissCutR,	EDMultiX);

	// Core
	TA2DataManager::LoadVariable( "nCATSCoreADCs", 			&nCATSCoreADCs,			EISingleX);
	TA2DataManager::LoadVariable( "nCATSCoreTDCs", 			&nCATSCoreTDCs,			EISingleX);
	TA2DataManager::LoadVariable( "nCATSCoreHits", 			&nCATSCoreHits,			EISingleX);
	TA2DataManager::LoadVariable( "CATSCoreADCAverage",	&fCATSCoreADCAverage,	EDSingleX);
	TA2DataManager::LoadVariable( "CATSCoreEnergy",			&fCATSCoreEnergy,			EDSingleX);

	// A single time for CATS based on the first hit in the core
	TA2DataManager::LoadVariable( "CATSTime", 			&fCATSTime,			EDSingleX);

	// Annulus
	TA2DataManager::LoadVariable( "nCATSAnnulusADCs", 		&nCATSAnnulusADCs,		EISingleX);
	TA2DataManager::LoadVariable( "CATSAnnulusEnergy",		&fCATSAnnulusEnergy,		EDSingleX);

	// Shield
	TA2DataManager::LoadVariable( "nCATSShieldADCs", 	&nCATSShieldADCs,		EISingleX);

	// Total CATS
	TA2DataManager::LoadVariable( "CATSEnergy",				&fCATSEnergy,				EDSingleX);
	TA2DataManager::LoadVariable( "CATSEnergyNoShield",	&fCATSEnergyNoShield,	EDSingleX);

	TA2DataManager::LoadVariable( "CATSTaggerTime",		fCATSTaggerTime,	EDMultiX);

	TA2Physics::LoadVariable();

	return;
}

//-----------------------------------------------------------------------------
void TA2CATSPhysics::Reconstruct() 
{

	Int_t elem;
	Int_t *rhits;
	Int_t adc_raw;
	Double_t adc_corr, adc_gm, sum_gain;
	Double_t adc_averaged, core_energy;
	Bool_t shield;

	Double_t CoreE, CoreE_sm, AnnulusE, AnnulusE_sm;

// Do CATS Shield first

// CATS Shield
	shield = kFALSE;
	if ( fCATSShield)
	{
		if ( fCATSShield->IsRawHits())
		{
			nCATSShieldADCs = fCATSShield->GetNADChits();
			rhits = fCATSShield->GetRawEnergyHits();
			if ( nCATSShieldADCs > 0) shield = kTRUE;
		}
	}

	fCATSCoreEnergy = 0;

	// CATS Core
	if ( fCATSCore)
	{
		if ( fCATSCore->IsRawHits())
		{

			// Real Data
			if ( gAR->GetProcessType() != EMCProcess)
			{

				// ADCs
				nCATSCoreADCs = fCATSCore->GetNADChits();
				rhits = fCATSCore->GetRawEnergyHits();

				sum_gain = 0;
				for ( i = 0; i < nCATSCoreADCs; i++)
				{
					elem = rhits[i];
					adc_raw = (Int_t)fCATSCore->GetElement( elem)->GetRawADCValue();
					adc_corr = SubtractPedestal( elem, adc_raw);
					adc_gm = ApplyGainMatch( elem, adc_corr);

					sum_gain += adc_gm;
				}
				if ( nCATSCoreADCs != 0) adc_averaged = sum_gain/nCATSCoreADCs;
				else adc_averaged = 0;

				core_energy = ApplyEnergyCalibration( adc_averaged);

				fCATSCoreADCAverage = adc_averaged;
				fCATSCoreEnergy = core_energy;
			}

			// MC Data
			else
			{
				CoreE = fCATSCore->GetEnergy( 0);
				CoreE_sm = MCEnergySmear( CoreE);
				fCATSCoreEnergy = MCEnergyShift( CoreE_sm);
				if ( fCATSCoreEnergy > 1000) fCATSCoreEnergy = 0;
			}
		}

		// An attempt to get a single time from CATS, presumed to be the one that gave the trigger.
		// Narrotor: this did not, in fact, work at all.
		// Ignore fCATSTime and the subsequent fCATTaggerTime

		nCATSCoreTDCs = fCATSCore->GetNTDChits();
		nCATSCoreHits = fCATSCore->GetNhits();

		fCATSTime = -1000;
		for ( i = 0; i < nCATSCoreTDCs; i++)
		{
			elem = fCATSCore->GetHits( i);
			if(fCATSCore->IsTime()) {
				fCATSTime = fCATSCore->GetTime( elem);
				break;
			}
		}
	}

	Double_t sum_annulus, annulus_energy;
	sum_annulus = 0;
	annulus_energy = 0;

	fCATSAnnulusEnergy = 0;

// CATS Annulus
	if ( fCATSAnnulus)
	{
		if ( fCATSAnnulus->IsRawHits())
		{
			// Real Data
		 	if ( gAR->GetProcessType() != EMCProcess)
			{
				nCATSAnnulusADCs = fCATSAnnulus->GetNADChits();
				rhits = fCATSAnnulus->GetRawEnergyHits();

				for ( i = 0; i <nCATSAnnulusADCs; i++)
				{
					elem = rhits[i];
					adc_raw = (Int_t)fCATSAnnulus->GetElement( elem)->GetRawADCValue();
					annulus_energy = AnnulusCalibration( elem, adc_raw);
					sum_annulus += annulus_energy;
				}
			}
			// MC Data
			else
			{
				AnnulusE = fCATSAnnulus->GetEnergy( 0);
				AnnulusE_sm = MCEnergySmear( AnnulusE);
				sum_annulus = MCEnergyShift( AnnulusE_sm);
			}
		}
		fCATSAnnulusEnergy = sum_annulus;
	}

	fCATSEnergy = fCATSCoreEnergy + fCATSAnnulusEnergy;

	if ( shield == kFALSE) fCATSEnergyNoShield = fCATSEnergy;

	// 4-mom stuff
	Double_t CATSth, CATSph;
	Double_t CATSpx, CATSpy, CATSpz;
	Double_t CATSmom, Emiss, Emiss2;
	Double_t eBeamPhot;
	TLorentzVector p4CATS, p4BeamPhot, p4incident, p4missing;
	const Double_t kD2R = TMath::DegToRad();

	CATSmom = fCATSEnergy;
	CATSth = 40*kD2R;
	CATSph = 0*kD2R;
	CATSpx = CATSmom*sin( CATSth)*cos( CATSph);
	CATSpy = CATSmom*sin( CATSth)*sin( CATSph);
	CATSpz = CATSmom*cos( CATSth);
	p4CATS.SetPxPyPzE( CATSpx, CATSpy, CATSpz, CATSmom);

//
// TAGGER STUFF
//
	// Number of tagger particles
	fTAGGNParticle	= fTAGG->GetNparticle();
	fNTagg = fTAGGNParticle;
	Double_t eBeamEnergy = fTAGG->GetBeamEnergy();

// get the channel electron energies
	const Double_t* fpdEnergy = fLADD->GetECalibration();

	TA2Particle taggerphoton;
        
// Tagger Multi-Hit stuff
	// Get the tagger hit multiplicity
//	Int_t m = fLADD->GetNMultihit();

	// Count the total number of tagger hits
//	Int_t nhits = 0;
//	for ( i = 0; i < m; i++) nhits += fLADD->GetNhitsM(i);

	fTaggerPhotonNhits = 0;
	fNPrompt = 0;
	fNRandom = 0;

// read-in all the hits
// loop over hit multiplicity
//	for ( i = 0; i < m; i++)
//	{

		Int_t nhits;
		Int_t chan;
		Int_t* hits;
		Double_t* time;

		if ( gAR->GetProcessType() == EMCProcess)
		{
			nhits = fLADD->GetNhits();
			hits = fLADD->GetHits();
			time = fLADD->GetTimeOR();
		}
		else
		{

			// Multiplicity 1 only
			i = 0;

//			// number of hits of current multiplicity
			nhits = fLADD->GetNhitsM( i);
//
//			// hit array of current multiplicity
			hits = fLADD->GetHitsM( i);
//
//			// time array of current multiplicity
			time = fLADD->GetTimeORM( i);
		}

		// loop over hits of current multiplicity
		for ( j = 0; j < nhits; j++)
		{
			chan = hits[j];

			// set hit element, time and energy
			fTaggerPhotonHits[fTaggerPhotonNhits] = chan;
			eBeamPhot = eBeamEnergy - fpdEnergy[chan];
			fTaggerPhotonEnergy[fTaggerPhotonNhits] = eBeamPhot;
			fTaggerPhotonTime[fTaggerPhotonNhits] = time[j]-fTaggTDCOffset[chan];

			if ( ( eBeamPhot >= 280) && ( eBeamPhot <= 340))
			{
				fTaggerTimeCut[fTaggerPhotonNhits] = fTaggerPhotonTime[fTaggerPhotonNhits];
			}
			else
			{
				fTaggerTimeCut[fTaggerPhotonNhits] = -1000;
			}

			p4BeamPhot.SetPxPyPzE( 0, 0, eBeamPhot, eBeamPhot);
			p4incident = fP4target[0] + p4BeamPhot;
			p4missing = p4incident - p4CATS;

			// Either one of these works just fine
			Emiss = p4missing.M() - fP4target[0].M();
			Emiss2 = ComptonScattering( eBeamPhot) - p4CATS.E();

			fCATSTaggerTime[fTaggerPhotonNhits] = fTaggerPhotonTime[fTaggerPhotonNhits] - fCATSTime;

			// Prompt Hits
			if ( (fTaggerPhotonTime[fTaggerPhotonNhits] >= fPhotTimePL && fTaggerPhotonTime[fTaggerPhotonNhits] <= fPhotTimePR) ||
			  	(gAR->GetProcessType() == EMCProcess) ) {

				fTaggerPhotonHitsNSP[fNPrompt] = chan;
				fCATSEnergyNoShieldPrompt[fNPrompt]  = fCATSEnergyNoShield;
//				fEmissP[fNPrompt]  = Emiss;
				fEmissP[fNPrompt]  = Emiss2;

				if ( ( eBeamPhot >= 280) && ( eBeamPhot <= 340))
				{
					fEmissCutP[fNPrompt]  = Emiss2;
				}
				else
				{
					fEmissCutP[fNPrompt]  = -200;
				}

				fNPrompt++;
			}

			// Random Hits
			if (fTaggerPhotonTime[fTaggerPhotonNhits] >= fPhotTimeRL1 && fTaggerPhotonTime[fTaggerPhotonNhits] <= fPhotTimeRR1) {

				fTaggerPhotonHitsNSR[fNRandom++] = chan;
				fCATSEnergyNoShieldRandom[fNRandom]  = fCATSEnergyNoShield;
//				fEmissR[fNRandom]  = Emiss;
				fEmissR[fNRandom]  = Emiss2;

				if ( ( eBeamPhot >= 280) && ( eBeamPhot <= 340))
				{
					fEmissCutR[fNRandom]  = Emiss2;
				}
				else
				{
					fEmissCutR[fNRandom]  = -200;
				}

				fNRandom++;
			}

			fTaggerPhotonNhits++;
		}
//	}

// Apply BufferEnd to the end of all arrays
	fTaggerPhotonHits[fTaggerPhotonNhits] = EBufferEnd;
	fTaggerPhotonEnergy[fTaggerPhotonNhits] = EBufferEnd;
	fTaggerPhotonTime[fTaggerPhotonNhits] = EBufferEnd;

	fTaggerTimeCut[fTaggerPhotonNhits] = EBufferEnd;

	fCATSEnergyNoShieldPrompt[fNPrompt] = EBufferEnd;
	fCATSEnergyNoShieldRandom[fNRandom] = EBufferEnd;
	fTaggerPhotonHitsNSP[fNPrompt] = EBufferEnd;
	fTaggerPhotonHitsNSR[fNRandom] = EBufferEnd;

	fEmissP[fNPrompt] = EBufferEnd;
	fEmissR[fNRandom] = EBufferEnd;
	fEmissCutP[fNPrompt] = EBufferEnd;
	fEmissCutR[fNRandom] = EBufferEnd;

	fCATSTaggerTime[fTaggerPhotonNhits] = EBufferEnd;

// Fill Tree File
	if(fProduceTreeFile == 1) {
		fTree->Fill();
	}
}

ClassImp( TA2CATSPhysics)
