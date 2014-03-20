void print_ni(){
	TFile* f = TFile :: Open("./energy_dep.root");
	
	tuple = (TNtuple*) f->Get("Info");
	tuple -> GetEntry(0);
	info = tuple->GetArgs();
	int num = info[0];
	float minz = 0;
	float maxz = info[3];
	
	float maxr = 0.0, minr = 0.0, maxe = 0.0;
	ntuple = (TNtuple*) f->Get("Ni");
	
	for (int i = 0; i < ntuple->GetEntries(); i++){
		ntuple->GetEntry(i);
        con = ntuple->GetArgs();
        if (con[2] > maxe) maxe = con[2];
        if (con[1] > maxr) maxr = con[1];
        if (con[1] < minr) minr = con[1];
	}
	const int bin = 50;
	TH2F *h3a = new TH2F("Ni_edep","Ni_edep", 100, minz, maxz,100, 0 , maxe);
	int data[bin];
	for (int j = 0; j<bin; j++){
		data[j] = 0;
	}
	int index;
	for (int i = 0; i < ntuple->GetEntries(); i++){
		ntuple->GetEntry(i);
        content = ntuple->GetArgs();
		index = int(content[0] /maxz * bin);
		if (index >= bin){

		index = bin-1;
		}

		data[index] ++; 
	}
	for (int i = 0; i < ntuple->GetEntries(); i++){
		ntuple->GetEntry(i);
        content = ntuple->GetArgs();
        index = int(content[0] /maxz * bin);
		if (index >= bin)
		index = bin-1;
		h3a->Fill(content[0], content[2]);
	}
	h3a->Draw("lego2");
}	

