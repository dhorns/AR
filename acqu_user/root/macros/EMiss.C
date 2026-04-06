// Tagger Energy
typedef struct {

	Int_t egamma;
	Double_t energy;
	Double_t denergy;

} TData;

TData tdata[328];

// Data Histogram File
TFile cats_data( "ARout/CATS/ARH_all.root");

// Tagger Time for Random Subtraction
TH1D *tt = (TH1D*)cats_data.Get( "PHYS_TaggerTime");

// 2D Tagger Channel vs. Missing Energy
TH2D *hP = (TH2D*)cats_data.Get( "PHYS_TaggerChannelCATSP_v_EmissP");
TH2D *hR = (TH2D*)cats_data.Get( "PHYS_TaggerChannelCATSR_v_EmissR");

// Prompt-Accidental Subtraction Ratio
// From the fit below
//const Double_t rPR = -0.0375;
//const Double_t rPR = -0.3;
//const Double_t rPR = -0.01347;
const Double_t rPR = 0.0458;

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

void EMiss2D( UInt_t rebin = 1)
{

	TString name;

	gROOT->ProcessLine( "ReadTagEng883()");

	TCanvas *c1 = new TCanvas ( "c1", "EMiss", 20, 350, 1000, 500);
	c1->Divide( 3, 1);

	c1->cd( 1);
	hP->SetTitle( "Prompt");
	hP->Draw();

	c1->cd( 2);
	hR->SetTitle( "Random");
	hR->Draw();

	c1->cd( 3);
	TH2D *hS = (TH2D*)hP->Clone( "subt");
	hS->SetTitle( "Subtracted");
	hS->Sumw2();
	hS->Add( hR, -rPR);
	hS->Draw();

	hS2 = (TH2D*)hS->Clone( "binned");
	hS2->SetTitle( "Binned");

	c1->Print( "plots/CATS/EMiss2D.pdf");

}

// Projects Missing Mass from tagger channel 'lo' to tagger channel 'hi'
void ProjEMiss( UInt_t lo, UInt_t hi)
{
	UInt_t eg_l, eg_h;
	TString name;

	eg_l = tdata[lo].egamma;
	eg_h = tdata[hi].egamma;

	TCanvas *c1 = new TCanvas ( "c1", "EMiss", 20, 350, 700, 500);

	hS2->GetYaxis()->SetRange( lo, hi);
	TH1D *proj = hS2->ProjectionX( "projX");
	proj->Draw();
	proj->SetTitle( "Missing Energy");

	TPaveText *pt = new TPaveText( 0.2, 0.4, 0.8, 0.9, "NDC");
	pt->SetBorderSize( 0);
	pt->SetFillStyle( 0);
	pt->SetTextAlign( 12);
	pt->SetTextSize( 0.05);
	name = Form( "E_{#gamma} = %d - %d MeV\n", eg_l, eg_h);
	pt->AddText( name);
	pt->Draw();

	name = Form( "plots/CATS/EMiss_%d-%d_MeV.pdf", eg_l, eg_h);
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

	TF1 *f1 = new TF1( "f1", "pol2", -50, 300);
	tt->Fit( "f1", "R");
	f1->GetParameters( &par[0]);
	f1->SetLineColor( 4);
	f1->SetLineWidth( 4);
	f1->Draw( "same");

	TF1 *f2 = new TF1("f", "[0]*x + 0.5*[1]*x*x + 0.3333*[2]*x*x*x", -1000, 1000);
	f2->SetParameters( par[0], par[1], par[2]);

	x[0] = -115;
	x[1] = -100;

	AP = f2->Eval( x[1]) - f2->Eval( x[0]);
	cout << " AP = " << AP;

	x[0] = -50;
	x[1] = 350;

	AR = f2->Eval( x[1]) - f2->Eval( x[0]);

	cout << "  AR = " << AR;

	ratio = AP/AR;

	cout << "  ratio = " << ratio;
	cout << endl;

//	c1->Print( "plots/TaggerTimeCut.pdf");

}
