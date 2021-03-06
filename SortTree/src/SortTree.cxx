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
  
  Int_t raw_baseline;
  Double_t baseline_volt;
  
  Short_t raw_peak;
  UShort_t peak_cell;
  Double_t peak_volt;

  Int_t raw_charge;
  Double_t charge_picocoulomb;
  Short_t charge_overflow;
  
  UInt_t rising_cell, rising_offset;
  Double_t rising_ns;

  UInt_t falling_cell, falling_offset;
  Double_t falling_ns;

  UShort_t fcr;
  Double_t unix_time;
  Int_t waveform_samples[1024];

};

int SortTree() {

  // Setting file names
  std::cout << "\n";
  std::string data_path = "/sps/nemo/scratch/pamahe/SNCrossTalk/crd2root/results/full/";
  std::string out_path  = "/sps/nemo/scratch/pamahe/SNCrossTalk/SortTree/results/full/";

  // Options to loop over different runs and settings (go fetch the corresponding root file)
  std::vector<std::string> trigger_list={"HT"};
  std::vector<int> run_list = {100, 101, 102, 104, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 119, 120, 121, 122, 127, 128, 129, 130, 131, 132, 133, 134, 135};

  for (auto trig : trigger_list) {
    for (auto run : run_list) {
  
      std::string input_filename = data_path + "run-" + std::to_string(run) + "_" + trig + "_full.root";
      std::string output_filename = out_path + "run-" + std::to_string(run) + "_" + trig + "s_full.root";
  
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
  
      Int_t i_raw_baseline;
      Double_t i_baseline_volt;
      
      Short_t i_raw_peak;
      UShort_t i_peak_cell;
      Double_t i_peak_volt;
      
      Int_t i_raw_charge;
      Double_t i_charge_picocoulomb;
      Short_t i_charge_overflow;
      
      UInt_t i_rising_cell, i_rising_offset;
      Double_t i_rising_ns;
      
      UInt_t i_falling_cell, i_falling_offset;
      Double_t i_falling_ns;
      
      UShort_t i_fcr;
      Double_t i_unix_time;
      Int_t i_waveform_samples[1024];
    
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

      input_tree->SetBranchAddress("raw_baseline",&i_raw_baseline);
      input_tree->SetBranchAddress("baseline_volt",&i_baseline_volt);

      input_tree->SetBranchAddress("raw_peak",&i_raw_peak);
      input_tree->SetBranchAddress("peak_cell",&i_peak_cell);
      input_tree->SetBranchAddress("peak_volt",&i_peak_volt);
      
      input_tree->SetBranchAddress("raw_charge",&i_raw_charge);
      input_tree->SetBranchAddress("charge_picocoulomb",&i_charge_picocoulomb);
      input_tree->SetBranchAddress("charge_overflow",&i_charge_overflow);
      
      input_tree->SetBranchAddress("rising_cell",&i_rising_cell);
      input_tree->SetBranchAddress("rising_offset",&i_rising_offset);
      input_tree->SetBranchAddress("rising_ns",&i_rising_ns);
      input_tree->SetBranchAddress("falling_cell",&i_falling_cell);
      input_tree->SetBranchAddress("falling_offset",&i_falling_offset);
      input_tree->SetBranchAddress("falling_ns",&i_falling_ns);
      
      input_tree->SetBranchAddress("fcr",&i_fcr);
      input_tree->SetBranchAddress("unix_time",&i_unix_time);
      input_tree->SetBranchAddress("waveform_samples",&i_waveform_samples);

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
      
      Int_t o_raw_baseline;
      Double_t o_baseline_volt;
      
      Short_t o_raw_peak;
      UShort_t o_peak_cell;
      Double_t o_peak_volt;
      
      Int_t o_raw_charge;
      Double_t o_charge_picocoulomb;
      Short_t o_charge_overflow;
      
      UInt_t o_rising_cell, o_rising_offset;
      Double_t o_rising_ns;
      
      UInt_t o_falling_cell, o_falling_offset;
      Double_t o_falling_ns;
      
      UShort_t o_fcr;
      Double_t o_unix_time;
      Int_t o_waveform_samples[1024];
    
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
      
      output_tree->Branch("raw_baseline",&o_raw_baseline,"o_raw_baseline/I");
      output_tree->Branch("baseline_volt",&o_baseline_volt,"o_baseline_volt/D");
      
      output_tree->Branch("raw_peak",&o_raw_peak,"o_raw_peak/S");
      output_tree->Branch("peak_cell",&o_peak_cell,"o_peak_cell/s");
      output_tree->Branch("peak_volt",&o_peak_volt,"o_peak_volt/D");
      
      output_tree->Branch("raw_charge",&o_raw_charge,"o_raw_charge/I");
      output_tree->Branch("charge_picocoulomb",&o_charge_picocoulomb,"o_charge_picocoulomb/D");
      output_tree->Branch("charge_overflow",&o_charge_overflow,"o_charge_overflow/S");
      
      output_tree->Branch("rising_cell",&o_rising_cell,"o_rising_cell/i");
      output_tree->Branch("rising_offset",&o_rising_offset,"rising_offset/i");
      output_tree->Branch("rising_ns",&o_rising_ns,"o_rising_ns/D");
      
      output_tree->Branch("falling_cell",&o_falling_cell,"o_falling_cell/i");
      output_tree->Branch("falling_offset",&o_falling_offset,"o_falling_offset/i");
      output_tree->Branch("falling_ns",&o_falling_ns,"o_falling_ns/D");

      output_tree->Branch("fcr",&o_fcr,"o_fcr/s");
      output_tree->Branch("unix_time",&o_unix_time,"o_unix_time/D");
      output_tree->Branch("waveform_samples",&o_waveform_samples,"o_waveform_samples[1024]/I");

      // Main program
      std::vector<Event> Tree; // The input tree is stored in a vector of Event struct
      int entries = input_tree->GetEntries();

      for (int ientries = 0; ientries < entries; ientries++) {

	if (ientries % 1000000 == 0) {
	  std::cout << "Getting input data : " << ientries << "/" << entries << "\n";
	}
	
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
	
	
	event.raw_baseline = i_raw_baseline;
	event.baseline_volt = i_baseline_volt;
	
	event.raw_peak = i_raw_peak;
	event.peak_cell = i_peak_cell;
	event.peak_volt = i_peak_volt;
	
	event.raw_charge = i_raw_charge;
	event.charge_picocoulomb = i_charge_picocoulomb;
	event.charge_overflow = i_charge_overflow;
	
	event.rising_cell = i_rising_cell; 
	event.rising_offset = i_rising_offset;
	event.rising_ns = i_rising_ns;
	
	event.falling_cell = i_falling_cell;
	event.falling_offset = i_falling_offset;
	event.falling_ns = i_falling_ns;
	
	event.fcr = i_fcr;
	event.unix_time = i_unix_time;
	
	for (int isamples = 0; isamples < 1024; isamples++) {
	  event.waveform_samples[isamples] = i_waveform_samples[isamples];
	}

	Tree.push_back(event);
      }
	
      // Display size of vector to assure that no data is lost
      std::cout << "Done getting input data, current tree size :\t" << Tree.size() << "\n";
      std::cout << "Sorting ...\n";

      // Sorting though std::sort based on the hit time in timestamps (raw_tdc)
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

	o_raw_baseline = Tree[ientries].raw_baseline;
	o_baseline_volt = Tree[ientries].baseline_volt;

	o_raw_peak = Tree[ientries].raw_peak;
	o_peak_cell = Tree[ientries].peak_cell;
	o_peak_volt = Tree[ientries].peak_volt;
  
	o_raw_charge = Tree[ientries].raw_charge;
	o_charge_picocoulomb = Tree[ientries].charge_picocoulomb;
	o_charge_overflow = Tree[ientries].charge_overflow;
		
	o_rising_cell = Tree[ientries].rising_cell;
	o_rising_offset = Tree[ientries].rising_offset;
	o_rising_ns = Tree[ientries].rising_ns;
	o_falling_cell = Tree[ientries].falling_cell;
	o_falling_offset = Tree[ientries].falling_offset;
	o_falling_ns = Tree[ientries].falling_ns;

	o_fcr = Tree[ientries].fcr;
	o_unix_time = Tree[ientries].unix_time;
	
	for (int isamples = 0; isamples < 1024; isamples++) {
	  o_waveform_samples[isamples] = Tree[ientries].waveform_samples[isamples];
	}

	
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


