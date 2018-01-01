//====================================================================================================
//
// 2017.11.14	Li Yi
// Modified from main08.cc in pythia8215 rootexamples
// generate hard pt bin option
// to compare the previous hard pt bias option
//
//====================================================================================================
//
// main08.cc is a part of the PYTHIA event generator.
// Copyright (C) 2016 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// This is a simple test program.
// It illustrates methods to emphasize generation at high pT.

// Stdlib header file for input and output.
#include <iostream>

// Stdlib header file for input and output to files
#include <fstream>

// Header file to access Pythia 8 program elements.
#include "Pythia8/Pythia.h"

// ROOT, for tree.
#include "TTree.h"

// ROOT, for saving file.
#include "TFile.h"


using namespace Pythia8;

int main(int argc, char* argv[]) {

	string seed;
	if(argc>=2) {
		seed = string(argv[1]);
	}
	else { 
		seed = "171114";
	}

	// Number of events to generate per bin.
	int nEvent = 1e6;
	if(argc>=3)  {
		nEvent = atoi(argv[2]);
	}

	// Optionally minimize output (almost) to final results.
	bool smallOutput = true;


	// Generator.
	Pythia pythia;

	// Shorthand for some public members of pythia (also static ones).
	Settings& settings = pythia.settings;
	Info& info = pythia.info;

	// Optionally limit output to minimal one.
	if (smallOutput) {
		pythia.readString("Init:showProcesses = off");
		pythia.readString("Init:showMultipartonInteractions = off");
		pythia.readString("Init:showChangedSettings = off");
		pythia.readString("Init:showChangedParticleData = off");
		pythia.readString("Next:numberCount = 1000000000");
		pythia.readString("Next:numberShowInfo = 0");
		pythia.readString("Next:numberShowProcess = 0");
		pythia.readString("Next:numberShowEvent = 0");
	}

	// Number of bins to use. 
	const int nBin = 12;

	// set up pT bins for range [0, 100]. The lowest bin
	// is generated with soft processes, to regularize pT -> 0 blowup.
	// Warning: if pTlimitLow[1] is picked too low there will be a
	// visible discontinuity, since soft processes are generated with
	// dampening and "Sudakov" for pT -> 0, while hard processes are not.
	double pTlimitLow[nBin+1] = {0., 2., 3., 5., 7., 9., 11., 15., 20., 25., 35., 45., 60.}; 
	//0., 20., 40., 70., 100.};


	std::ofstream fout;
	fout.open(Form("/home/fas/caines/ly247/Scratch/pythiadata/Xsec4pythia8215_pp200hard_PionDecayOff_seed%s.csv",seed.data()));
	if(!fout.is_open()) {std::cout<<"ERR!!!  Cannot open fout for writing!!"<<std::endl;}
	fout << "#pT hat:min,max\tsigma\tEvents"<<std::endl;

	// Loop over number of bins, i.e. number of subruns.
	for (int iBin = 0; iBin < nBin; ++iBin) {

		// Normally HardQCD, but first bin nonDiffractive.
		// Need MPI on in nonDiffractive to get first interaction, but not else.
		if (iBin == 0) {
			pythia.readString("HardQCD:all = off");
			pythia.readString("SoftQCD:nonDiffractive = on");
		} else {
			pythia.readString("HardQCD:all = on");
			pythia.readString("SoftQCD:nonDiffractive = off");
		}

		// hardcoded here. Use settings.parm for non-string input.
		settings.parm("PhaseSpace:pTHatMin", pTlimitLow[iBin]);
		settings.parm("PhaseSpace:pTHatMax", pTlimitLow[iBin + 1]);

		// Off weak decay
		pythia.readFile("Config_Pythia8Pion0Off.cmnd");

		pythia.readString("Random:setSeed = on"); //wheter to set seed
		pythia.readString("Random:seed = "+seed); //initial seed!

		// Initialize for RHIC at 200 GeV.
		pythia.readString("Beams:eCM = 200.");
		pythia.init();

		// Create file on which histogram(s) can be saved.
		TFile* outFile = new TFile(Form("~/Scratch/pythiadata/pythia8215_pp200hard_pT%d-%d_PionDecayOff_seed%s.root",int(pTlimitLow[iBin]),int(pTlimitLow[iBin+1]),seed.data()), "RECREATE");

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

		// Begin event loop.
		for (int iEvent = 0; iEvent < nEvent; ++iEvent) {

			// Generate events. Skip if failure.
			if (!pythia.next()) continue;

			// Soft events have no upper pT limit. They therefore overlap
			// with hard events, and the overlap must be removed by hand.
			// No overlap for elastic/diffraction, which is only part of soft.
			double pTHat  = info.pTHat();
			if (iBin == 0 && info.isNonDiffractive()
					&& pTHat > pTlimitLow[1]) continue;

			// Fill hard scale of event.
			wt = info.weight();

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

			// End of event loop. Statistics.
		}
		pythia.stat();

		// Print out cross section and weight total for each iBin
		std::cout<<std::setprecision(2)<<"pT hat: "<<pTlimitLow[iBin]<<" - "<<pTlimitLow[iBin+1]<<"\tsigma = "<<std::setprecision(5)<<info.sigmaGen()<<"\tEvents = "<<info.weightSum()<<std::endl;
		//fout<<"pT hat: "<<pTlimitLow[iBin]<<" - "<<pTlimitLow[iBin+1]<<"\tsigma = "<<info.sigmaGen()<<"\tEvents = "<<info.weightSum()<<std::endl;
		fout<<std::setprecision(2)<<pTlimitLow[iBin]<<","<<pTlimitLow[iBin+1]<<","<<std::setprecision(5)<<info.sigmaGen()<<","<<info.weightSum()<<std::endl;

		// Save histogram on file and close file.
		tree->Write();
		delete outFile;
		//tree->Delete();
		//outfile->Delete();
		// End of pT-bin loop.
	}

	// close txt files
	fout.close();

	// Done.
	return 0;
}
