//____________________________________________________________________
//
// Using Pythia6 with ROOT
// To make an event sample (of size 100) do
//
//    shell> root
//    root [0] .L GenPythia6.C
//    root [1] makeEventSample(1000)
//
// To start the tree view on the generated tree, do
//
//    shell> root
//    root [0] .L GenPythia6.C
//    root [1] showEventSample()
//
//
// The following session:
//    shell> root
//    root [0] .x GenPythia6.C(500)
// will execute makeEventSample(500) and showEventSample()
//
// Alternatively, you can compile this to a program
// and then generate 1000 events with
//
//    ./GenPythia6 1000
//
// To use the program to start the viewer, do
//
//    ./GenPythia6 -1
//
// NOTE 1: To run this example, you must have a version of ROOT
// compiled with the Pythia6 version enabled and have Pythia6 installed.
// The statement gSystem->Load("$HOME/pythia6/libPythia6");  (see below)
// assumes that the directory containing the Pythia6 library
// is in the pythia6 subdirectory of your $HOME.  Locations
// that can specify this, are:
//
//  Root.DynamicPath resource in your ROOT configuration file
//    (/etc/root/system.rootrc or ~/.rootrc).
//  Runtime load paths set on the executable (Using GNU ld,
//    specified with flag `-rpath').
//  Dynamic loader search path as specified in the loaders
//    configuration file (On GNU/Linux this file is
//    etc/ld.so.conf).
//  For Un*x: Any directory mentioned in LD_LIBRARY_PATH
//  For Windows: Any directory mentioned in PATH
//
// NOTE 2: The example can also be run with ACLIC:
//  root > gSystem->Load("libEG");
//  root > gSystem->Load("$ROOTSYS/../pythia6/libPythia6"); //change to your setup
//  root > gSystem->Load("libEGPythia6");
//  root > .x GenPythia6.C+
//
//
//____________________________________________________________________
//
// Author: Christian Holm Christensen <cholm@hilux15.nbi.dk>
// Update: 2002-08-16 16:40:27+0200
// Copyright: 2002 (C) Christian Holm Christensen
// Copyright (C) 2006, Rene Brun and Fons Rademakers.
// For the licensing terms see $ROOTSYS/LICENSE.
//
#ifndef __CINT__
#include "TApplication.h"
#include "TPythia6.h"
#include "TFile.h"
#include "TError.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TF1.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TCanvas.h"
#include "Riostream.h"
#include <cstdlib>
using namespace std;
#endif

#define FILENAME   "/gpfs/scratch60/fas/caines/ly247/pythia200/pythia_Perugia2012_nopiondecay.root"
#define TREENAME   "tree"
#define BRANCHNAME "particles"
#define HISTNAME   "ptSpectra"
#define PDGNUMBER  211

// This function just load the needed libraries if we're executing from
// an interactive session.
void loadLibraries()
{
#ifdef __CINT__
  // Load the Event Generator abstraction library, Pythia 6
  // library, and the Pythia 6 interface library.
  gSystem->Load("libEG");
  gSystem->Load("$PYTHIA6/libPythia6"); //change to your setup
  gSystem->Load("libEGPythia6");
  gSystem->Load("libLHAPDF.so");
#endif
}

