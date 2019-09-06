
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

#include <TFile.h>
#include <TH1D.h>

void charge_ratio() {

  std::vector<std::string> run_list = {"100"};
  std::vector<std::string> ct_list = {"1", "10", "100","1e6", "1e8"};

  for (auto run : run_list) {
    for (auto ct : ct_list) {
      
      std::string input_filename = "/sps/nemo/scratch/pamahe/SNCrossTalk/Coincidence/results_NS/run_" + run +"/Coinc_run-" + run + "_" + ct + ".root";
      TFile *input_file = new TFile(input_filename.c_str(),"READ");

      std::string output_filename = "/sps/nemo/scratch/pamahe/SNCrossTalk/Coincidence/charge_ratio_run-" + run + "_HT_" + ct + ".root";
      TFile *output_file = new TFile(output_filename.c_str(),"RECREATE");

      TH1D *output_histo_saml = new TH1D("charge_ratio_saml","Charge ratio SAMLONG OM over Reference OM", 5000, 0.0, 10.0);
      TH1D *output_histo_neigh = new TH1D("charge_ratio_neigh","Charge ratio neighbour OM over Reference OM", 5000, 0.0, 10.0);
      TH1D *output_histo_other = new TH1D("charge_ratio_other","Charge ratio other OM over Reference OM", 5000, 0.0, 10.0);
      
      for (int ref_slot = 0; ref_slot < 20; ref_slot++) {
	for (int ref_channel = 0; ref_channel < 12; ref_channel++) {

	  for (int ibin = 0; ibin < 5000; ibin++) {
	    output_histo_saml->SetBinContent(ibin + 1, 0.0);
	    output_histo_neigh->SetBinContent(ibin + 1, 0.0);
	    output_histo_other->SetBinContent(ibin + 1, 0.0);
	  }

	  int slot, channel;
	  if (ref_channel % 2 == 0) {
	    channel = ref_channel + 1;
	    slot = ref_slot;
	  } else {
	    channel = ref_channel - 1;
	    slot = ref_slot;
	  }
	  
	  std::string ref_cube = std::to_string(ref_slot) + "." + std::to_string(ref_channel);
	  std::string saml_cube = std::to_string(slot) + "." + std::to_string(channel);
	  std::string neigh_cube = std::to_string(slot - 1) + "." + std::to_string(ref_channel);
	  
	  std::string input_histo_saml_name = "charge_ratio_" + ref_cube + "_" + saml_cube;
	  TH1D *input_histo_saml = (TH1D *)input_file->Get(input_histo_saml_name.c_str());
	  
	  std::string input_histo_neigh_name = "charge_ratio_" + ref_cube + "_" + neigh_cube;
	  TH1D *input_histo_neigh = (TH1D *)input_file->Get(input_histo_neigh_name.c_str());
	  
	  std::string input_histo_other_name = "charge_ratio_other" + ref_cube;
	  TH1D *input_histo_other = (TH1D *)input_file->Get(input_histo_other_name.c_str());
	    
	  for (int ibin = 0; ibin < 5000; ibin++) {
	    
	    if (TMath::IsNaN((double)output_histo_saml->GetBinContent(ibin + 1))) {
	      //output_histo_saml->AddBinContent(ibin + 1, 0.0);
	    } else {
	      output_histo_saml->AddBinContent(ibin + 1, input_histo_saml->GetBinContent(ibin + 1));
	    }
	    
	    if (TMath::IsNaN((double)output_histo_neigh->GetBinContent(ibin + 1))) {
	      //output_histo_neigh->AddBinContent(ibin + 1, 0.0);
	    } else {
	      output_histo_neigh->AddBinContent(ibin + 1, input_histo_neigh->GetBinContent(ibin + 1));
	    }

	    if (TMath::IsNaN((double)output_histo_other->GetBinContent(ibin + 1))) {
	      //output_histo_other->AddBinContent(ibin + 1, 0.0);
	    } else {
	      output_histo_other->AddBinContent(ibin + 1, input_histo_other->GetBinContent(ibin + 1));
	    }
	  }	  
	}
      }
	
      
      input_file->Close();
      
      output_histo_saml->Scale(1 / double(output_histo_saml->GetEntries()));
      output_histo_neigh->Scale(1 / double(output_histo_saml->GetEntries()));
      output_histo_other->Scale(1 / double(output_histo_saml->GetEntries()));
      
      output_histo_saml->Rebin(10);
      output_histo_neigh->Rebin(10);
      output_histo_other->Rebin(10);
      
      output_histo_saml->Write();
      output_histo_neigh->Write();
      output_histo_other->Write();
      output_file->Close();
    }
  }
}

  
