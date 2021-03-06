#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

/// Used to plot the charge distribution from CRD files and reproduce the bug found in RHD files
/// where the charge is stored in a short type and overflows


void cut_charge() {

  TFile *input_file = new TFile("/sps/nemo/scratch/pamahe/SNCrossTalk/SortTree/results/full/run-104_HTs_full.root","READ");
  TTree *input_tree = (TTree *)input_file->Get("Calo_Hits");

  TFile *output_file = new TFile("/sps/nemo/scratch/pamahe/SNCrossTalk/cut_charge/cut_charge.root","RECREATE");

  // First 2 histograms output the artificially bugged charge distribution
  // Third histogram output the real charge distribution
  TH1D *neg_charge_cut = new TH1D("negCD","Charge distribution negative", 400, -40000, 0);
  TH1D *all_charge_cut = new TH1D("allCD","Charge distribution all range", 800, -40000, 40000);
  TH1D *true_charge = new TH1D("trueCD","Charge distribution all range", 800, -60000, 10000);

  Int_t hit_number;
  Double_t tdc_ns;
  Int_t raw_charge;
  Short_t raw_peak;
  input_tree->SetBranchAddress("hit_number",&hit_number);
  input_tree->SetBranchAddress("tdc_ns",&tdc_ns);
  input_tree->SetBranchAddress("raw_charge",&raw_charge);
  input_tree->SetBranchAddress("raw_peak",&raw_peak);

  int entries = input_tree->GetEntries();

  for (int ientries = 0; ientries < entries; ientries++) {

    if (ientries % 100000 == 0) {
      std::cout << ientries << "/" << entries << "\n";
    }

    input_tree->GetEntry(ientries);
    
    
    Short_t charge = raw_charge; // Recreate the overflow bug
    
    neg_charge_cut->Fill(charge);
    all_charge_cut->Fill(charge);
    true_charge->Fill(raw_charge);

  }

  neg_charge_cut->Write();
  all_charge_cut->Write();
  true_charge->Write();
}
