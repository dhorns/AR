// Histogram File
TFile he4_mcdata( "ARout/CB/MC_He4_210MeV.root");

//
TH1D *MissEPi0 = (TH1D*)he4_mcdata.Get( "PHYS_MissingEnergyPromptPi0");

void Eff( UInt_t chan);
{
	UInt_t binlo, binhi;
	Double_t yield, eff;

	// Integrate EMiss from -20 to 40
	binlo = MissEPi0->GetXaxis()->FindBin( -20);
	binhi = MissEPi0->GetXaxis()->FindBin( 40);
	yield = MissEPi0->Integral( binlo, binhi);

	eff = yield/1e6;

	cout << "eff = " << eff << endl;

}
