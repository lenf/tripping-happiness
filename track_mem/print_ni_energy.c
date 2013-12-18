void print_ni_energy(){
	TFile* f = TFile :: Open("./deep_energy.root");
	
	ntuple = (TNtuple*) f->Get("Deep");
		
	TH1F *h2a = new TH1F("Ni_edep","Ni_edep", 50, 0, 25);
	
	for (int i = 0; i < ntuple->GetEntries(); i++){
		ntuple->GetEntry(i);
        content = ntuple->GetArgs();
		h2a->Fill(content[0], content[1]);
	}
	
	h2a->Draw();
}	

