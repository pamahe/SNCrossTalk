// Standard library :
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

// Third party :
// - ROOT
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

int TriggerRate() {

  std::string input_path = "/sps/nemo/scratch/pamahe/SNCrossTalk/SortTree/results/basic/";
  std::string output_path = "/sps/nemo/scratch/pamahe/SNCrossTalk/TriggerRate/results/";

  // run list to work on
  std::vector<int> run_list = {100, 101, 102, 104, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 119, 120, 121, 122, 127, 128, 129, 130, 131, 132, 133, 134, 135};

  for (auto run : run_list) {

    // Open input files
    std::string input_filename  = input_path + "run-" + std::to_string(run) + "_HTs_basic.root";
    std::string output_filename = output_path + "run_" + std::to_string(run) + "_HT_TR.root";
        
    TFile *input_file = new TFile(input_filename.c_str(),"READ");
    TTree *input_tree = (TTree *)input_file->Get("Calo_Hits");
    
    UInt_t slot_id, channel_id;
    Double_t tdc_ns;
    UInt_t lt_trig_count;

    input_tree->SetBranchAddress("slot_id",&slot_id);
    input_tree->SetBranchAddress("channel_id",&channel_id);
    input_tree->SetBranchAddress("tdc_ns",&tdc_ns);
    input_tree->SetBranchAddress("lt_trig_count",&lt_trig_count);
    int entries = input_tree->GetEntries();
    
    Double_t channelBinning_fpga[5] = {0., 4., 8., 12., 13.};
    Double_t channelBinning_samlong[8] = {0., 2., 4., 6., 8., 10., 12., 13.};
    // Creating output file and histograms
    // _real_ histograms includes non registered events thanks to the lt_trig_count variable
    TFile *output_file = new TFile(output_filename.c_str(),"RECREATE");
    TH2D *output_histo      = new TH2D("TrigRate", "Trigger Rate for Registered Events per Optical Module, full run HT", 20, 0.0, 20.0, 13, 0.0, 13.0);
    TH2D *output_histo_samlong = new TH2D("SAMLONG_TR","Trigger Rate for Registered Events per SAMLONG, full run HT", 20, 0.0, 20.0, 7, channelBinning_samlong);
    TH2D *output_histo_fpga = new TH2D("FPGA_TR","Trigger Rate for Registered Events per FPGA, full run HT", 20, 0.0, 20.0, 4, channelBinning_fpga);
    TH2D *output_histo_real = new TH2D("REAL_TR","Trigger Rate , full run HT", 20, 0.0, 20.0, 13, 0.0, 13.0);
    TH2D *output_histo_real_saml = new TH2D("REAL_TR_SAMLONG","Trigger Rate SAMLONG, full run HT", 20, 0.0, 20.0, 7, channelBinning_samlong);
    TH2D *output_histo_real_fpga = new TH2D("REAL_TR_FPGA","Trigger Rate FPGA, full run HT", 20, 0.0, 20.0, 4, channelBinning_fpga);

    // Store the events per OM
    Int_t Calo_Hits[260] = {0};
    Int_t Calo_Hits_real[260] = {0};

    Double_t TDC_min, TDC_max, run_duration;

    for (int ientries = 0; ientries < entries; ientries ++) {
      
      input_tree->GetEntry(ientries);

      // Find start and end of the run
      if (ientries == 0) {
	TDC_min = tdc_ns;
	TDC_max = tdc_ns;
      } else {
	if (tdc_ns > TDC_max) {
	  TDC_max = tdc_ns;
	}
	if (tdc_ns < TDC_min) {
	  TDC_min = tdc_ns;
	}
      }
      // Locate the event
      int position = slot_id + channel_id + slot_id * 12;
      Calo_Hits[position] += 1;
      Calo_Hits_real[position] += 1;
      Calo_Hits_real[position] += lt_trig_count;
    }
    // Compute the run duration
    run_duration = (TDC_max - TDC_min) * 1e-9;
    std::cout << "\nRun : " << run << "\n";
    std::cout << "Run duration : " << TDC_max << " - " << TDC_min << " = " << run_duration << "\n"; 

    // Fill histograms with the trigger rate
    for (int ipos = 0; ipos < 260; ipos++) {
      
      int ref_board = ipos / 13;
      int ref_channel = (ipos - ref_board * 13);
      double binContent = (double) Calo_Hits[ipos] / run_duration;
      double binContent_real = (double) Calo_Hits_real[ipos] / run_duration;
      output_histo->Fill(ref_board, ref_channel, binContent);
      output_histo_samlong->Fill(ref_board, ref_channel, binContent);
      output_histo_fpga->Fill(ref_board, ref_channel, binContent);
      output_histo_real->Fill(ref_board, ref_channel, binContent_real);
      output_histo_real_saml->Fill(ref_board, ref_channel, binContent_real);
      output_histo_real_fpga->Fill(ref_board, ref_channel, binContent_real);
    }
    
    input_file->Close();
    output_file->Write();
    output_file->Close();
  }
    
  return 0;
}

