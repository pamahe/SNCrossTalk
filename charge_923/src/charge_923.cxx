#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>

// Compare behavior of OMs 9.2 and 9.3 of the French Wall to other OMs of that wall (investigate event with charge close to 0
void charge_923() {

  std::vector<int> run_list = {100, 101, 102, 104, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 119, 120, 121, 127, 128, 129, 130, 131, 132, 133, 134, 135};

  // Automated work on different runs, the problem was not due to settings
  for (auto run : run_list) {
 
    std::cout << "\n Current run : " << run << "\n\n";

    std::string input_filename = "/sps/nemo/scratch/pamahe/SNCrossTalk/SortTree/results/full/run-" + std::to_string(run) + "_HTs_full.root";
    
    TFile *input_file = new TFile(input_filename.c_str(),"READ");
    TTree *input_tree = (TTree *)input_file->Get("Calo_Hits");
    
    UInt_t slot_id, channel_id;
    Int_t raw_charge;
    Short_t raw_peak;
    input_tree->SetBranchAddress("slot_id",&slot_id);
    input_tree->SetBranchAddress("channel_id",&channel_id);
    input_tree->SetBranchAddress("raw_charge",&raw_charge);
    input_tree->SetBranchAddress("raw_peak",&raw_peak);

    int entries = input_tree->GetEntries();

    std::string output_filename  = "/sps/nemo/scratch/pamahe/SNCrossTalk/charge_923/results/CD_run-" + std::to_string(run) + ".root";

    // Create 4 TH1D to compare charge and amplitude of 9.2 and 9.3 to others OMs of the French wall
    TFile *output_file = new TFile(output_filename.c_str(),"RECREATE");
    TH1D *charge_distrib_all = new TH1D("CD_all","Charge distribution over all OM (except 9.2 and 9.3)", 2000, -50000.0, 50000.0);
    TH1D *charge_distrib_923 = new TH1D("CD_923","Charge distribution for OM 9.2 and 9.3", 2000, -50000.0, 50000.0);

    TH1D *ampl_distrib_all = new TH1D("AD_all","Ampl distribution over all OM (except 9.2 and 9.3)", 200, -15000.0, 15000.0);
    TH1D *ampl_distrib_923 = new TH1D("AD_923","Ampl distribution for OM 9.2 and 9.3", 200, -15000.0, 15000.0);

    
    int entries_all = 0;
    int entries_923 = 0;
    
    for (int ientries = 0; ientries < entries; ientries++) {
      
      if (ientries % 1000000 == 0) {
	std::cout << ientries << "/" << entries << "\n";
      }
      
      input_tree->GetEntry(ientries);

      // Fill the histograms
      if (slot_id == 9 and (channel_id == 2 or channel_id == 3)) {
	
	charge_distrib_923->Fill(raw_charge);
	ampl_distrib_923->Fill(raw_peak);
	entries_923++;
	
      } else {
	
	charge_distrib_all->Fill(raw_charge);
	ampl_distrib_all->Fill(raw_peak);
	entries_all++;
      }
      
    }

    // Normalization factors
    Double_t factor_all, factor_923;
    factor_all = 1 / double(entries_all);
    factor_923 = 1 / double(entries_923);

    
    charge_distrib_all->Scale(factor_all);
    if (factor_923 == factor_923) { // Avoid NAN
      charge_distrib_923->Scale(factor_923);
    }
    ampl_distrib_all->Scale(factor_all);
    if (factor_923 == factor_923) {
      ampl_distrib_923->Scale(factor_923);
    }
    charge_distrib_all->Write();
    charge_distrib_923->Write();

    ampl_distrib_all->Write();
    ampl_distrib_923->Write();

    input_file->Close();
    output_file->Close();
  }
}
    
