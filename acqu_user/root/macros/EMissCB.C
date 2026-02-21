TFile compton_data( "histograms/ARH_comptonC12.root");
TFile pi0_data( "histograms/ARH_pi0C12.root");
TH1D *EM_comp = (TH1D*)compton_data.Get( "PHYS_MissingEnergyPrompt");
TH1D *EM_pi0 = (TH1D*)pi0_data.Get( "PHYS_MissingEnergyPrompt");

void EMissCB()
{

	Double_t par[3];
	TString name;

	TCanvas *c1 = new TCanvas ( "c1", "EMiss", 20, 350, 500, 700);
	c1->Divide( 1, 2);

	c1->cd( 1);
	EM_comp->SetTitle( "Compton");
	EM_comp->Draw();

	TF1 *f1 = new TF1( "f1", "gaus", -20, 30);
	EM_comp->Fit( "f1", "R");
	f1->GetParameters( &par[0]);
	cout << " sigma = " << par[2] << endl;

	TPaveText *pl = new TPaveText( 0.2, 0.4, 0.8, 0.9, "NDC");
	pl->SetBorderSize( 0);
	pl->SetFillStyle( 0);
	pl->SetTextAlign( 12);
	pl->SetTextSize( 0.05);
	name = Form( "#sigma = %5.1f MeV\n", par[2]);
	pl->AddText( name);
	pl->Draw();

	c1->cd( 2);
	EM_pi0->Draw();
	EM_pi0->SetTitle( "Pi0");

	TPaveText *pl2 = new TPaveText( 0.2, 0.4, 0.8, 0.9, "NDC");
	pl2->SetBorderSize( 0);
	pl2->SetFillStyle( 0);
	pl2->SetTextAlign( 12);
	pl2->SetTextSize( 0.05);
	name = Form( "Unscaled\n");
	pl2->AddText( name);
	pl2->Draw();

	c1->Print( "plots/EMissCB.pdf");

}