// nEvents is how many events we want.
int makeEventSample(Int_t nEvents)
{
  // Load needed libraries
  loadLibraries();

  // Create an instance of the Pythia event generator ...
  TPythia6* pythia = new TPythia6;

  pythia->SetMSTP(5,370);
  // ... and initialise it to run p+p at sqrt(200) GeV in CMS
  pythia->SetMDCY(pythia->Pycomp(111),1,0);		// don't decay pion0
  pythia->Initialize("cms", "p", "p", 200);

  //pythia->SetMSTP(5,0);		// From Zilong: when MSTP(5) is zero, the Initialize doesn't change anything. Email commu. on Sat, May 14, 2016 at 5:43 PM
  //pythia->SetPARP(90,0.213);
  //pythia->SetMDCY(pythia->Pycomp(111),1,0);		// don't decay pion0
  //pythia->Initialize("cms", "p", "p", 200);

  // Open an output file
  TFile* file = TFile::Open(FILENAME, "RECREATE");
  if (!file || !file->IsOpen()) {
    Error("makeEventSample", "Couldn;t open file %s", FILENAME);
    return 1;
  }

  // Make a tree in that file ...
  TTree* tree = new TTree(TREENAME, "Pythia 6 tree");

  // ... and register a the cache of pythia on a branch (It's a
  // TClonesArray of TMCParticle objects. )
  TClonesArray* particles = (TClonesArray*)pythia->GetListOfParticles();
  tree->Branch(BRANCHNAME, &particles);

  // Now we make some events
  for (Int_t i = 0; i < nEvents; i++) {
    if (i % 10000 == 0)
      cout << "Event # " << i << endl;

    // Make one event.
    pythia->GenerateEvent();

    // Maybe you want to have another branch with global event
    // information.  In that case, you should process that here.
    // You can also filter out particles here if you want.

    // Now we're ready to fill the tree, and the event is over.
    tree->Fill();
  }

  // Show tree structure
  tree->Print();
/*
  // After the run is over, we may want to do some summary plots:
  TH1D* hist = new TH1D(HISTNAME, "p_{#perp}  spectrum for  #pi^{+}",
                        100, 0, 3);
  hist->SetXTitle("p_{#perp}");
  hist->SetYTitle("dN/dp_{#perp}");
  char expression[64];
  sprintf(expression,"sqrt(pow(%s.fPx,2)+pow(%s.fPy,2))>>%s",
          BRANCHNAME, BRANCHNAME, HISTNAME);
  char selection[64];
  sprintf(selection,"%s.fKF==%d", BRANCHNAME, PDGNUMBER);
  tree->Draw(expression,selection);

  // Normalise to the number of events, and the bin sizes.
  hist->Sumw2();
  hist->Scale(3 / 100. / hist->Integral());
  hist->Fit("expo", "QO+", "", .25, 1.75);
  TF1* func = hist->GetFunction("expo");
  func->SetParNames("A", "- 1 / T");
*/
  // and now we flush and close the file
  file->Write();
  file->Close();

  return 0;
}

// Show the Pt spectra, and start the tree viewer.
int showEventSample()
{
  // Load needed libraries
  loadLibraries();

  // Open the file
  TFile* file = TFile::Open(FILENAME, "READ");
  if (!file || !file->IsOpen()) {
    Error("showEventSample", "Couldn;t open file %s", FILENAME);
    return 1;
  }

  // Get the tree
  TTree* tree = (TTree*)file->Get(TREENAME);
  if (!tree) {
    Error("showEventSample", "couldn't get TTree %s", TREENAME);
    return 2;
  }

  // Start the viewer.
  tree->StartViewer();

  // Get the histogram
  TH1D* hist = (TH1D*)file->Get(HISTNAME);
  if (!hist) {
    Error("showEventSample", "couldn't get TH1D %s", HISTNAME);
    return 4;
  }

  // Draw the histogram in a canvas
  gStyle->SetOptStat(1);
  TCanvas* canvas = new TCanvas("canvas", "canvas");
  canvas->SetLogy();
  hist->Draw("e1");
  TF1* func = hist->GetFunction("expo");

  char expression[64];
  sprintf(expression,"T #approx %5.1f", -1000 / func->GetParameter(1));
  TLatex* latex = new TLatex(1.5, 1e-4, expression);
  latex->SetTextSize(.1);
  latex->SetTextColor(4);
  latex->Draw();

  return 0;
}

void GenPythia6(Int_t n=1000000) {
   makeEventSample(n);
   //showEventSample();
}

#ifndef __CINT__
int main(int argc, char** argv)
{
  TApplication app("app", &argc, argv);

  Int_t n = 1000000;
  if (argc > 1)
    n = strtol(argv[1], NULL, 0);

  int retVal = 0;
  if (n > 0)
    retVal = makeEventSample(n);
  else {
    retVal = showEventSample();
    app.Run();
  }

  return retVal;
}
#endif

//____________________________________________________________________
//
// EOF
//

