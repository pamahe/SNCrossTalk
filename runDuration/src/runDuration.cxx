#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>

#include <TFile.h>
#include <TBranch.h>
#include <TTree.h>

void runDuration()
{
  // run duration text file
  std::ofstream run_duration;
  run_duration.open("run_duration.txt");

  if(run_duration.is_open()) {

    // Run list to work on
    std::vector<int> run_list = {100, 101, 102, 104, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 119, 120, 121, 122, 127, 128, 129, 130, 131, 132, 133, 134, 135};
    for (auto run : run_list) {
      
      std::cout << "Current Run : " << run << "\n";

      run_duration << "Run : \t" << run << " Duration : \t";

      // input file and tree
      std::string input_filename = "/sps/nemo/scratch/pamahe/SNCrossTalk/SortTree/results/basic/run-" + std::to_string(run) + "_HTs_basic.root";
      TFile *input_file = new TFile(input_filename.c_str(),"READ");
      TTree *input_tree = (TTree *)input_file->Get("Calo_Hits");
      
      Double_t tdc_ns;
      input_tree->SetBranchAddress("tdc_ns",&tdc_ns);
      
      input_tree->GetEntry(input_tree->GetEntries() - 1); // Sorted tree, we look at last entry hence, run duration
      
      run_duration << tdc_ns * 1e-9 << "\n"; // output to text file
    }
  }
  else { std::cout << "Could not open output file.\n";}

  run_duration.close();
}

      
    
