#include <iostream>
using namespace std;
#include "TFile.h"
#include "TSystem.h"
#include "TNtuple.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"

void print_physics(){
	TFile* f = TFile :: Open("./MolerBhabha.root");
	ntuple = (TNtuple*) f->Get("Second_electron_energy");
	float mine = 0;
	float maxe = 100;
	int n = 100;
	int j;
	TH1F *h1a = new TH1F("Moler", "Bhabha", n, mine, maxe);
	for (j = 0; j < ntuple->GetEntries(); j++){
		ntuple->GetEntry(j);
        content = ntuple->GetArgs();
        h1a->Fill(content[0]);
	}
	
	TFile* f1 = TFile :: Open("./Livermore.root");
	ntuple1 = (TNtuple*) f1->Get("Second_electron_energy");
	TH1F *h1b = new TH1F("Livermore", "Livermore", n, mine, maxe);
	for (j = 0; j < ntuple1->GetEntries(); j++){
		ntuple1->GetEntry(j);
        content = ntuple1->GetArgs();
        h1b->Fill(content[0]);
	}
	
	//TCanvas *c1 = new TCanvas("Moler","Bhabha");
	//TCanvas *c2 = new TCanvas("Livermore","Livermore");
	//c1->cd();
	h1a->Draw();
	//c2->cd();
	h1b->SetLineColor(kRed);
	h1b->Draw("same");
}
