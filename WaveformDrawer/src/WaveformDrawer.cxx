// Standard library :
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <exception>

// Third party :
// - Bayeux
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/io_factory.h>
// - Boost
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
// - ROOT
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TH1D.h>


struct app_params_type
{

  /// Input file
  std::string input_filename;

  /// Output file
  std::string output_filename;
  
  /// List of hits to draw
  std::string input_hit_list;
  
  /// Time limits in ns
  double TDC_min = -1;
  double TDC_max = -1;

};

int main(int argc_, char** argv_) 
{
  int error_code = EXIT_SUCCESS;
  try {
    
    // Configuration
    app_params_type app_params;

    // Parse options :
    namespace po = boost::program_options;
    po::options_description opts("Allowed options");
    opts.add_options()

      ("help","produce help message")

      ("input-file,i",
       po::value<std::string>(&app_params.input_filename)
       ->value_name("path"),
       "Set ROOT input file path ad name")

      ("output-file,o",
       po::value<std::string>(&app_params.output_filename)
       ->value_name("path"),
       "Set ROOT output file path and name")
   
      ("input-list,l",
       po::value<std::string>(&app_params.input_hit_list)->default_value("no_list")
       ->value_name("path"),
       "Set path and name of a file containing a list of hits to draw")
      
      ("min-tdc,m",
       po::value<double>(&app_params.TDC_min)->default_value(-1)
       ->value_name("number"),
       "Set minimum time for hits of interest.")

      ("max-tdc,M",
       po::value<double>(&app_params.TDC_max)->default_value(-1)
       ->value_name("number"),
       "Set maximum time for hits of interest.")

      ; //end of options descriptions

    // Describe command line arguments :
    po::variables_map vm;
    po::store(po::command_line_parser(argc_,argv_)
	      .options(opts)
	      .run(), vm);
    po::notify(vm);

    // Use command line arguments :
    if (vm.count("help")) {
      std::cout << opts << std::endl;
      return(-1);
    }

    // Use command line arguments
    std::vector<int> hit_list;
    if (!app_params.input_hit_list.empty() and app_params.input_hit_list != "no_list") {
      std::ifstream finput_list(app_params.input_hit_list.c_str());
      DT_THROW_IF(!finput_list, std::logic_error,"Cannot open hit list file");
      while (finput_list and !finput_list.eof()) {
	std::string line;
	std::getline(finput_list, line);
	boost::trim_copy(line);
	if (line.empty()) {
	  continue;
	}
	std::istringstream ins(line);
	std::string hit_number;
	ins >> hit_number >> std::ws;
	if (!hit_number.empty() and hit_number[0] !='#') {
	  hit_list.push_back(std::stoi(hit_number));
	}
      }
    }
    DT_THROW_IF(app_params.input_hit_list.empty(), std::logic_error, "Input hit list is empty!");
    
    DT_THROW_IF(app_params.TDC_min > app_params.TDC_max, std::logic_error, "Invalid Time Limits!");
    std::cout << "\nSelecting hits with TDC > " << app_params.TDC_min << " and < " << app_params.TDC_max << "\n\n";


    // Use command line arguments
    TFile *input_file = new TFile(app_params.input_filename.c_str(),"READ");
    TTree *input_tree = (TTree *)input_file->Get("Calo_Hits");

    TFile *output_file = new TFile(app_params.output_filename.c_str(),"RECREATE");
    
    int entries = input_tree->GetEntries();

    Int_t hit_number;
    UInt_t slot_id, channel_id;
    Double_t tdc_ns;
    Int_t waveform_samples[1024];
    
    input_tree->SetBranchAddress("hit_number",&hit_number);
    input_tree->SetBranchAddress("slot_id",&slot_id);
    input_tree->SetBranchAddress("channel_id",&channel_id);
    input_tree->SetBranchAddress("tdc_ns",&tdc_ns);
    input_tree->SetBranchAddress("waveform_samples",&waveform_samples);
  
    if (app_params.input_hit_list == "no_list") {
      std::cout << "\nNo input list given \n";

      for (int ientries = 0; ientries < entries; ientries ++) {

	if (ientries % 1000000 == 0) {
	  std::cout << ientries << "/" << entries << "\n";
	}

	input_tree->GetEntry(ientries);

	bool selected_hit = true;
	if (app_params.TDC_min != -1 and tdc_ns < app_params.TDC_min) {
	  selected_hit = false;
	  //std::cout << "Selecting hits with TDC > " << app_params.TDC_min << "\n";
	}
	if (app_params.TDC_max != -1 and tdc_ns > app_params.TDC_max) {
	  selected_hit = false;
	  //std::cout << "Selecting hits with TDC < " << app_params.TDC_max << "\n";
	}
	
	if (selected_hit) {
	  std::string output_histo_name = "hit_" + std::to_string(hit_number);
	  std::string output_histo_title = "Waveform hit number : " + std::to_string(hit_number);
	  TH1D *output_histo = new TH1D(output_histo_name.c_str(),output_histo_title.c_str(), 1024, 0.0, 1024.0);
	  
	  for (int isample = 0; isample < 1024; isample++) {
	    output_histo->SetBinContent(isample+1, waveform_samples[isample]);
	  }
	  output_histo->Write();
	}
      }
          
    } else {
      std::cout << "\nInput list given \n";
      
      for (int ientries = 0; ientries < entries; ientries++) {

	if (ientries % 1000 == 0 ) {
	  std::cout << ientries << "/" << entries << "\n";
	}
	
	input_tree->GetEntry(ientries);
	
	bool selected_hit = true;
	if (app_params.TDC_min != -1 and tdc_ns < app_params.TDC_min) {
	  selected_hit = false;
	  //std::cout << "Selecting hits with TDC > " << app_params.TDC_min << "\n";
	}
	if (app_params.TDC_max != -1 and tdc_ns > app_params.TDC_max) {
	  selected_hit = false;
	  //std::cout << "Selecting hits with TDC < " << app_params.TDC_max << "\n";
	}

	if (selected_hit) {
	  if (std::find(hit_list.begin(), hit_list.end(), hit_number) != hit_list.end() ) {
	    
	    std::string output_histo_name = "hit_" + std::to_string(hit_number);
	    std::string output_histo_title = "Waveform hit number : " + std::to_string(hit_number);
	    TH1D *output_histo = new TH1D(output_histo_name.c_str(),output_histo_title.c_str(), 1024, 0.0, 1024.0);
	    
	    for (int isample = 0; isample < 1024; isample++) {
	      output_histo->SetBinContent(isample+1, waveform_samples[isample]);
	    }
	    output_histo->Write();
	  }
	}
      }
    }




    input_file->Close();
    output_file->Close();
    
  } catch (std::exception & x) {
    std::cerr << "error : " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error : unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return 0;
}
