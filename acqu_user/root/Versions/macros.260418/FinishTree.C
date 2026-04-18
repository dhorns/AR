void FinishTree(Char_t* file = NULL)
{
    	TString name;
	printf("\nEnd-of-Run macro executing:\n");

        printf("Closing tree files..."); 
 	TA2Pi0Compton* comp = (TA2Pi0Compton*)(gAN->GetPhysics());
 	comp->CloseTrees();
	printf(" done. \n");

	printf("Saving defined histograms...");
	if( !file) file = "ARHistograms.root";
	TFile f1(file,"RECREATE");
	gROOT->GetList()->Write();
	f1.Close();
  	printf("done.\n",file);
  	printf("All histograms saved to %s\n",file);
}
