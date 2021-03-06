#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <fstream>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>

void plotCoincSaml() {

  // Opening file with list of coincidences hit number
  std::ifstream hit_list("/sps/nemo/scratch/pamahe/SNCrossTalk/CoincSaml/hit_list/list_CS_run-104_HT.lis");
  if (!hit_list) std::cout << "Could not open hit list file!\n";
  
  // Opening data file to have access to waveform samples
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

  // Opening output file
  TFile *output_file = new TFile("/sps/nemo/scratch/pamahe/SNCrossTalk/PlotCoincSaml/PlotCS_run-104.root","RECREATE");
  TDirectory *ref_OM[260]; // Creating a directory to store coincidences hits per reference OM

   std::vector<std::string> hits;
   std::vector<std::vector<std::string> > events;
  // Allow to change reference hits based on specific lines in input list
  int ref_slot = 0;
  int ref_channel = -1;

  while (hit_list and !hit_list.eof()) {
    std::string line;
    std::getline(hit_list,line);

    if (line[0] == '#') {  // line that start with # indicate a change in reference module
      ref_channel++;
      if (ref_channel == 13) {
	ref_channel = 0;
	ref_slot++;
      }
      //std::cout << ref_slot << "\t" << ref_channel << "\n";
      
      // Creating directory per reference OM
      std::string dirName = "ref_cube_" + std::to_string(ref_slot) + "." + std::to_string(ref_channel);
      ref_OM[ref_slot * 13 + ref_channel] = output_file->mkdir(dirName.c_str());
    }
    
    // Specific hit list formatting :
    //      reference hit
    //      coincidence hit
    //      blank line etc
    // we change from "column" to "line" format
    
    if (line.empty()) { // the reference hit will change next line, we need to send events in memory to another place
      if (hits.size() > 1) { //If there is at least one coincidence hit
						
	std::string position = std::to_string(ref_slot) + "." + std::to_string(ref_channel);
	auto it = hits.begin();
	hits.insert(it, position);  
	events.push_back(hits);
	
      }
      hits.clear();
    }
    if (!line.empty() and line[0] != '#') { // This is the line that store events in the temp vector, should probably be moved above the if(line.empty())
      hits.push_back(line);
    }
  }

  for (int ientries = 0; ientries < entries; ientries++) {
    
    if (ientries % 1000000 == 0) {
      std::cout << ientries << "/" << entries << "\n";
    }
    
    input_tree->GetEntry(ientries);
    ref_OM[slot_id * 13 + channel_id]->cd();
    std::string position = std::to_string(slot_id) + "." + std::to_string(channel_id);
    
    for (int ievent = 0; ievent < events.size(); ievent++) {
      //std::cout << "Events size = " << events.size() << "\n";
      //std::cout << events[ievent][0] << "\t" << events[ievent][1] << "\t" <<  events[ievent][2] << "\n";

      //PENSER A DELETE LE CANVAS
      if (position == events[ievent][0]) {
	if (std::to_string(hit_number) == events[ievent][1]) {
	  
	  std::cout << "found hit number : " << hit_number << "\n";
	  
	  std::string canvas_title = "Reference (" + events[ievent][1] + ") and coincidence (" + events[ievent][2] + "hit waveform";
	  std::string canvas_name = events[ievent][1] + "_" + events[ievent][2];
	    
	  TCanvas *c = new TCanvas(canvas_name.c_str(),canvas_title.c_str(),800,600);
	  TPad *c1 = new TPad("c1","",0.01, 0.01, 0.51, 0.99);
	  TPad *c2 = new TPad("c2","",0.51, 0.01, 0.99, 0.99);
	  c1->Draw(); c2->Draw();
	  
	  c1->cd();
	  std::string waveform_title = "Waveform hit number " + std::to_string(hit_number) + " run 104";
	  TH1D *waveform_1 = new TH1D("wf_1",waveform_title.c_str(), 1024, 0.0, 1024.0);
	  for (int ibin = 0; ibin < 1024; ibin++) {
	    waveform_1->SetBinContent(ibin + 1, waveform_samples[ibin]);
	  }
	  waveform_1->Draw();
	  c->Update();
	  
	  for (int isecond = (ientries - 10); isecond < (ientries + 10); isecond++) {
	    input_tree->GetEntry(isecond);
	    if (std::to_string(hit_number) == events[ievent][2]) {
	      
	      std::cout << "\tfound hit number : " << hit_number << "\n\n";
	      
	      c2->cd();
	      std::string waveform_title = "Waveform hit number " + std::to_string(hit_number) + " run 104";
	      TH1D *waveform_2 = new TH1D("wf_2",waveform_title.c_str(), 1024, 0.0, 1024.0);
	      for (int ibin = 0; ibin < 1024; ibin++) {
		waveform_2->SetBinContent(ibin + 1, waveform_samples[ibin]);
	      }
	      waveform_2->Draw();
	      c->Update();
	      c->Write();
	      waveform_1->Delete();
	      waveform_2->Delete();
	      //c1->Delete(); c2->Delete();
	      //c->Delete();
	      break;
	    }
	  }   
	}
      }
    }
  }
  
  hit_list.close();
  input_file->Close();
  output_file->Close();
}
 
