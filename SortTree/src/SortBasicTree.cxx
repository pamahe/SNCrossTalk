// Standard library :
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <string>

// Third party:
// - ROOT
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TMath.h>

// Used to store root tree as a vector of struct for sorting
struct Event {
 
  Int_t hit_number, trigger_id;
  UInt_t slot_id, channel_id;
  UShort_t LT_flag, HT_flag;
  UInt_t event_id;
  
  ULong64_t raw_tdc;
  Double_t tdc_ns;
  UInt_t lt_trig_count, lt_time_count;

};

int SortBasicTree() {

  // Setting file names
  std::cout << "\n";
  std::string data_path = "/sps/nemo/scratch/pamahe/SNCrossTalk/crd2root/results/basic/";
  std::string out_path  = "/sps/nemo/scratch/pamahe/SNCrossTalk/SortTree/results/basic/";

  // Options to loop over different runs and settings
  std::vector<std::string> trigger_list={"HT"};
  std::vector<int> run_list = {138, 139};

  // loops on runs and trigger (HT, LT depending on what analysis is wanted)
  for (auto trig : trigger_list) {
    for (auto run : run_list) {
  
      std::string input_filename = data_path + "run-" + std::to_string(run) + "_s1_" + trig + "_basic.root";
      std::string output_filename = out_path + "run-" + std::to_string(run) + "_s1_" + trig + "s_basic.root";
  
      std::cout << "Current file is :\t" << input_filename << "\n";

      // Opening input file and tree
      TFile *input_file = new TFile(input_filename.c_str(),"READ");
      TTree *input_tree = (TTree *)input_file->Get("Calo_Hits");
      
      // Input tree variables
      Int_t i_hit_number, i_trigger_id;
      UInt_t i_slot_id, i_channel_id;
      UShort_t i_LT_flag, i_HT_flag;
      UInt_t i_event_id;
      ULong64_t i_raw_tdc;
      Double_t i_tdc_ns;
      UInt_t i_lt_trig_count, i_lt_time_count;
      
      // Input tree adressing
      input_tree->SetBranchAddress("hit_number",&i_hit_number);
      input_tree->SetBranchAddress("trigger_id",&i_trigger_id);
      input_tree->SetBranchAddress("slot_id",&i_slot_id);
      input_tree->SetBranchAddress("channel_id",&i_channel_id);
      input_tree->SetBranchAddress("LT_flag",&i_LT_flag);
      input_tree->SetBranchAddress("HT_flag",&i_HT_flag);
      input_tree->SetBranchAddress("event_id",&i_event_id);
      input_tree->SetBranchAddress("raw_tdc",&i_raw_tdc);
      input_tree->SetBranchAddress("tdc_ns",&i_tdc_ns);
      input_tree->SetBranchAddress("lt_trig_count",&i_lt_trig_count);
      input_tree->SetBranchAddress("lt_time_count",&i_lt_time_count);
 
      // Creating output file and tree
      TFile *output_file = new TFile(output_filename.c_str(),"RECREATE");
      TTree *output_tree = new TTree("Calo_Hits","Calorimeter hit data sorted");
      
      // Output tree variables
      Int_t o_hit_number, o_trigger_id;
      UInt_t o_slot_id, o_channel_id;
      UShort_t o_LT_flag, o_HT_flag;
      UInt_t o_event_id;
      ULong64_t o_raw_tdc;
      Double_t o_tdc_ns;
      UInt_t o_lt_trig_count, o_lt_time_count;
      
      // creating output file branches
      output_tree->Branch("hit_number",&o_hit_number,"o_hit_number/I");
      output_tree->Branch("trigger_id",&o_trigger_id,"o_trigger_id/I");
      output_tree->Branch("slot_id",&o_slot_id,"o_slot_id/i");
      output_tree->Branch("channel_id",&o_channel_id,"o_channel_id/i");
      output_tree->Branch("LT_flag",&o_LT_flag,"o_LT_flag/s");
      output_tree->Branch("HT_flag",&o_HT_flag,"o_HT_flag/s");
      output_tree->Branch("event_id",&o_event_id,"o_event_id/i");
      output_tree->Branch("raw_tdc",&o_raw_tdc,"o_raw_tdc/l");
      output_tree->Branch("tdc_ns",&o_tdc_ns,"o_tdc_ns/D");
      output_tree->Branch("lt_trig_count",&o_lt_trig_count,"o_lt_trig_count/i");
      output_tree->Branch("lt_time_count",&o_lt_time_count,"o_lt_time_count/i");
      
      // Main program
      std::vector<Event> Tree; // The input tree is stored in a vector of Event struct
      int entries = input_tree->GetEntries();
      
      for (int ientries = 0; ientries < entries; ientries++) {
	
	input_tree->GetEntry(ientries);
	
	Event event;
	event.hit_number = i_hit_number;
	event.trigger_id = i_trigger_id;
	event.slot_id = i_slot_id;
	event.channel_id = i_channel_id;
	event.LT_flag = i_LT_flag;
	event.HT_flag = i_HT_flag;
	event.event_id = i_event_id;
	event.raw_tdc = i_raw_tdc;
	event.tdc_ns = i_tdc_ns;
	event.lt_trig_count = i_lt_trig_count;
	event.lt_time_count = i_lt_time_count;
	
	Tree.push_back(event);
      }

      // Display size of of vector to assure that no data is lost
      std::cout << "Done getting input data, current tree size :\t" << Tree.size() << "\n";
      std::cout << "Sorting ...\n";

      // Sorting through std::sort based on time in timestamps (raw_tdc)
      std::sort(Tree.begin(), Tree.end(), [] (const Event& e1, const Event& e2) {
	return e1.raw_tdc < e2.raw_tdc;
      });

      // Correcting time so the first event is recorded with t = 0
      std::cout << "Done sorting, current tree size :\t" << Tree.size() << "\n";
      
      ULong64_t TDC_ref;
      double TDC_ns_ref;
      
      for (int ientries = 0; ientries < entries; ientries++) {
	
	if (ientries == 0) {
	  TDC_ref = Tree[ientries].raw_tdc;
	  TDC_ns_ref = Tree[ientries].tdc_ns;
	}
	Tree[ientries].raw_tdc = Tree[ientries].raw_tdc - TDC_ref;
	Tree[ientries].tdc_ns = Tree[ientries].tdc_ns - TDC_ns_ref;
      }
      
      std::cout << "Done correcting time.\n";

      // The sorted vector is used to fill the output tree
      for (int ientries = 0; ientries < entries; ientries++) {
	
	
	o_hit_number = Tree[ientries].hit_number;
	o_trigger_id = Tree[ientries].trigger_id;
	o_slot_id = Tree[ientries].slot_id;
	o_channel_id = Tree[ientries].channel_id;
	o_LT_flag = Tree[ientries].LT_flag;
	o_HT_flag = Tree[ientries].HT_flag;
	o_event_id = Tree[ientries].event_id;
	o_raw_tdc = Tree[ientries].raw_tdc;
	o_tdc_ns = Tree[ientries].tdc_ns;
	o_lt_trig_count = Tree[ientries].lt_trig_count;
	o_lt_time_count = Tree[ientries].lt_time_count;

	output_tree->Fill();
	
	if (ientries % 1000000 == 0) {
	  std::cout << "Filling : " << ientries << "/" << entries << "\n";
	}
      }
      
      std::cout << "Done Filling, output tree size :\t" << output_tree->GetEntries() << "\n";
      
      input_file->Close();
      output_file->Write();
      output_file->Close();
      
      std::cout << "\n\n";
      
    }
  }
  return 0;
}

