// Stuff to do at the end of an analysis run.

void FinishBasic( TString name)
{
	TString path;

	cout << "End-of-Run macro executing";
	cout << endl;

	path.Append( name);
	path.Append( ".root");
	TFile f( path, "recreate");
	gROOT->GetList()->Write();
	f.Close();

	path.Prepend( "All histograms saved to \"");
	path.Append( "\"");
	cout << path;
	cout << endl;
}
