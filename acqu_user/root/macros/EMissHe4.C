//TFile he4_data( "ARout/CB/ARH.root");
TFile he4_data( "ARH.root");
//TH1D *EM_P = (TH1D*)he4_data.Get( "PHYS_MissingEnergyPromptPi0Cut");
//TH1D *EM_R = (TH1D*)he4_data.Get( "PHYS_MissingEnergyRandomPi0Cut");

TH1D *tt = (TH1D*)he4_data.Get( "PHYS_TaggerPi0Time");

TH2D *hP = (TH2D*)he4_data.Get( "PHYS_TaggerChannelPromptPi0_v_MissingEnergyPromptPi0");
TH2D *hR = (TH2D*)he4_data.Get( "PHYS_TaggerChannelRandomPi0_v_MissingEnergyRandomPi0");

const Double_t rPR = 0.0636;

TH2D *hS2;

void EMissHe4()
{

	Double_t par[3];
	TString name;

	TCanvas *c1 = new TCanvas ( "c1", "EMissHe4", 20, 350, 1200, 500);
	c1->Divide( 4, 1);

	c1->cd( 1);
	hP->SetTitle( "Prompt");
	hP->Draw();

	c1->cd( 2);
	hR->Draw();
	hR->SetTitle( "Random");

	c1->cd( 3);
	TH2D *hS = (TH2D*)hP->Clone( "subt");
	hS->SetTitle( "Subtracted");
	hS->Sumw2();
	hS->Add( hR, -rPR);
	hS->Draw();

	c1->cd( 4);
//	TH2D *hS2 = (TH2D*)hS->Clone( "binned");
	hS2 = (TH2D*)hS->Clone( "binned");
	hS2->SetTitle( "Binned");
//	hS2->GetYaxis()->SetRange( 50, 50);
//	TH1D *proj = hS2->ProjectionX( "projX");
//	proj = hS2->ProjectionX( "projX");
	hS2->Draw();

//	c1->Print( "plots/EMissHe4.pdf");

}
void ProjEMiss( UInt_t lo, UInt_t hi)
{

	TCanvas *c1 = new TCanvas ( "c1", "EMissHe4", 20, 20, 700, 500);

	hS2->GetYaxis()->SetRange( lo, hi);
	TH1D *proj = hS2->ProjectionX( "projX");
	proj->Draw();

//	c1->Print( "plots/EMissHe4.pdf");

}

void TaggerTime( UInt_t rebin = 1)
{

	Double_t par[3];
	Double_t x[2], y[2], z[2];
	Double_t AR, AP, ratio;
	Double_t w, l;
	TString name;

	TCanvas *c1 = new TCanvas ( "c1", "Tagger Time", 20, 350, 500, 500);
	tt->GetXaxis()->SetRangeUser( -500, 500);
	tt->SetMinimum( 0);
	tt->Draw();

	TF1 *f1 = new TF1( "f1", "pol2", -300, -100);
	tt->Fit( "f1", "R");
	f1->GetParameters( &par[0]);
	f1->SetLineWidth( 4);

	TF1 *f2 = new TF1("f", "[0]*x + 0.5*[1]*x*x + 0.3333*[2]*x*x*x", -1000, 1000);
	f2->SetParameters( par[0], par[1], par[2]);

	x[0] = -25;
	x[1] = -10;

	AP = f2->Eval( x[1]) - f2->Eval( x[0]);
	cout << " AP = " << AP;

	x[0] = -300;
	x[1] = -100;

	AR = f2->Eval( x[1]) - f2->Eval( x[0]);

	cout << "  AR = " << AR;

	ratio = AP/AR;

	cout << "  ratio = " << ratio;
	cout << endl;

//	c1->Print( "plots/TaggerTimeCut.pdf");

}
