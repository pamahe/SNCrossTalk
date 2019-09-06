#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TH1D.h>

void waveformDrawer() {

  std::vector<std::string> run_list = {"101", "102"};

  for (auto run : run_list) {

    std::cout << "Current run is : " << run << "\n\n";

    std::string hit_list_name = "/sps/nemo/scratch/pamahe/SNCrossTalk/CoincSaml/hit_list/list_CS_run-" + run + "_HT.lis";
    ifstream hit_list(hit_list_name.c_str());

    std::string input_filename = "/sps/nemo/scratch/pamahe/SNCrossTalk/SortTree/results/full/run-" + run + "_HTs_full.root";
    TFile *input_file = new TFile(input_filename.c_str(), "READ");
    TTree *input_tree = (TTree *)input_file->Get("Calo_Hits");

    Int_t hit_number;
    UInt_t slot_id, channel_id;
    Int_t waveform_samples[1024];

    input_tree->SetBranchAddress("hit_number",&hit_number);
    input_tree->SetBranchAddress("slot_id",&slot_id);
    input_tree->SetBranchAddress("channel_id",&channel_id);
    input_tree->SetBranchAddress("waveform_samples",&waveform_samples);

    int entries = input_tree->GetEntries();

    if (!hit_list.is_open()) {
      std::cerr << "Could not open input-list file!" << std::endl;
      return(-1);
    }

    std::string output_filename = "/sps/nemo/scratch/pamahe/SNCrossTalk/CoincSaml/Plots/CS_plots_run-" + run + ".root";
    TFile *output_file = new TFile(output_filename.c_str(),"RECREATE");

    TDirectory *ref_OM[260];

    std::string line;
    int ref_slot = 0;
    int ref_channel = -1;
    int line_number = 0;
    while (getline(hit_list,line)) {

      if (line.empty()) continue;
      if (line[0] == '#') {
	ref_channel++;
	if (ref_channel == 13) {
	  ref_channel = 0;
	  ref_slot++;
	}
	std::string directory_name = "ref_cube_" + std::to_string(ref_slot) + "." + std::to_string(ref_channel);
	ref_OM[ref_slot * 13 + ref_channel] = output_file->mkdir(directory_name.c_str());
	ref_OM[ref_slot * 13 + ref_channel]->cd();
      }
      
      std::vector<std::string> temp;
      size_t pos = 0;
      int count = 0;
      std::string delimiter = "\t";
      while ((pos = line.find(delimiter)) != std::string::npos) {
	temp.push_back(line.substr(0,pos));
	count++;
	line.erase(0,pos + delimiter.length());
      }
      if (temp.size() <= 1) {
	temp.clear();
      }
      else {
	for (int i = 0; i < temp.size(); i++) {
	  
	  for (int ientries = 0; ientries < entries; ientries++) {

	    input_tree->GetEntry(ientries);
	    if (hit_number == std::stoi(temp[i])) {
	      
	      std::string waveform_name = std::to_string(slot_id) + "." + std::to_string(channel_id) + "_hit_" + temp[i];
	      std::string waveform_title = "Cube : " + std::to_string(slot_id) + "." + std::to_string(channel_id) + ", hit number : " + temp[i];
	
	      TH1D *waveform = new TH1D(waveform_name.c_str(), waveform_title.c_str(), 1024, 0.0, 1024.0);
	      for (int i = 0; i < 1024; i++) {
		waveform->SetBinContent(i + 1, waveform_samples[i]);
	      }
	      waveform->Write();
	    }
	  }
	}
      }
    }
  }
}
	  
	      
