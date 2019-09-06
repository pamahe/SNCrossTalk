#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

#include <TFile.h>
#include <TH1D.h>

void sumPlot() {
  
  std::vector<std::string> ct_list = {"1", "10", "100", "1e3", "1e4", "1e5", "1e6"};
  
  for (auto ct : ct_list) {
    
    std::string output_histotitle = "sum_prob_" + ct;
    std::string output_histoname  = "Sum over all OMS of coincidences probabilities around OM of reference, normalized, for run 104 with a " + ct + " time window.";
    TH1D *output_histo = new TH1D(output_histotitle.c_str(), output_histoname.c_str(), 13, 0.0, 13.0);
  
    std::string input_filename = "/sps/nemo/scratch/pamahe/SNCrossTalk/2Dto1D/results/CT_1D_run-104_HT_" + ct + ".root";
    TFile *input_file = new TFile(input_filename.c_str(),"READ");

    for (int ref_slot = 2; ref_slot < 18; ref_slot++) {
      for (int ref_channel = 2; ref_channel < 11; ref_channel++) {
	

	std::string input_histotitle = "ctp_" + std::to_string(ref_slot) + "." + std::to_string(ref_channel);
	TH1D *input_histo = (TH1D *)input_file->Get(input_histotitle.c_str()); 
	
	for (int ibin = 0; ibin < 13; ibin++) {
	  output_histo->AddBinContent(ibin + 1, (input_histo->GetBinContent(ibin + 1)/132));
	  std::cout << input_histo->GetBinContent(ibin + 1) << "\n";
	}
      }
    }

    input_file->Close();
    std::string output_filename = "SumProb_run-104_all.root";
    TFile *output_file = new TFile(output_filename.c_str(),"UPDATE");
	
    output_histo->Write();
    output_file->Close();
  }
}
	  
