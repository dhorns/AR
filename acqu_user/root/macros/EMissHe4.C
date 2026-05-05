// Tagger Energy
typedef struct {

	Int_t egamma;
	Double_t energy;
	Double_t denergy;
	Int_t scaler;

} TData;

TData tdata[328];

// Histogram File
TFile he4_data( "ARout/CB/ARH_He4.root");

// Tagger-Pi0 Time for Random Subtraction
TH1D *tt = (TH1D*)he4_data.Get( "PHYS_TaggerPi0Time");

// Tagger Sum Scalers
TH1D *ts = (TH1D*)he4_data.Get( "SumScalers2000to3000");
TH1D *ss = new TH1D( "TaggerScalers", "TaggerScalers", 328, 0, 327);

// 2D Tagger Channel vs. Missing Energy
TH2D *hP = (TH2D*)he4_data.Get( "PHYS_TaggerChannelPromptPi0_v_MissingEnergyPromptPi0");
TH2D *hR = (TH2D*)he4_data.Get( "PHYS_TaggerChannelRandomPi0_v_MissingEnergyRandomPi0");

// Prompt-Accidental Subtraction Ratio
const Double_t rPR = 0.0636;

// Target thickness in nuclei/cm^2 for the 5-cm cell.
const Double_t t_cm2 = 0.940e23;
// Converts cm^2 to microbarn.
const Double_t t_ub = 1e30/t_cm2;
// Approximate Detection Efficiency
//const Double_t edet = 0.64;
// For 210 MeV
//const Double_t edet = 0.73;
const Double_t edet = 0.50;
// Approximate Tagging Efficiency
const Double_t etag = 0.185;

TH2D *hS2;

TH1D *scalers;

void ReadTagEng( Int_t eg)
{
	UInt_t i, sc;
	Double_t eff, deff;
	TString file;

	file = Form( "includes/tageng%d.dat", eg);

	ifstream inFile( file);
	while( !inFile.eof()) {
		inFile >> i >> eff >> deff >> sc;
		tdata[i].energy = deff;
		tdata[i].egamma = (int)(deff + 0.5);
		tdata[i].scaler = sc;
	}
	inFile.close();
}

void EMissHe4()
{

	TString name;

	gROOT->ProcessLine( "ReadTagEng(883)");

	TCanvas *c1 = new TCanvas ( "c1", "EMissHe4", 200, 350, 1000, 500);
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

//	c1->Print( "plots/CB/EMissHe4.pdf");
	c1->Print( "plots/CB/emiss_data.eps");

}

void ProjEMiss( UInt_t chan)
{
	UInt_t eg;
	TString name;

	eg = tdata[chan].egamma;

	TCanvas *c1 = new TCanvas ( "c1", "EMissHe4", 20, 350, 700, 500);

	hS2->GetYaxis()->SetRange( chan, chan);
	TH1D *proj = hS2->ProjectionX( "projX");
	proj->Draw();
	proj->SetTitle( "Missing Energy");

	TPaveText *pt = new TPaveText( 0.2, 0.4, 0.8, 0.9, "NDC");
	pt->SetBorderSize( 0);
	pt->SetFillStyle( 0);
	pt->SetTextAlign( 12);
	pt->SetTextSize( 0.05);
	name = Form( "E_{#gamma} = %d MeV\n", eg);
	pt->AddText( name);
	pt->Draw();

//	name = Form( "plots/CB/EMissHe4_%dMeV.pdf", eg);
	name = Form( "plots/CB/EMissHe4_%dMeV.eps", eg);
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

//	c1->Print( "plots/CB/TaggerTimeCut.pdf");

}

void TaggerScalers()
{
	UInt_t i, s_chan;
	Double_t scal;

	gROOT->ProcessLine( "ReadTagEng(883)");

	for ( i = 0; i < 328; i++)
	{
		s_chan = tdata[i].scaler;
		scal = ts->GetBinContent( s_chan-1999);
		ss->SetBinContent( i, scal);

//		cout << i;
//		cout << " " << s_chan;
//		cout << " " << scal;
//		cout << endl;

	}

	scalers = (TH1D*)ss->Clone( "scalers");

	TCanvas *c1 = new TCanvas ( "c1", "Scalers", 20, 350, 700, 500);
	ss->Draw();

}

void XS( UInt_t chan)
{
	Int_t binlo, binhi;
	Double_t yield, tscalers, ratio, xs;

	hS2->GetYaxis()->SetRange( chan, chan);
	TH1D *emiss = hS2->ProjectionX( "projX");

	// Integrate EMiss from -20 to 40
	binlo = emiss->GetXaxis()->FindBin( -20);
	binhi = emiss->GetXaxis()->FindBin( 40);
	yield = emiss->Integral( binlo, binhi);

	tscalers = scalers->GetBinContent( chan);

	ratio = yield/tscalers;

	xs = ratio*t_ub/edet/etag;

	cout << chan;
	cout << " " << tdata[chan].egamma;
	cout << " " << yield;
	cout << " " << tscalers;
	cout << " " << ratio;
	cout << " " << edet;
	cout << " " << etag;
	cout << " " << xs;
	cout << endl;

}
