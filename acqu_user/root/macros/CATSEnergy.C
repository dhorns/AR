
// Tagger Energy
typedef struct {

	Int_t egamma;
	Double_t energy;
	Double_t denergy;

} TData;

TData tdata[328];

// Histogram File
TFile CATSdata( "ARout/CATS/ARH_InBeam.root");
//TFile CATSdata( "ARout/CATS/ARH_all.root");

// Tagger-CATS Time for Random Subtraction
TH1D *tt = (TH1D*)CATSdata.Get( "PHYS_CATSTaggerTime");

// 2D Tagger Channel vs. CATS Energy
TH2D *hP = (TH2D*)CATSdata.Get( "PHYS_TaggerChannelCATSP_v_CATSEnergyNoShieldPrompt");
TH2D *hR = (TH2D*)CATSdata.Get( "PHYS_TaggerChannelCATSR_v_CATSEnergyNoShieldRandom");

// Prompt-Accidental Subtraction Ratio
const Double_t rPR = 0.0636;

TH2D *hS2;
TH1D* proj_global = nullptr;//Added by Gen

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
//	Commented out as proj needed to become proj_golbal to be used in MCvsData 	
/*	TH1D *proj = hS2->ProjectionX( "projX");
	proj->Draw();
	proj->SetTitle( "CATS Energy");
	proj->GetXaxis()->SetRangeUser( 0, 1.25*eg); */
// 	Added by Gen:
	proj_global = hS2->ProjectionX("projX"); 
	proj_global->Draw();
	proj_global->SetTitle( "CATS Energy");
	proj_global->GetXaxis()->SetRangeUser( 0, 1.25*eg); 
	
	proj_global->GetXaxis()->SetTitle("Energy (MeV)");
	proj_global->GetYaxis()->SetTitle("Counts");
	proj_global->GetXaxis()->SetTitleSize(0.045);
	proj_global->GetYaxis()->SetTitleSize(0.045);
	proj_global->GetXaxis()->SetLabelSize(0.035);
	proj_global->GetYaxis()->SetLabelSize(0.035);


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

