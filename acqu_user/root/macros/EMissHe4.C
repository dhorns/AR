TFile he4_data( "ARout/CB/ARH.root");
TH1D *EM_P = (TH1D*)he4_data.Get( "PHYS_MissingEnergyPromptPi0Cut");
TH1D *EM_R = (TH1D*)he4_data.Get( "PHYS_MissingEnergyRandomPi0Cut");
TH1D *ttcut = (TH1D*)he4_data.Get( "PHYS_TaggerPi0TimeCut");

const Double_t rPR = 0.0707;

void EMissHe4()
{

	Double_t par[3];
	TString name;

	TCanvas *c1 = new TCanvas ( "c1", "EMissHe4", 20, 350, 500, 1000);
	c1->Divide( 1, 3);

	c1->cd( 1);
	EM_P->SetTitle( "Prompt");
	EM_P->Draw();

//	TF1 *f1 = new TF1( "f1", "gaus", -20, 30);
//	EM_comp->Fit( "f1", "R");
//	f1->GetParameters( &par[0]);
//	cout << " sigma = " << par[2] << endl;

//	TPaveText *pl = new TPaveText( 0.2, 0.4, 0.8, 0.9, "NDC");
//	pl->SetBorderSize( 0);
//	pl->SetFillStyle( 0);
//	pl->SetTextAlign( 12);
//	pl->SetTextSize( 0.05);
//	name = Form( "#sigma = %5.1f MeV\n", par[2]);
//	pl->AddText( name);
//	pl->Draw();

	c1->cd( 2);
	EM_R->Draw();
	EM_R->SetTitle( "Random");

//	TPaveText *pl2 = new TPaveText( 0.2, 0.4, 0.8, 0.9, "NDC");
//	pl2->SetBorderSize( 0);
//	pl2->SetFillStyle( 0);
//	pl2->SetTextAlign( 12);
//	pl2->SetTextSize( 0.05);
//	name = Form( "Unscaled\n");
//	pl2->AddText( name);
//	pl2->Draw();

	c1->cd( 3);
	TH1D *EM_S = (TH1D*)EM_P->Clone( "subt");
	EM_S->Sumw2();
	EM_S->Add( EM_R, rPR);
	EM_S->Draw();

	c1->Print( "plots/EMissHe4.pdf");

}

void TaggerTime( UInt_t rebin = 1)
{

	Double_t par[2];
	Double_t x[2], y[2];
	Double_t AR, AP, ratio;
	Double_t w, l;
	TString name;

	TCanvas *c1 = new TCanvas ( "c1", "Tagger Time", 20, 350, 500, 500);
//	tagtime->GetXaxis()->SetRangeUser( -300, 100);
//	tagtime->Draw();
	ttcut->GetXaxis()->SetRangeUser( -500, 500);
	ttcut->SetMinimum( 0);
	ttcut->Draw();

	TF1 *f1 = new TF1( "f1", "pol1", -300, -100);
	ttcut->Fit( "f1", "R");
	f1->GetParameters( &par[0]);

	x[0] = -25;
	x[1] = -10;

	w = x[1] - x[0];
	y[0] = par[0] + x[0]*par[1];
	y[1] = par[0] + x[1]*par[1];
	l = (y[0]+y[1])/2;
	AP = w*l;

	cout << " AP = " << AP;

	x[0] = -300;
	x[1] = -100;

	w = x[1] - x[0];
	y[0] = par[0] + x[0]*par[1];
	y[1] = par[0] + x[1]*par[1];
	l = (y[0]+y[1])/2;
	AR = w*l;

	cout << "  AR = " << AR;

	ratio = AP/AR;

	cout << "  ratio = " << ratio;
	cout << endl;

	c1->Print( "plots/TaggerTimeCut.pdf");

}
