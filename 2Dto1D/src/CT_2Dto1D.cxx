#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

#include <TFile.h>
#include <TH2D.h>
#include <TH1D.h>

// Low distance version with hardcoded position
// We only look two OMs away in a damond shape
void CT_2Dto1D() {

  // Different coincidence windows that will be computed
  // see coincidence.cxx README
  std::vector<string> ct_list = {"1", "10", "100", "1e3", "1e4", "1e5", "1e6"};

  for (auto ct : ct_list) {
    
    std::cout << "\nWorking on run 104, coincidence window : " << ct << "\n";
    
    // Setting input and output file names
    std::string input_path     = "/sps/nemo/scratch/pamahe/SNCrossTalk/Coincidence/results/run_104/";
    std::string input_filename = "Coinc_run-104_" + ct + ".root";
    std::string input_file_    = input_path + input_filename;
    
    std::string output_path    = "/sps/nemo/scratch/pamahe/SNCrossTalk/2Dto1D/results/";
    std::string output_filename = "CT_1D_run-104_HT_" + ct + ".root";
    std::string output_file_   = output_path + output_filename;
    
    // Opening input file
    TFile *input_file = new TFile(input_file_.c_str(),"READ");
    
    // Creating output file
    TFile *output_file = new TFile(output_file_.c_str(),"RECREATE");
    
    
    
    
    
    // Setting the reference cube
    for (int ref_board = 0; ref_board < 20; ref_board++) {
      for (int ref_channel = 0; ref_channel < 12; ref_channel++) {
	std::cout << ref_board << "\t" << ref_channel << "\n";
	
	// Setting names for input and output histograms (number of hits and probabilities)
	std::string input_histo_hits_name = "cth_" + std::to_string(ref_board) + "." + std::to_string(ref_channel); 
	std::string input_histo_prob_name = "ctp_" + std::to_string(ref_board) + "." + std::to_string(ref_channel);
	
	std::string output_histo_hits_name  =  "cth_" + std::to_string(ref_board) + "." + std::to_string(ref_channel);
	std::string output_histo_hits_title =  "CT Hits count, reference cube : " + std::to_string(ref_board) + "." + std::to_string(ref_channel);
	std::string output_histo_prob_name  =  "ctp_" + std::to_string(ref_board) + "." + std::to_string(ref_channel);
	std::string output_histo_prob_title =  "CT Probability, reference cube : " + std::to_string(ref_board) + "." + std::to_string(ref_channel);
	
	// Opening input histograms
	TH2D *input_histo_hits = (TH2D *)input_file->Get(input_histo_hits_name.c_str());
	TH2D *input_histo_prob = (TH2D *)input_file->Get(input_histo_prob_name.c_str());
	
	// Creating output histograms
	TH1D *output_histo_hits = new TH1D(output_histo_hits_name.c_str(), output_histo_hits_title.c_str(), 13, 0.0, 13.0);
	TH1D *output_histo_prob = new TH1D(output_histo_prob_name.c_str(), output_histo_prob_title.c_str(), 13, 0.0, 13.0);
	
	std::string binLabel = ""; // bin label is for example : 0.12.5    12 is column, 5 is row and 0 was hand-written before IT wall implementation
	binLabel = "0." + std::to_string(ref_board) + "." + std::to_string(ref_channel);
	output_histo_hits->GetXaxis()->SetBinLabel(1, binLabel.c_str());
	output_histo_prob->GetXaxis()->SetBinLabel(1, binLabel.c_str());
	
	// the output probabilities are in %  and reference OM probability is set to 1
	output_histo_hits->SetBinContent(1 , input_histo_hits->GetBinContent(ref_board + 1, ref_channel + 1));
	output_histo_prob->SetBinContent(1 , 1);
   
	
	if (ref_channel % 2 == 0) { // If the reference cube is on an even channel, the electronical neighbour is above him
	  
	  if ((ref_board + 1) < 20) { // Not top row so there is an OM above it
	    output_histo_hits->SetBinContent(2, input_histo_hits->GetBinContent(ref_board + 1, ref_channel + 2));
	    output_histo_prob->SetBinContent(2, input_histo_prob->GetBinContent(ref_board + 1, ref_channel + 2));
	    
	    binLabel = "0." + std::to_string(ref_board) + "." + std::to_string(ref_channel + 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(2, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(2, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(2, 0); // For top row
	    output_histo_prob->SetBinContent(2, 0);
	    
	    binLabel = "0." + std::to_string(ref_board) + "." + std::to_string(ref_channel + 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(2, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(2, binLabel.c_str());
	  }
	  
	  
	  // The  remaining cubes are : below, left, right, diagonals, 2 up, 2down, 2 left, 2 right
	  // - Below
	  if ( (ref_channel - 1) > -1) { 
	    output_histo_hits->SetBinContent(4, input_histo_hits->GetBinContent(ref_board + 1, ref_channel ));
	    output_histo_prob->SetBinContent(4, input_histo_prob->GetBinContent(ref_board + 1, ref_channel ));
	    
	    binLabel = "0." + std::to_string(ref_board) + "." + std::to_string(ref_channel - 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(4, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(4, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(4, 0);
	    output_histo_prob->SetBinContent(4, 0);
	    
	    binLabel = "0." + std::to_string(ref_board) + "." + std::to_string(ref_channel - 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(4, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(4, binLabel.c_str());
	  }
	  // - Left
	  if ( (ref_board - 1) > -1) {
	    output_histo_hits->SetBinContent(5, input_histo_hits->GetBinContent(ref_board , ref_channel + 1));
	    output_histo_prob->SetBinContent(5, input_histo_prob->GetBinContent(ref_board , ref_channel + 1));
	    
	    binLabel = "0." + std::to_string(ref_board - 1) + "." + std::to_string(ref_channel);
	    output_histo_hits->GetXaxis()->SetBinLabel(5, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(5, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(5, 0);
	    output_histo_prob->SetBinContent(5, 0);
	    
	    binLabel = "0." + std::to_string(ref_board - 1) + "." + std::to_string(ref_channel);
	    output_histo_hits->GetXaxis()->SetBinLabel(5, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(5, binLabel.c_str());
	  }
	  // - Right
	  if ( (ref_board + 1) < 20) {
	    output_histo_hits->SetBinContent(3, input_histo_hits->GetBinContent(ref_board + 2, ref_channel + 1));
	    output_histo_prob->SetBinContent(3, input_histo_prob->GetBinContent(ref_board + 2, ref_channel + 1));
	    
	    binLabel = "0." + std::to_string(ref_board + 1) + "." + std::to_string(ref_channel);
	    output_histo_hits->GetXaxis()->SetBinLabel(3, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(3, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(3, 0);
	    output_histo_prob->SetBinContent(3, 0);
	    
	    binLabel = "0." + std::to_string(ref_board + 1) + "." + std::to_string(ref_channel);
	    output_histo_hits->GetXaxis()->SetBinLabel(3, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(3, binLabel.c_str());
	  }
	  // - 4 Diagonals
	  if ( (ref_board + 1) < 20 and (ref_channel + 1) < 13) {
	    output_histo_hits->SetBinContent(6, input_histo_hits->GetBinContent(ref_board + 2, ref_channel + 2));
	    output_histo_prob->SetBinContent(6, input_histo_prob->GetBinContent(ref_board + 2, ref_channel + 2));
	    
	    binLabel = "0." + std::to_string(ref_board + 1) + "." + std::to_string(ref_channel + 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(6, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(6, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(6, 0);
	    output_histo_prob->SetBinContent(6, 0);
	    
	    binLabel = "0." + std::to_string(ref_board + 1) + "." + std::to_string(ref_channel + 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(6, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(6, binLabel.c_str());
	  }
	  if ( (ref_board + 1) < 20 and (ref_channel - 1) > -1) {
	    output_histo_hits->SetBinContent(7, input_histo_hits->GetBinContent(ref_board + 2, ref_channel));
	    output_histo_prob->SetBinContent(7, input_histo_prob->GetBinContent(ref_board + 2, ref_channel));
	    
	    binLabel = "0." + std::to_string(ref_board + 1) + "." + std::to_string(ref_channel - 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(7, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(7, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(7, 0);
	    output_histo_prob->SetBinContent(7, 0);
	    
	    binLabel = "0." + std::to_string(ref_board + 1) + "." + std::to_string(ref_channel - 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(7, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(7, binLabel.c_str());
	  }
	  if ( (ref_board - 1) > -1 and (ref_channel + 1) < 13) {
	    output_histo_hits->SetBinContent(9, input_histo_hits->GetBinContent(ref_board, ref_channel + 2));
	    output_histo_prob->SetBinContent(9, input_histo_prob->GetBinContent(ref_board, ref_channel + 2));
	    
	    binLabel = "0." + std::to_string(ref_board - 1) + "." + std::to_string(ref_channel + 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(9, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(9, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(9, 0);
	    output_histo_prob->SetBinContent(9, 0);
	    
	    binLabel = "0." + std::to_string(ref_board - 1) + "." + std::to_string(ref_channel + 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(9, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(9, binLabel.c_str());
	  }
	  if ( (ref_board - 1) > -1 and (ref_channel - 1) > -1) {
	    output_histo_hits->SetBinContent(8, input_histo_hits->GetBinContent(ref_board, ref_channel));
	    output_histo_prob->SetBinContent(8, input_histo_prob->GetBinContent(ref_board, ref_channel));
	    
	    binLabel = "0." + std::to_string(ref_board - 1) + "." + std::to_string(ref_channel - 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(8, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(8, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(8, 0);
	    output_histo_prob->SetBinContent(8, 0);
	    
	    binLabel = "0." + std::to_string(ref_board - 1) + "." + std::to_string(ref_channel - 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(8, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(8, binLabel.c_str());
	  }
	  // - 2 Ups
	  if ( (ref_channel + 2) < 20) {
	    output_histo_hits->SetBinContent(10, input_histo_hits->GetBinContent(ref_board + 1, ref_channel + 3));
	    output_histo_prob->SetBinContent(10, input_histo_prob->GetBinContent(ref_board + 1, ref_channel + 3));
	    
	    binLabel = "0." + std::to_string(ref_board) + "." + std::to_string(ref_channel + 2);
	    output_histo_hits->GetXaxis()->SetBinLabel(10, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(10, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(10, 0);
	    output_histo_prob->SetBinContent(10, 0);
	    
	    binLabel = "0." + std::to_string(ref_board) + "." + std::to_string(ref_channel + 2);
	    output_histo_hits->GetXaxis()->SetBinLabel(10, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(10, binLabel.c_str());
	  }
	  // - 2 Below
	  if ( (ref_channel - 2) > -1) {
	    output_histo_hits->SetBinContent(12, input_histo_hits->GetBinContent(ref_board + 1, ref_channel - 1));
	    output_histo_prob->SetBinContent(12, input_histo_prob->GetBinContent(ref_board + 1, ref_channel - 1));
	    
	    binLabel = "0." + std::to_string(ref_board) + "." + std::to_string(ref_channel - 2);
	    output_histo_hits->GetXaxis()->SetBinLabel(12, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(12, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(12, 0);
	    output_histo_prob->SetBinContent(12, 0);
	    
	    binLabel = "0." + std::to_string(ref_board) + "." + std::to_string(ref_channel - 2);
	    output_histo_hits->GetXaxis()->SetBinLabel(12, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(12, binLabel.c_str());
	  }
	  // - 2 Left
	  if ( (ref_board - 2) > -1) {
	    output_histo_hits->SetBinContent(13, input_histo_hits->GetBinContent(ref_board - 1, ref_channel + 1));
	    output_histo_prob->SetBinContent(13, input_histo_prob->GetBinContent(ref_board - 1, ref_channel + 1));
	    
	    binLabel = "0." + std::to_string(ref_board - 2) + "." + std::to_string(ref_channel);
	    output_histo_hits->GetXaxis()->SetBinLabel(13, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(13, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(12, 0);
	    output_histo_prob->SetBinContent(12, 0);
	    
	    binLabel = "0." + std::to_string(ref_board - 2) + "." + std::to_string(ref_channel);
	    output_histo_hits->GetXaxis()->SetBinLabel(13, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(13, binLabel.c_str());
	  }
	  // - 2 Right
	  if ( (ref_board + 2) < 20) {
	    output_histo_hits->SetBinContent(11, input_histo_hits->GetBinContent(ref_board + 3, ref_channel + 1));
	    output_histo_prob->SetBinContent(11, input_histo_prob->GetBinContent(ref_board + 3, ref_channel + 1));
	    
	    binLabel = "0." + std::to_string(ref_board + 2) + "." + std::to_string(ref_channel);
	    output_histo_hits->GetXaxis()->SetBinLabel(11, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(11, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(11, 0);
	    output_histo_prob->SetBinContent(11, 0);
	    
	    binLabel = "0." + std::to_string(ref_board + 2) + "." + std::to_string(ref_channel);
	    output_histo_hits->GetXaxis()->SetBinLabel(11, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(11, binLabel.c_str());
	  }
	}
	
	
	
	
	
	else { // If the electronical neighbour is below the reference module
	  
	  if ((ref_board - 1) > 0) { // If not bottom row, there is an OM below it
	    output_histo_hits->SetBinContent(2, input_histo_hits->GetBinContent(ref_board + 1, ref_channel));
	    output_histo_prob->SetBinContent(2, input_histo_prob->GetBinContent(ref_board + 1, ref_channel));
	    
	    binLabel = "0." + std::to_string(ref_board) + "." + std::to_string(ref_channel - 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(2, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(2, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(2, 0); // For bottom row
	    output_histo_prob->SetBinContent(2, 0);
	    
	    binLabel = "0." + std::to_string(ref_board) + "." + std::to_string(ref_channel - 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(2, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(2, binLabel.c_str());
	  }
	  
	  // The remaining cubes are : up, left, right, diagonals, 2up, 2 down, 2 left and 2 right
	  // - Up 
	  if ( (ref_channel - 1) > -1) { 
	    output_histo_hits->SetBinContent(4, input_histo_hits->GetBinContent(ref_board + 1, ref_channel + 2));
	    output_histo_prob->SetBinContent(4, input_histo_prob->GetBinContent(ref_board + 1, ref_channel + 2));
	    
	    binLabel = "0." + std::to_string(ref_board) + "." + std::to_string(ref_channel + 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(4, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(4, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(4, 0);
	    output_histo_prob->SetBinContent(4, 0);
	    
	    binLabel = "0." + std::to_string(ref_board) + "." + std::to_string(ref_channel  + 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(4, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(4, binLabel.c_str());
	  }
	  // - Left
	  if ( (ref_board - 1) > -1) {
	    output_histo_hits->SetBinContent(3, input_histo_hits->GetBinContent(ref_board, ref_channel + 1));
	    output_histo_prob->SetBinContent(3, input_histo_prob->GetBinContent(ref_board, ref_channel + 1));
	    
	    binLabel = "0." + std::to_string(ref_board - 1) + "." + std::to_string(ref_channel);
	    output_histo_hits->GetXaxis()->SetBinLabel(3, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(3, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(3, 0);
	    output_histo_prob->SetBinContent(3, 0);
	    
	    binLabel = "0." + std::to_string(ref_board - 1) + "." + std::to_string(ref_channel);
	    output_histo_hits->GetXaxis()->SetBinLabel(3, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(3, binLabel.c_str());
	  }
	  // - Right
	  if ( (ref_board + 1) < 20) {
	    output_histo_hits->SetBinContent(5, input_histo_hits->GetBinContent(ref_board + 2, ref_channel + 1));
	    output_histo_prob->SetBinContent(5, input_histo_prob->GetBinContent(ref_board + 2, ref_channel + 1));
	    
	    binLabel = "0." + std::to_string(ref_board + 1) + "." + std::to_string(ref_channel);
	    output_histo_hits->GetXaxis()->SetBinLabel(5, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(5, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(5, 0);
	    output_histo_prob->SetBinContent(5, 0);
	    
	    binLabel = "0." + std::to_string(ref_board + 1) + "." + std::to_string(ref_channel);
	    output_histo_hits->GetXaxis()->SetBinLabel(5, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(5, binLabel.c_str());
	  }
	  // - 4 Diagonals
	  if ( (ref_board + 1) < 20 and (ref_channel + 1) < 13) {
	    output_histo_hits->SetBinContent(6, input_histo_hits->GetBinContent(ref_board + 2, ref_channel + 2));
	    output_histo_prob->SetBinContent(6, input_histo_prob->GetBinContent(ref_board + 2, ref_channel + 2));
	    
	    binLabel = "0." + std::to_string(ref_board + 1) + "." + std::to_string(ref_channel + 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(6, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(6, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(6, 0);
	    output_histo_prob->SetBinContent(6, 0);
	    
	    binLabel = "0." + std::to_string(ref_board + 1) + "." + std::to_string(ref_channel + 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(6, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(6, binLabel.c_str());
	  }
	  if ( (ref_board + 1) < 20 and (ref_channel - 1) > -1) {
	    output_histo_hits->SetBinContent(7, input_histo_hits->GetBinContent(ref_board + 2, ref_channel));
	    output_histo_prob->SetBinContent(7, input_histo_prob->GetBinContent(ref_board + 2, ref_channel));
	    
	    binLabel = "0." + std::to_string(ref_board + 1) + "." + std::to_string(ref_channel - 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(7, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(7, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(7, 0);
	    output_histo_prob->SetBinContent(7, 0);
	    
	    binLabel = "0." + std::to_string(ref_board + 1) + "." + std::to_string(ref_channel - 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(7, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(7, binLabel.c_str());
	  }
	  if ( (ref_board - 1) > -1 and (ref_channel + 1) < 13) {
	    output_histo_hits->SetBinContent(9, input_histo_hits->GetBinContent(ref_board, ref_channel + 2));
	    output_histo_prob->SetBinContent(9, input_histo_prob->GetBinContent(ref_board, ref_channel + 2));
	    
	    binLabel = "0." + std::to_string(ref_board - 1) + "." + std::to_string(ref_channel + 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(9, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(9, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(9, 0);
	    output_histo_prob->SetBinContent(9, 0);
	    
	    binLabel = "0." + std::to_string(ref_board - 1) + "." + std::to_string(ref_channel + 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(9, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(9, binLabel.c_str());
	  }
	  if ( (ref_board - 1) > -1 and (ref_channel - 1) > -1) {
	    output_histo_hits->SetBinContent(8, input_histo_hits->GetBinContent(ref_board, ref_channel));
	    output_histo_prob->SetBinContent(8, input_histo_prob->GetBinContent(ref_board, ref_channel));
	    
	    binLabel = "0." + std::to_string(ref_board - 1) + "." + std::to_string(ref_channel - 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(8, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(8, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(8, 0);
	    output_histo_prob->SetBinContent(8, 0);
	    
	    binLabel = "0." + std::to_string(ref_board - 1) + "." + std::to_string(ref_channel - 1);
	    output_histo_hits->GetXaxis()->SetBinLabel(8, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(8, binLabel.c_str());
	  }
	  // - 2 Ups
	  if ( (ref_channel + 2) < 20) {
	    output_histo_hits->SetBinContent(10, input_histo_hits->GetBinContent(ref_board + 1, ref_channel + 3));
	    output_histo_prob->SetBinContent(10, input_histo_prob->GetBinContent(ref_board + 1, ref_channel + 3));
	    
	    binLabel = "0." + std::to_string(ref_board) + "." + std::to_string(ref_channel + 2);
	    output_histo_hits->GetXaxis()->SetBinLabel(10, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(10, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(10, 0);
	    output_histo_prob->SetBinContent(10, 0);
	    
	    binLabel = "0." + std::to_string(ref_board) + "." + std::to_string(ref_channel + 2);
	    output_histo_hits->GetXaxis()->SetBinLabel(10, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(10, binLabel.c_str());
	  }	
	  // - 2 Below
	  if ( (ref_channel - 2) > -1) {
	    output_histo_hits->SetBinContent(12, input_histo_hits->GetBinContent(ref_board + 1, ref_channel - 1));
	    output_histo_prob->SetBinContent(12, input_histo_prob->GetBinContent(ref_board + 1, ref_channel - 1));
	    
	    binLabel = "0." + std::to_string(ref_board) + "." + std::to_string(ref_channel - 2);
	    output_histo_hits->GetXaxis()->SetBinLabel(12, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(12, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(12, 0);
	    output_histo_prob->SetBinContent(12, 0);
	    
	    binLabel = "0." + std::to_string(ref_board) + "." + std::to_string(ref_channel - 2);
	    output_histo_hits->GetXaxis()->SetBinLabel(12, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(12, binLabel.c_str());
	  }
	  // - 2 Left
	  if ( (ref_board - 2) > -1) {
	    output_histo_hits->SetBinContent(13, input_histo_hits->GetBinContent(ref_board - 1, ref_channel + 1));
	    output_histo_prob->SetBinContent(13, input_histo_prob->GetBinContent(ref_board - 1, ref_channel + 1));
	    
	    binLabel = "0." + std::to_string(ref_board - 2) + "." + std::to_string(ref_channel);
	    output_histo_hits->GetXaxis()->SetBinLabel(13, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(13, binLabel.c_str());
	  }
	  
	  else {
	    output_histo_hits->SetBinContent(13, 0);
	    output_histo_prob->SetBinContent(13, 0);
	    
	    binLabel = "0." + std::to_string(ref_board - 2) + "." + std::to_string(ref_channel);
	    output_histo_hits->GetXaxis()->SetBinLabel(13, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(13, binLabel.c_str());
	  }
	  // - 2 Right
	  if ( (ref_board + 2) < 20) {
	    output_histo_hits->SetBinContent(11, input_histo_hits->GetBinContent(ref_board + 3, ref_channel + 1));
	    output_histo_prob->SetBinContent(11, input_histo_prob->GetBinContent(ref_board + 3, ref_channel + 1));
	    
	    binLabel = "0." + std::to_string(ref_board + 2) + "." + std::to_string(ref_channel);
	    output_histo_hits->GetXaxis()->SetBinLabel(11, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(11, binLabel.c_str());
	  }
	  else {
	    output_histo_hits->SetBinContent(11, 0);
	    output_histo_prob->SetBinContent(11, 0);
	    
	    binLabel = "0." + std::to_string(ref_board + 2) + "." + std::to_string(ref_channel);
	    output_histo_hits->GetXaxis()->SetBinLabel(11, binLabel.c_str());
	    output_histo_prob->GetXaxis()->SetBinLabel(11, binLabel.c_str());
	  }
	}
      }
    }   
    input_file->Close();
    output_file->Write();
    output_file->Close();
    
  }
}