void MCvsData0(UInt_t chan){ //Added by gen
	//attempting to collect the histogram from void CATSEnergy above
	ProjCATSE(chan);
	TH1D* h_ARH = proj_global;

	//Debugging attempt
 	if (!h_ARH) {
        	std::cerr << "Error: Failed to retrive ProjCATSE!" << std::endl;
        return;	
	}
	
	//To denote the incident photon energy as done in ProjCATSE()	
	UInt_t eg;
	TString name;

	eg = tdata[chan].egamma;

	//calling in the Monte Carlo simulation 
	TFile *f_mc = TFile::Open("ARout/CATS/MC_InBeam.root");
	//Testing error message if Monte Carlo simulation has no plot
	TH1F* h_mc = dynamic_cast<TH1F*>(f_mc ->Get("PHYS_CATSEnergy"));

	if (!h_mc){
		std::cerr << "Error: MC histogram not found!" <<  std::endl;
	}
	//Attempting to scale the simulated histogram here:
	int peakBin = h_ARH->GetMaximumBin();
    	double peakCentre = h_ARH->GetBinCenter(peakBin);

	//attempting to plot with axes from peak +/- 20% intergration regions 
	double frac = 0.10; // 20%

	double xmin = peakCentre * (1.0 - frac);
	double xmax = peakCentre * (1.0 + frac);

	int data_bin_min = h_ARH->FindBin(xmin);
	int data_bin_max = h_ARH->FindBin(xmax);
	
    	double data_int = h_ARH->Integral(data_bin_min, data_bin_max);

	int mc_bin_min = h_mc->FindBin(xmin);
	int mc_bin_max = h_mc->FindBin(xmax);

	double mc_int = h_mc->Integral(mc_bin_min, mc_bin_max);

	//Scaling the histogram: 
	TH1F* h_mc_scaled = nullptr; 
	if (mc_int > 0) {
    		double scale = data_int / mc_int;
    		h_mc_scaled = (TH1F*)h_mc->Clone("h_mc_scaled");
    		h_mc_scaled->Scale(scale);
    		std::cout << "Scaling MC by factor: " << scale << std::endl;
		} 
	else {
   	 	std::cerr << "Error: MC integral is zero in selected region!" << std::endl;
	}

	TCanvas *c = new TCanvas("c_comapre", "Data vs Simulation", 800, 600);
	
	//Style for ARH
	h_ARH->SetMarkerStyle(20);
	h_ARH->SetMarkerColor(kRed);
	h_ARH->SetLineWidth(0);
	//Style for MC
	if (h_mc_scaled){ 
        	h_mc_scaled->SetMarkerStyle(21);
        	h_mc_scaled->SetMarkerColor(kBlue);
	}

	double maxY = h_mc_scaled ? std::max(h_ARH->GetMaximum(), h_mc_scaled->GetMaximum())
                          : h_ARH->GetMaximum();
	h_ARH->SetMaximum(1.1 * maxY);

	//setting axis title, size and location
	h_ARH->GetXaxis()->SetTitle("Energy (MeV)");   
	h_ARH->GetYaxis()->SetTitle("Counts");      
	h_ARH->GetXaxis()->SetTitleSize(0.045);
	h_ARH->GetYaxis()->SetTitleSize(0.045);
	h_ARH->GetXaxis()->SetLabelSize(0.035);
	h_ARH->GetYaxis()->SetLabelSize(0.035);

	h_ARH->GetXaxis()->SetRangeUser(xmin, xmax);

	if(h_mc_scaled){
		h_mc_scaled->GetXaxis()->SetRangeUser(xmin,xmax);
	}

	h_ARH->Draw("E1 P");
	if (h_mc_scaled) h_mc_scaled->Draw("HIST SAME");

	//To denote the incident photon energy as done in ProjCATSE()
	Double_t x[2], y[2];

    	x[0] = 0.15;
    	x[1] = 0.35;
    	y[0] = 0.45;
    	y[1] = 0.55;
	
    	TPaveText *pt = new TPaveText(x[0], y[0], x[1], y[1], "NDC");
    	pt->SetBorderSize(0);
   	pt->SetFillStyle(0);
    	pt->SetTextAlign(12);
    	pt->SetTextSize(0.04);
   
    	name = Form("E_{#gamma} = %d MeV", eg);
   	pt->AddText(name);
    	pt->Draw();

	//Legend
	TLegend *leg = new TLegend(0.75, 0.4, 0.88, 0.58);
	leg->SetHeader("In Beam", "C");
	leg->AddEntry(h_ARH, "Data", "lep");
	leg->AddEntry(h_mc_scaled, "Simulation" , "l");
	leg->Draw();

	c->Print("plots/MCvsDATA.pdf");	
}

