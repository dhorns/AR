
// Tagger Energy
typedef struct {

	Int_t egamma;
	Double_t energy;
	Double_t denergy;
	Int_t scaler;

} TData;

TData tdata[328];

// Histogram File
TFile he4_data( "ARout/CB/ARH.root");

// Tagger-Pi0 Time for Random Subtraction
TH1D *tt = (TH1D*)he4_data.Get( "PHYS_TaggerPi0Time");

// 2D Tagger Channel vs. Missing Energy
TH2D *hP = (TH2D*)he4_data.Get( "PHYS_TaggerChannelPromptPi0_v_MissingEnergyPromptPi0");
TH2D *hR = (TH2D*)he4_data.Get( "PHYS_TaggerChannelRandomPi0_v_MissingEnergyRandomPi0");

// Prompt-Accidental Subtraction Ratio
const Double_t rPR = 0.0636;

TH2D *hS2;
//TH2D *hS2 = nullptr;
TH1D* proj_global = nullptr; //added by Zoey

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

	Double_t par[3];
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

	c1->Print( "plots/EMissHe4.pdf");

}
void ProjEMiss( UInt_t chan)
{
	UInt_t eg;
	TString name;

	eg = tdata[chan].egamma;

	TCanvas *c1 = new TCanvas ( "c1", "EMissHe4", 20, 350, 700, 500);

	// Dave's Code
//	hS2->GetYaxis()->SetRange( chan, chan);
//	TH1D *proj = hS2->ProjectionX( "projX");
//	proj->Draw();
//	proj->SetTitle( "Missing Energy");

	// Zoey's Code
	hS2->GetYaxis()->SetRange( chan, chan);
	proj_global = hS2->ProjectionX( "projX"); //added by Zoey
	proj_global->Draw();
	proj_global->SetTitle( "Missing Energy");
	proj_global->GetXaxis()->SetRangeUser( 0, 1.25*eg);

	Double_t x[2],y[2];

	x[0]=0.15;
	x[1]=0.35;
	y[0]=0.45;
	y[1]=0.55;

	TPaveText *pt = new TPaveText( 0.2, 0.4, 0.8, 0.9, "NDC");
	pt->SetBorderSize( 0);
	pt->SetFillStyle( 0);
	pt->SetTextAlign( 12);
	pt->SetTextSize( 0.05);
	name = Form( "E_{#gamma} = %d MeV\n", eg);
	pt->AddText( name);
	pt->Draw();

	name = Form( "plots/EMissHe4_%dMeV.pdf", eg);
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

//added by Zoey
void MCvsData( UInt_t chan)
{
	ProjEMiss( chan);
	TH1D* h_ARH = proj_global;

	//debugging
	if (!h_ARH){
		std::cerr<<"ERROR: Failed to retrieve ProjEMiss" << std::endl;
	return;
	}

	//to denote the incident photon energy as done in ProjEMiss()
	UInt_t eg;
	TString name;

	eg = tdata[chan].egamma;

	//calling in the Monte Carlo sim
	TFile *f_mc = TFile::Open( "ARout/CB/MC_He4.root");

	if (!f_mc || f_mc->IsZombie()) {
   	 	std::cerr << "ERROR: Could not open MC file" << std::endl;
   	 	return;
	}

	f_mc->ls();  // DEBUG: see what's inside

	TH1F* h_mc = dynamic_cast<TH1F*>(f_mc->Get( "PHYS_MissingEnergyPromptPi0"));
	if (!h_mc){
    		std::cerr << "ERROR: Histogram PHYS_MissingEnergyPrompt not found or wrong type" << std::endl;
   	 	return;
	}

//	TH1* h_mc = h_mc2D->ProjectionX("h_mc");

	TCanvas *c = new TCanvas("c_compare", "Data vs Simulation", 800, 600);

	//Style for real data
	h_ARH ->SetMarkerStyle(20);
	h_ARH ->SetMarkerColor(kRed);

	//Style for MC simulation
	h_mc ->SetMarkerStyle(21);
	h_mc ->SetMarkerColor(kBlue);
	
	//scale simulated histogram
/*	int peakBin = h_ARH->GetMaximumBin();
	double peakCentre = h_ARH->GetBinCenter(peakBin);

	double width = 45.0;
	double xmin = peakCentre - width;
	double xmax = peakCentre + width;

	int bin_min = h_ARH->FindBin(xmin);
	int bin_max = h_ARH->FindBin(xmax);

	double data_int = h_ARH->Integral(bin_min, bin_max);
	double mc_int = h_mc->Integral(bin_min, bin_max);

	if (mc_int>0){
		double scale = data_int / mc_int;
		h_mc->Scale(scale);
		std::cout << "scaling MC by factor: " << scale << std::endl;
	}
	else{
		std::cerr <<"Error: MC integral is zero in selected region" << std::endl;
	}


	//draw histogram

	//double maxY = std::max(h_ARH->GetMaximum(), h_mc->GetMaximum());
	double maxY = h_ARH->GetMaximum();
	maxY *= 1.1;
	double maxY2 = h_mc->GetMaximum();
	maxY2 *= 1.1;
	h_ARH->SetMaximum(maxY,maxY2); */

	// scale simulated histogram
	int peakBin = h_ARH->GetMaximumBin();
	double peakCentre = h_ARH->GetBinCenter(peakBin);

	double width = 45.0;
	double xmin = peakCentre - width;
	double xmax = peakCentre + width;

	int bin_min = h_ARH->FindBin(xmin);
	int bin_max = h_ARH->FindBin(xmax);

	double data_int = h_ARH->Integral(bin_min, bin_max);
	double mc_int = h_mc->Integral(bin_min, bin_max);

	if (mc_int > 0) {
   		double scale = data_int / mc_int;
  	  	h_mc->Scale(scale);
    		std::cout << "scaling MC by factor: " << scale << std::endl;
	}	
	else {
    		std::cerr << "Error: MC integral is zero in selected region" << std::endl;
	}

// draw histograms
	double maxY = std::max(h_ARH->GetMaximum(), h_mc->GetMaximum()); // largest peak
	maxY *= 1.1; // add 10% margin

	h_ARH->SetMaximum(maxY);
	h_mc->SetMaximum(maxY);

	h_ARH->Draw("E");
	h_mc->Draw("HIST SAME");


	std::cout << "max for h_ARH: " << h_ARH->GetMaximum() << std::endl;
	std::cout << "max for h_mc: " << h_mc->GetMaximum() << std::endl;

	// Set X-axis range
	h_ARH->GetXaxis()->SetRangeUser(-100, 100);

	// Set Y-axis range
	//h_ARH->GetYaxis()->SetRangeUser(0, 100);
	
	//setting axis title
	h_ARH->GetXaxis()->SetTitle("Energy(MeV)");
	h_ARH->GetYaxis()->SetTitle("Counts");
	//setting axis size and locations
	h_ARH->GetXaxis()->SetTitleSize(0.045);
	h_ARH->GetYaxis()->SetTitleSize(0.045);
	h_ARH->GetXaxis()->SetLabelSize(0.035);
	h_ARH->GetYaxis()->SetLabelSize(0.035);

	h_ARH->Draw("E");
	h_mc->Draw("HIST SAME");

	//denote incident photon energy
	Double_t x[2], y[2];

	x[0]=0.15;
	x[1]=0.35;
	y[0]=0.45;
	y[1]=0.55;

	TPaveText *pt = new TPaveText(x[0],y[0],x[1],y[1],"NDC");
	pt->SetBorderSize(0);
	pt->SetFillStyle(0);
	pt->SetTextAlign(12);
	pt->SetTextSize(0.04);

	name = Form("E_{#gamma} = %d MeV", eg);
	pt->AddText(name);
	pt->Draw();

	//legend
	TLegend *leg = new TLegend(0.75, 0.4, 0.88, 0.58);
//	leg->SetHeader("Missing Energy", "C");
	leg->AddEntry(h_ARH, "Data", "lep");
	leg->AddEntry(h_mc, "Simulation", "l");
	leg->Draw();

	c->Print("plots/MCvsData.pdf");
}


