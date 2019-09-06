#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

/// ROOT macro to manually compute charge of events based on the waveform
/// Used to verify the good behavior of software charge computation

void chargeComputation() {

  TFile *input_file = new TFile("/sps/nemo/scratch/pamahe/SNCrossTalk/crd2root/results/full/run-104_HT_full.root","READ");
  TTree *input_tree = (TTree *)input_file->Get("Calo_Hits");

  Int_t hit_number;
  UInt_t slot_id, channel_id;
  Int_t raw_charge;
  Int_t waveform_samples[1024];
  input_tree->SetBranchAddress("hit_number",&hit_number);
  input_tree->SetBranchAddress("slot_id",&slot_id);
  input_tree->SetBranchAddress("channel_id",&channel_id);
  input_tree->SetBranchAddress("raw_charge",&raw_charge);
  input_tree->SetBranchAddress("waveform_samples",&waveform_samples);
  int entries = input_tree->GetEntries();

  // Output file contains the manually computed charge in CC histo
  // and the software computed charge in DC histo to compare
  TFile *output_file = new TFile("/sps/nemo/scratch/pamahe/SNCrossTalk/chargeComputation/results/CC_run-104_HT.root","RECREATE");
  TH1D *data_charge = new TH1D("DC","Data Charge Distribution", 2000, -50000.0, 50000.0);
  TH1D *computed_charge = new TH1D("CC","Computed Charge Distribution", 2000, -50000.0, 50000.0);

  TH1D *waveform = new TH1D("w", "", 1024, 0.0, 1024.0);

  for (int ientries = 0; ientries < 1; ientries++) {
    input_tree->GetEntry(ientries);    
    if (ientries % 1000000 == 0) {
      std::cout << ientries << "/" << entries << "\n";
    }
    
    int peak_sample = 0;
    for (int isample = 0; isample < 1024; isample++) {
      waveform->SetBinContent(isample + 1, - waveform_samples[isample] + 2048); // Copy the waveform and shift it to positive values for integration
      if (waveform_samples[isample] < waveform_samples[peak_sample]) peak_sample = isample; // peak_sample is the sample of maximum charge
    }
    TCanvas *c = new TCanvas("c","",800,600);

    // Integration limits define in software
    // 64 samples before peak, 992 samples after peak (if I understood that correctly)
    int integrationStartSample = peak_sample - 64; 
    int integrationEndSample = integrationStartSample + 992;

    waveform->Draw("hist,same");

    // Draw lines at integration limits for better visualisation
    TLine *l1 = new TLine(integrationStartSample,0,integrationStartSample,5000);
    l1->SetLineColor(4);
    l1->SetLineStyle(4);
    l1->Draw("same0");
    TLine *l2 = new TLine(integrationEndSample,0,integrationEndSample,5000);
    l2->SetLineColor(3);
    l2->SetLineStyle(4);
    l2->Draw("same0");

    // Fill charge histograms, data is SN software computed
    data_charge->Fill(raw_charge);
    computed_charge->Fill(- waveform->Integral(integrationStartSample,integrationEndSample)); // negative sign because we shifted the waveform to positive values

    c->Update();
    //waveform->Delete();
    
  }
  input_file->Close();
  data_charge->Write();
  computed_charge->Write();
  output_file->Close();
}
