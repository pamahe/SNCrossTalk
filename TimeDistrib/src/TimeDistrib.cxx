// Standard library :
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

// Third party :
// - ROOT :
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TH2D.h>
#include <TStyle.h>
// - Boost :
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
// - Bayeux :
#include <bayeux/datatools/clhep_units.h>
#include <bayeux/datatools/temporary_files.h>
#include <bayeux/mygsl/tabulated_sampling.h>
#include <bayeux/mygsl/tabulated_function.h>
// - SNFEE :
#include <snfee/snfee.h>

// \brief Application configuration parameters
struct app_params_type
{  
  // Input root file
  std::string input_filename;

  // Output root file
  std::string output_filename;
};

// Main program
// Output a TH1D with the hit time distribution of events in a run
int main(int argc_, char** argv_) 
{
  snfee::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    
    // configuration
    app_params_type app_params;
    
    // Parse options
    namespace po = boost::program_options;
    po::options_description opts("Allowed options");
    opts.add_options()
      
      ("help","produce help message")

      ("input-file,i",
       po::value<std::string>(&app_params.input_filename)
       ->value_name("path"),
       "add a root input file should only contain HT = 1 hits")
      
      ("output-file,o",
       po::value<std::string>(&app_params.output_filename)
       ->value_name("path"),
       "add a root output file")
      
      ; // end of option descriptions
    
    // Describe command line arguments
    po::variables_map vm;
    po::store(po::command_line_parser(argc_,argv_)
	      .options(opts)
	      .run(), vm);
    po::notify(vm);

    // Use command line arguments
    if (vm.count("help")) {
      std::cout << "Coincidence : " 
		<< " Read a ROOT Calo Hits Tree (sorted) and evaluate coincidence for each OM"
		<< std::endl << std::endl;
      std::cout << "Datastudy [options]:" << std::endl << std::endl;
      std::cout << opts << std::endl << std::endl;
      std::cout << "Usage : " << std ::endl << std::endl;
      std::cout << "WIP" << std::endl << std::endl;
      return(-1);
    }

    // Opening and adressing input file
    TFile *input_file = new TFile(app_params.input_filename.c_str(),"READ");
    TTree *input_tree = (TTree *)input_file->Get("Calo_Hits");
    
    UInt_t slot_id, channel_id;
    Double_t tdc_ns;
    
    input_tree->SetBranchAddress("slot_id",&slot_id);
    input_tree->SetBranchAddress("channel_id",&channel_id);
    input_tree->SetBranchAddress("tdc_ns",&tdc_ns);
    int entries = input_tree->GetEntries();

    // Calculating run duration
    double TDC_MAX = 0;
    for (int ientries = 0; ientries < entries; ientries++) {
      input_tree->GetEntry(ientries);
      if(tdc_ns > TDC_MAX) TDC_MAX = tdc_ns;
    }

    TDC_MAX = TDC_MAX * 1e-9; // Time distribution in seconds
    // Creating output file
    TFile *output_file = new TFile(app_params.output_filename.c_str(),"RECREATE");
    TH1D *histo_full = new TH1D("full","Hit time distribution for all OM, full run", int(100*TDC_MAX), 0.0, TDC_MAX); // 100 bin per second
    std::cout << "Full Run Hit Time Distribution ...\n";
    for (int ientries = 0; ientries < entries; ientries++) {
      input_tree->GetEntry(ientries);
      // Fill the histogram with hit time distribution in seconds
      histo_full->Fill(tdc_ns * 1e-9);
    }
    histo_full->Write();

    // The hit time distribution per OM  (commented block below) should work but takes a long time
    // and is not useful for the small runs we had as statistics are low

    
    /*
    std::cout << "Hit Time Distribution per OM ...\n";
    for (int ref_slot = 0; ref_slot < 20; ref_slot++) {
      std::cout << "Board : " << ref_slot << "\n";
      for (int ref_channel = 0; ref_channel < 13; ref_channel++) {

	std::string ref_cube = std::to_string(ref_slot) + "." + std::to_string(ref_channel);
	std::string histo_name = ref_cube;
	std::string histo_title = "Hit time distribution, OM : " + ref_cube;
	TH1D *histo = new TH1D(histo_name.c_str(), histo_title.c_str(), int(TDC_MAX/10), 0.0, TDC_MAX); // 1 bin per 10 second

	for(int ientries = 0; ientries < entries; ientries++) {
	  input_tree->GetEntry(ientries);
	  if (slot_id == ref_slot and channel_id == ref_channel) histo->Fill(tdc_ns * 1e-6);
	}

	histo->Write();
      }
      }*/
    input_file->Close();
    output_file->Close();
    
  } catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  snfee::terminate();
  return (error_code);
}

      
