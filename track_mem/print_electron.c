void print_electron(){
	TFile* f = TFile :: Open("./energy_dep.root");
	
	const int N0 = 5;
	const float radius = 25;
	
	tuple = (TNtuple*) f->Get("Info");
	tuple -> GetEntry(0);
	info = tuple->GetArgs();
	int num = info[0];
	float density = info[1];
	float molar = info[2];
	float minz = 0;
	float maxz = info[3];
	float minr = 0, maxr = 150;
	ntuple = (TNtuple*) f->Get("Electron");
		
	const int binz = 100, binr = 150;
	float dz = (maxz - minz) / binz;
	float dr = (maxr - minr) / binr;
	int data[binz][binr];
	
	int i,j,t;
	
	for (i=0; i<binz; i++)
	for (j=0; j<binr; j++)
		data[i][j] = 0;
	TH2F *h3b = new TH2F("Electron amount","Ionisation", binz, minz, maxz, binr, minr, maxr);
	TH1F *h2a = new TH1F("section", "Average section", binr, minr, maxr);
	TH1F *h2b = new TH1F("track", "Track's profile", binz, minz, maxz);
		
	int sum = 0;
	for (j = 0; j < ntuple->GetEntries(); j++){
		ntuple->GetEntry(j);
        content = ntuple->GetArgs();
		h3b->Fill(content[0], content[1], 1.0/num);
		if (content[1] < radius)
			sum ++;
		h2a->Fill(content[1], 1.0/num/(maxz-minz));
		if ((content[0] >= 0) && (content[1] <= maxr)){
			i = int (content[0] / dz);
			t = int (content[1] / dr);
			if (i * dz > content[0])
				i--;
			if (t * dr > content[1])
				t--;
			if (i>=binz)
				i = binz-1;
			if (t>=binr)
				t = binr-1;
			data[i][t] ++; 
		}
	}
	int rmax = 0;
	for (i=0; i<binz; i++){
		for (j=0; j<binr; j++){
			if (data[i][j] >= N0){
				//h2b->Fill((i+0.5)*dz, dr);
				rmax = j;
			};
		}
		h2b->Fill((i+0.5)*dz, rmax * dr);
		rmax = 0;
	}
    sum /= num;
    cout << "Amount of poly " << (radius*radius*10e-18*(maxz-minz)*10e-6*3.14)*density/molar*6.02*10e23 << " Amount of e- " << sum << "    " << (maxz-binz*dz)/dz << endl;
	
	TH2F *h3c = new TH2F("Electron end","Ionisation end", binz, minz, maxz, binr, minr, maxr);
	ntuple = (TNtuple*) f->Get("ElectronEnd");
	
	for (j = 0; j < ntuple->GetEntries(); j++){
		ntuple->GetEntry(j);
        content = ntuple->GetArgs();
		h3c->Fill(content[0], content[1], 1.0/num);
	}
		
	TCanvas *c2 = new TCanvas("Ionisation","Ionisation");
	TCanvas *c3 = new TCanvas("e- section", "Average section");
	TCanvas *c4 = new TCanvas("Track's profile", "Track's profile");
	TCanvas *c5 = new TCanvas("EndIonisation","Ionisation's end");
	c2->cd();
    h3b->Draw("lego2");
    c3->cd();
	h2a->Draw();
	c4->cd();
	h2b->Draw();
	c5->cd();
	h3c->Draw("lego2");
}
