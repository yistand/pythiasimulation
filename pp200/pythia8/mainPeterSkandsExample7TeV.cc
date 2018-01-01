//====================================================================================================
//
// 2017.11.20	Li YI
// try to reproduce Peter Skands's plot at http://mcplots.cern.ch/?query=plots,ppppbar,uemb-hard,nch-vs-pt-trns,Pythia%206.Pythia%206%20vs%208
// third plot for CMS results.
//
//====================================================================================================
//
// 2017.04.20	Li Yi
// Modified from main91.cc in pythia8215 rootexamples
//
//====================================================================================================
//
// main91.cc is a part of the PYTHIA event generator.
// Copyright (C) 2016 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// This is a simple test program.
// It studies the charged multiplicity distribution at the LHC.
// Modified by Rene Brun, Axel Naumann and Bernhard Meirose
// to use ROOT for histogramming.

// Stdlib header file for input and output.
#include <iostream>

// Header file to access Pythia 8 program elements.
#include "Pythia8/Pythia.h"

// ROOT, for histogramming.
#include "TH1.h"

// ROOT, for tree.
#include "TTree.h"

//// ROOT, for interactive graphics.
//#include "TVirtualPad.h"
//#include "TApplication.h"

// ROOT, for saving file.
#include "TFile.h"

using namespace Pythia8;

int main(int argc, char* argv[]) {

	//// Create the ROOT application environment.
	//TApplication theApp("hist", &argc, argv);

	// Create Pythia instance and set it up to generate hard QCD processes
	// above pTHat = 20 GeV for pp collisions at 14 TeV.
	Pythia pythia;
	pythia.readFile("Config_PeterSkandsExample7TeV.cmnd");

	pythia.init();

	// Create file on which histogram(s) can be saved.
	TFile* outFile = new TFile(Form("~/Scratch/pythiadata/pythia8215_pp7TeVMB.root"), "RECREATE");

	//// Book histogram.
	//TH1F *mult = new TH1F("mult","charged multiplicity", 100, -0.5, 799.5);
	//TH1F *hW = new TH1F("hW","weight", 1000, 0, 0.001);

	// Initiate tree
	int eventid;
	double wt;
	int npart;
	const int MaxArray = 1000;
	float px[MaxArray], py[MaxArray], pz[MaxArray], energy[MaxArray]; 
	int charge[MaxArray];
	int id[MaxArray], status[MaxArray];
	TTree *tree = new TTree("tree","tree");
	tree->Branch("eventid",&eventid);
	tree->Branch("eventweight",&wt);
	tree->Branch("npart",&npart);
	tree->Branch("id",id,"id[npart]/I");
	tree->Branch("status",status,"status[npart]/I");
	tree->Branch("charge",charge,"charge[npart]/I");
	tree->Branch("px",px,"px[npart]/F");
	tree->Branch("py",py,"py[npart]/F");
	tree->Branch("pz",pz,"pz[npart]/F");
	tree->Branch("energy",energy,"energy[npart]/F");

	// Begin event loop. Generate event; skip if generation aborted.
	int nEvents = pythia.mode("Main:numberOfEvents");
	for (int iEvent = 0; iEvent < nEvents; ++iEvent) {
		if (!pythia.next()) continue;

		wt = pythia.info.weight();
		eventid = iEvent;

		npart = 0;
		// Find number of all final particles.
		for (int i = 0; i < pythia.event.size(); ++i) {
			//if (pythia.event[i].status()==22) cout<<"w = "<<wt<<" pt = "<<pythia.event[i].pT()<<endl;
			if (pythia.event[i].isFinal() ) // && pythia.event[i].isCharged())
			{
				id[npart] = pythia.event[i].id();
				status[npart] = pythia.event[i].status();
				charge[npart] = static_cast<int>(pythia.event[i].charge());

				px[npart] = static_cast<float>(pythia.event[i].px());
				py[npart] = static_cast<float>(pythia.event[i].py());
				pz[npart] = static_cast<float>(pythia.event[i].pz());
				energy[npart] = static_cast<float>(pythia.event[i].e());

				npart++;
			}
		}

		tree->Fill();
	}

	// Statistics on event generation.
	pythia.stat();

	//// Show histogram. Possibility to close it.
	//mult->Draw();
	//std::cout << "\nDouble click on the histogram window to quit.\n";
	//gPad->WaitPrimitive();

	// Save histogram on file and close file.
	tree->Write();
	delete outFile;

	// Done.
	return 0;
}
