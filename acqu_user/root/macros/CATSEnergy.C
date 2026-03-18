
// Tagger Energy
typedef struct {

	Int_t egamma;
	Double_t energy;
	Double_t denergy;

} TData;

TData tdata[328];

// Histogram File
TFile CATSdata( "ARout/CATS/ARH_InBeam.root");

// Tagger-CATS Time for Random Subtraction
TH1D *tt = (TH1D*)CATSdata.Get( "PHYS_CATSTaggerTime");

// 2D Tagger Channel vs. CATS Energy
TH2D *hP = (TH2D*)CATSdata.Get( "PHYS_TaggerHitsNSP_v_CATSEnergyNoShieldPrompt");
TH2D *hR = (TH2D*)CATSdata.Get( "PHYS_TaggerHitsNSR_v_CATSEnergyNoShieldRandom");

// Prompt-Accidental Subtraction Ratio
const Double_t rPR = 0.0636;

TH2D *hS2;

void ReadTagEng883()
{
	UInt_t i;
	Double_t eff, deff;
	TString file = "includes/tageng883.dat";

	ifstream inFile( file);
	while( !inFile.eof()) {
		inFile >> i >> eff >> deff;
		tdata[i].energy = deff;
		tdata[i].egamma = (int)(deff + 0.5);
	}
	inFile.close();
}

void CATSEnergy()
{

	Double_t par[3];
	TString name;

	gROOT->ProcessLine( "ReadTagEng883()");

	TCanvas *c1 = new TCanvas ( "c1", "EMissCATS", 200, 350, 1200, 500);
	c1->Divide( 3, 1);

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

	hS2 = (TH2D*)hS->Clone( "binned");
	hS2->SetTitle( "Binned");

	c1->Print( "plots/CATSEnergy-2D.pdf");

}
void ProjCATSE( UInt_t chan)
{
	UInt_t eg;
	TString name;

	eg = tdata[chan].egamma;

	TCanvas *c1 = new TCanvas ( "c1", "CATSEnergy", 20, 350, 700, 500);

	hS2->GetYaxis()->SetRange( chan, chan);
	TH1D *proj = hS2->ProjectionX( "projX");
	proj->Draw();
	proj->SetTitle( "CATS Energy");
	proj->GetXaxis()->SetRangeUser( 0, 1.25*eg); 

	Double_t x[2], y[2];

	x[0] = 0.15;
	x[1] = 0.35;
	y[0] = 0.45;
	y[1] = 0.55;

	TPaveText *pt = new TPaveText( x[0], y[0], x[1], y[1], "NDC");
	pt->SetBorderSize( 0);
	pt->SetFillStyle( 0);
	pt->SetTextAlign( 12);
	pt->SetTextSize( 0.04);
	name = Form( "E_{#gamma} = %d MeV\n", eg);
	pt->AddText( name);
	pt->Draw();

	name = Form( "plots/CATSEnergy_%dMeV.pdf", eg);
	c1->Print( name);

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
