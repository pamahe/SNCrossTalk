#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>

#include <TFile.h>
#include <TTree.h>
#include <TH2D.h>

void C0CM() {

  // Opening input ROOT file
  TFile *input_file = new TFile("/sps/nemo/scratch/pamahe/SNCrossTalk/SortTree/results/full/run-104_HTs_full.root","READ");
  TTree *input_tree = (TTree *)input_file->Get("Calo_Hits");

  // Adressing input tree
  Int_t hit_number;
  UInt_t slot_id, channel_id;
  Short_t raw_peak;
  Int_t raw_charge;
  Double_t tdc_ns;
  input_tree->SetBranchAddress("hit_number",&hit_number);
  input_tree->SetBranchAddress("slot_id",&slot_id);
  input_tree->SetBranchAddress("channel_id",&channel_id);
  input_tree->SetBranchAddress("raw_peak",&raw_peak);
  input_tree->SetBranchAddress("raw_charge",&raw_charge);
  input_tree->SetBranchAddress("tdc_ns",&tdc_ns);
  int entries = input_tree->GetEntries();

  // Creating output file
  TFile *output_file = new TFile("/sps/nemo/scratch/pamahe/SNCrossTalk/C0CM/C0CM_run-104_HT.root","RECREATE");

  // Defining a binning to plot the TH2D per FPGA
  Double_t channelBinning_fpga[5] = {0., 4., 8., 12., 13.};

  // Creating output TH2D
  TH2D *perOM = new TH2D("OM","C0/CM per OM for run 104", 20, 0.0, 20.0, 13, 0.0, 13.0);
  TH2D *perFPGA = new TH2D("FPGA","C0/CM per FPGA for run 104", 20, 0.0, 20.0, 4, channelBinning_fpga);

  Double_t TDC_MAX = 0;
  Int_t hits_start[260] = {0};
  Int_t hits_end[260] = {0};
 
  // Looping over all entries
  for (int ientries = 0; ientries < entries; ientries++) {
    if (ientries % 1000000 == 0) std::cout << ientries << "/" << entries << std::endl;
    input_tree->GetEntry(ientries);

    // Position of the event on the wall
    int position = slot_id * 13 + channel_id;

    // cutting the run in two:
    // -> Before 50 ms
    // -> After 50 ms
    if (tdc_ns <= 50000000) hits_start[position] += 1;
    if (tdc_ns > 50000000) hits_end[position] += 1;
    // Getting the higher time of arrival
    if (tdc_ns > TDC_MAX) TDC_MAX = tdc_ns;
  }

  Double_t C0CM_OM[260];

  for (int ipos = 0; ipos < 260; ipos++) {
    
    // Computing ratio :
    // Trigger rate in the first 50 ms over
    // Trigger rate in the rest of the run
    // per OM
    C0CM_OM[ipos] = (double(hits_start[ipos]) / 50000000.0) / (double(hits_end[ipos]) / (TDC_MAX - 50000000.0));
    
    std::cout << hits_start[ipos] << "\t" << hits_end[ipos] << "\t" << C0CM_OM[ipos] << "\n";

    int slot = ipos / 13;
    int channel = ipos - slot * 13;

    // Filling the ratio and checking for NaN
    if (C0CM_OM[ipos] != C0CM_OM[ipos]) {
      perOM->Fill(slot , channel , 0.0);
      perFPGA->Fill(slot , channel , 0.0);
    } else {
      perOM->Fill(slot , channel , C0CM_OM[ipos]);
      perFPGA->Fill(slot , channel , C0CM_OM[ipos]);
    }
  }

  gStyle->SetOptStat(0);
  gStyle->SetPaintTextFormat("1.2e");
  perOM->SetMarkerSize(0.8);
  perFPGA->SetMarkerSize(0.8);

  input_file->Close();
  perOM->Write();
  perFPGA->Write();
  output_file->Close();
}
