TFile cats_data( "scratch/ARH_all.root");
//TFile cats_data( "scratch/ARH_Day6.root");
TH1D *EM_P = (TH1D*)cats_data.Get( "PHYS_EmissP");
TH1D *EM_R = (TH1D*)cats_data.Get( "PHYS_EmissR");

TH1D *EMC_P = (TH1D*)cats_data.Get( "PHYS_EmissCutP");
TH1D *EMC_R = (TH1D*)cats_data.Get( "PHYS_EmissCutR");

TH1D *tagtime = (TH1D*)cats_data.Get( "PHYS_TaggerTime");
TH1D *ttcut = (TH1D*)cats_data.Get( "PHYS_TaggerTimeCut");

TH2D *EM2D_P = (TH2D*)cats_data.Get( "PHYS_EmissP_v_TaggerEnergy");
TH2D *EM2D_R = (TH2D*)cats_data.Get( "PHYS_EmissR_v_TaggerEnergy");

//const Double_t rPR = -0.0375;
//const Double_t rPR = -0.3;

// This is from the fit below
const Double_t rPR = -0.01347;

void EMiss( UInt_t rebin = 1)
{

	TString name;

	TCanvas *c1 = new TCanvas ( "c1", "EMiss", 20, 350, 500, 700);
	c1->Divide( 1, 2);

	c1->cd( 1);
	EM_P->Draw();

	c1->cd( 2);
	EM_R->Draw();

	TH1D *EM_S = new TH1D( "EM_S", "Subtracted", 200, -100, 100);

	EM_S->Sumw2();
	EM_S->Add( EM_P, EM_R, 1, rPR);

	TCanvas *c2 = new TCanvas ( "c2", "EMissS", 300, 400, 500, 500);
	EM_S->Draw();

	c1->Print( "plots/EMiss.pdf");

}

void EMissCut( UInt_t rebin = 1)
{

	TString name;

	TCanvas *c1 = new TCanvas ( "c1", "EMissCut", 20, 350, 500, 700);
	c1->Divide( 1, 2);

	c1->cd( 1);
	EMC_P->Draw();

	c1->cd( 2);
	EMC_R->Draw();

	TH1D *EMC_S = new TH1D( "EMC_S", "Subtracted", 200, -100, 100);

	EMC_S->Sumw2();
	EMC_S->Add( EMC_P, EMC_R, 1, rPR);

	TCanvas *c2 = new TCanvas ( "c2", "EMissCutS", 300, 400, 500, 500);
	EMC_S->Draw();
	EMC_S->GetXaxis()->SetRangeUser( -20, 20);

	c2->Print( "plots/EMissCut.pdf");

}
void EMiss2D( UInt_t rebin = 1)
{

	TString name;

	TCanvas *c1 = new TCanvas ( "c1", "EMiss2D", 20, 350, 500, 500);

	TH2D *EM2D_S = new TH2D( "EM2D_S", "Subtracted", 900, 0, 900, 200, -100, 100);

	EM2D_S->Sumw2();
	EM2D_S->Add( EM2D_P, EM2D_R, 1, rPR);
	EM2D_S->GetXaxis()->SetRangeUser( 290, 330);
//	EM2D_S->Draw();

	TH1D *proj = new TH1D( "proj", "Projection", 200, -100, 100);
	proj = EM2D_S->ProjectionY( "Em");
	proj->Draw();

//	c1->Print( "plots/EMiss.pdf");

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
	ttcut->GetXaxis()->SetRangeUser( -300, 500);
	ttcut->SetMinimum( 0);
	ttcut->Draw();

	TF1 *f1 = new TF1( "f1", "pol1", -50, 350);
	ttcut->Fit( "f1", "R");
	f1->GetParameters( &par[0]);

	x[0] = -115;
	x[1] = -110;

	w = x[1] - x[0];
	y[0] = par[0] + x[0]*par[1];
	y[1] = par[0] + x[1]*par[1];
	l = (y[0]+y[1])/2;
	AP = w*l;

	cout << " AP = " << AP;

	x[0] = -50;
	x[1] = 350;

	w = x[1] - x[0];
	y[0] = par[0] + x[0]*par[1];
	y[1] = par[0] + x[1]*par[1];
	l = (y[0]+y[1])/2;
	AR = w*l;

	cout << "  AR = " << AR;

	ratio = AP/AR;

	cout << "  ratio = " << ratio;
	cout << endl;

	c1->Print( "plots/TaggerTime.pdf");

}