void MCvsData40(UInt_t chan){ //Added by gen
	//attempting to collect the histogram from void CATSEnergy above
	ProjCATSE(chan);
	TH1D* h_ARH = proj_global;

	//Debugging attempt
 	if (!h_ARH) {
        	std::cerr << "Error: Failed to retrive ProjCATSE!" << std::endl;
        return;	
	}
	
	//To denote the incident photon energy as done in ProjCATSE()	
	UInt_t eg =  tdata[chan].egamma;
	TString name;

	//calling in the Monte Carlo simulation 
	TFile *f_mc_pi0 = TFile::Open("ARout/CATS/MC_Pi0.root");
	TFile *f_mc_compton = TFile::Open("ARout/CATS/MC_Compton.root");

	TH1F* h_mc_pi0 = dynamic_cast<TH1F*>(f_mc_pi0->Get("PHYS_EmissP"));
	TH1F* h_mc_compton = dynamic_cast<TH1F*>(f_mc_compton->Get("PHYS_EmissP"));

	if (!h_mc_pi0 || !h_mc_compton) {
        	std::cerr << "Error: MC histograms not found in Pi0 or Compton!" << std::endl;
        	return;
    	}

	//Scaling simulations Compton and Pi0
    	double scale_factor;
    	if (eg <= 200) scale_factor = 60;
    	else if (eg >= 300) scale_factor = 30;
    	else scale_factor = 60 - (eg - 200) * (30.0 / 100.0);

 	TH1F* h_mc_pi0_scaled = (TH1F*)h_mc_pi0->Clone("h_mc_pi0_scaled");
	h_mc_pi0_scaled->Scale(scale_factor);

	TH1F* h_mc_combined = (TH1F*)h_mc_compton->Clone("h_mc_combined");
	h_mc_combined->Add(h_mc_pi0_scaled); // Compton + scaled Pi0		

	if(!h_mc_combined){
		std::cout<<"Error : Failed to combine Monte Carlo Histograms" <<std::endl;
	}
	
	int peakBin =h_ARH->GetMaximumBin();
	double peakCentre = h_ARH->GetBinCenter(peakBin);

	double xmin = peakCentre * 0.9;
	double xmax = peakCentre * 1.1;
	double data_int = h_ARH->Integral( h_ARH->FindBin(xmin),h_ARH->FindBin(xmax));

	double mc_int = h_mc_combined->Integral(h_mc_combined->FindBin(xmin), h_mc_combined->FindBin(xmax));
	
	TH1F* h_mc_combined_scaled = nullptr;
	
	if (mc_int > 0) {
        	h_mc_combined_scaled = (TH1F*)h_mc_combined->Clone("h_mc_combined_scaled");
        	h_mc_combined_scaled->Scale(data_int / mc_int);
        	std::cout << "Scaling combined MC by: " << data_int / mc_int << std::endl;
    	} else {
        	std::cerr << "Error MC integral is zero" << std::endl;
        	return;
    	}
	
//	TH1F* h_mc_combined_scaled = (TH1F*)h_mc_combined->Clone("h_mc_combined_scaled");
//	h_mc_combined_scaled->Scale(data_int / mc_int);
	// Now comparing with the real data (ARH)
    //	int data_bin_min = h_ARH->FindBin(eg - 0.1 * eg);
    //	int data_bin_max = h_ARH->FindBin(eg + 0.1 * eg);

    //	double data_int = h_ARH->Integral(data_bin_min, data_bin_max);

    	//int mc_bin_min = h_mc_combined->FindBin(eg - 0.1 * eg);
    	//int mc_bin_max = h_mc_combined->FindBin(eg + 0.1 * eg);
    //	double mc_int = h_mc_combined->Integral(h_mc_combined->FindBin(xmin), h_mc_combined->FindBin(xmax));
		

//    	TH1F* h_mc_pi0_scaled_final = nullptr;
//    	if (mc_int > 0) {
//        	double scale = data_int / mc_int;
//        	h_mc_pi0_scaled_final = (TH1F*)h_mc_pi0_scaled->Clone("h_mc_pi0_scaled_final");
//        	h_mc_pi0_scaled_final->Scale(scale);
//    	}
	
	//double max_mc = std::max({
    	//h_mc_compton->GetMaximum(),
    	//h_mc_pi0_scaled->GetMaximum(),
    	//h_mc_combined->GetMaximum()
	//});
	//h_mc_compton->SetMaximum(1.2 * max_mc);
	
	TCanvas *c_mc = new TCanvas("c_mc", "MC Components", 800, 600);
	c_mc->Divide(1,3); // should be 3 rows, 1 column 
	
	// Drawing Compton Scattering components 
	c_mc->cd(1);
	h_mc_compton->SetTitle("Compton");
	h_mc_compton->GetXaxis()->SetTitle("Energy (MeV)");
	h_mc_compton->GetYaxis()->SetTitle("Counts");
	h_mc_compton->Draw("HIST");
	
	//Drawing the scaled Pi0 components
	c_mc->cd(2);
	h_mc_pi0_scaled->SetTitle("Pi0 (Scaled)");
	h_mc_pi0_scaled->GetXaxis()->SetTitle("Energy (MeV)");
	h_mc_pi0_scaled->GetYaxis()->SetTitle("Counts");
	h_mc_pi0_scaled->Draw("HIST");
	
	//Drawing the combine Pi0 and Compton Plot
	c_mc->cd(3);
	h_mc_combined->SetTitle("Combined (Compton + Pi0");
	h_mc_combined->GetXaxis()->SetTitle("Energy (MeV)");
	h_mc_combined->GetYaxis()->SetTitle("Counts");
	h_mc_combined->Draw("HIST");

	// Legend
	TLegend *leg_mc = new TLegend(0.65, 0.65, 0.88, 0.88);
	leg_mc->SetHeader("MC Components", "C");
	leg_mc->AddEntry(h_mc_compton, "Compton", "l");
	leg_mc->AddEntry(h_mc_pi0_scaled, "Pi0 (scaled)", "l");
	leg_mc->AddEntry(h_mc_combined, "Combined", "l");
	leg_mc->Draw();

	// Save it
	c_mc->Print("plots/MC_components.pdf");
	
	//Scaling the histogram: 
/*	TH1F* h_mc_combined_scaled = nullptr; 
	if (mc_int > 0){
    		double scale = data_int / mc_int;
    		h_mc_combined_scaled = (TH1F*)h_mc_combined->Clone("h_mc_combined_scaled");
    		h_mc_combined_scaled->Scale(scale);
    		std::cout << "Scaling combined MC by factor: " << scale << std::endl;
		} 
	else {
   	 	std::cerr << "Error: MC integral is zero in selected region!" << std::endl;
		return;
		} */

	TCanvas *c = new TCanvas("c_comapre", "Data vs Simulation", 800, 600);
	
	//Style for ARH
	h_ARH->SetMarkerStyle(20);
	h_ARH->SetMarkerColor(kRed);
	h_ARH->SetLineWidth(0);
	//Style for MC
	if (h_mc_combined_scaled){ 
        	h_mc_combined_scaled->SetMarkerStyle(21);
        	h_mc_combined_scaled->SetMarkerColor(kBlue);
	}
    	double maxY = std::max(h_ARH->GetMaximum(), h_mc_combined_scaled->GetMaximum());
    	h_ARH->SetMaximum(1.1 * maxY);

	//setting axis title, size and location
	h_ARH->GetXaxis()->SetTitle("Energy (MeV)");   
	h_ARH->GetYaxis()->SetTitle("Counts");      
	h_ARH->GetXaxis()->SetTitleSize(0.045);
	h_ARH->GetYaxis()->SetTitleSize(0.045);
	h_ARH->GetXaxis()->SetLabelSize(0.035);
	h_ARH->GetYaxis()->SetLabelSize(0.035);
	
	h_ARH->Draw("E1 P");
	if (h_mc_combined_scaled) h_mc_combined_scaled->Draw("HIST SAME");
	//h_mc->Draw("HIST SAME");
	//h_mc_scaled->Draw("HIST SAME");

	//To denote the incident photon energy as done in ProjCATSE()
	Double_t x[2], y[2];

    	x[0] = 0.15;
    	x[1] = 0.35;
    	y[0] = 0.45;
    	y[1] = 0.55;
	
    	TPaveText *pt = new TPaveText(x[0], y[0], x[1], y[1], "NDC");
    	pt->SetBorderSize(0);
   	pt->SetFillStyle(0);
    	pt->SetTextAlign(12);
    	pt->SetTextSize(0.04);
   
    	name = Form("E_{#gamma} = %d MeV", eg);
   	pt->AddText(name);
    	pt->Draw();

	//Legend
	TLegend *leg = new TLegend(0.75, 0.4, 0.88, 0.58);
	leg->SetHeader("40-degree", "C");
	leg->AddEntry(h_ARH, "Data", "lep");
	leg->AddEntry(h_mc_combined_scaled, "Simulation (Pi0 + Compton)" , "l");
	leg->Draw();


	c->Print("plots/MCvsDATA_Combined.pdf");	
}

