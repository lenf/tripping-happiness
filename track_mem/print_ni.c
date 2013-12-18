void print_ni(){
	TFile* f = TFile :: Open("./energy_dep.root");
	
	tuple = (TNtuple*) f->Get("Info");
	tuple -> GetEntry(0);
	info = tuple->GetArgs();
	int num = info[0];
	float minz = 0;
	float maxz = info[3];
	
	float maxr = 0.0, minr = 0.0;
	ntuple = (TNtuple*) f->Get("Ni");
	
	for (int i = 0; i < ntuple->GetEntries(); i++){
		ntuple->GetEntry(i);
        con = ntuple->GetArgs();
        if (con[1] > maxr) maxr = con[1];
        if (con[1] < minr) minr = con[1];
	}
	
	TH2F *h3a = new TH2F("Ni_edep","Ni_edep", 50, minz, maxz, 50, minr, maxr);
	
	for (int i = 0; i < ntuple->GetEntries(); i++){
		ntuple->GetEntry(i);
        content = ntuple->GetArgs();
		h3a->Fill(content[0], content[1], content[2]/num);
	}
	h3a->Draw("lego2");
}	

