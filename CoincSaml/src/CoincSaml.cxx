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
  // Output list file
  std::string output_listname;
  // Coincidence window (in ns)
  // fixed: when a hit is registered on an OM the Samlong chip opens a 400 ns asymetrical sampling window
  double ctWindow_low  = 125; //275 for run 100, 125 for the others etc.
  double ctWindow_high = 275; // 125 for run 100, 275 for the others etc.
 
};

// Main program
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
  
      ("output-list,o",
       po::value<std::string>(&app_params.output_listname)
       ->value_name("path"),
       "add a root output list file")
      
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
    UInt_t lt_trig_count;
    Int_t hit_number;
    input_tree->SetBranchAddress("hit_number",&hit_number);
    input_tree->SetBranchAddress("slot_id",&slot_id);
    input_tree->SetBranchAddress("channel_id",&channel_id);
    input_tree->SetBranchAddress("tdc_ns",&tdc_ns);
    input_tree->SetBranchAddress("lt_trig_count",&lt_trig_count);
    int entries = input_tree->GetEntries();

    // Creating hit_list file (same as Coincidence.cxx)
    std::ofstream hit_list;
    
    hit_list.open(app_params.output_listname.c_str());
    DT_THROW_IF(!hit_list.is_open(), std::logic_error, "Could not open hit list file");
      

    // Creating output file
    TFile *output_file = new TFile(app_params.output_filename.c_str(),"RECREATE");

    double tdc_max = 0;               // Run duration
    int totalHits[260] = {0};         // Registered hits number per OM
    double triggerRate[260] = {0};    // Trigger Rate per OM
    int measured_coinc[260] = {0};    // Number of measured coincidences per OM 
    double expected_coinc[260] = {0}; // Number of calculated coincidences per OM
    
    // TRIGGER RATE
    // used to compute the number of expected fortuitous coincidences and the error on that value
    std::cout << "Calculating trigger rate per OM ... new way \n";
    for (int ientries = 0; ientries < entries; ientries++) {
      input_tree->GetEntry(ientries);
      if (tdc_ns > tdc_max) tdc_max = tdc_ns;                // Get total run duration
      totalHits[slot_id * 13 + channel_id] += 1;             // Count registered hit
      totalHits[slot_id * 13 + channel_id] += lt_trig_count; // Count not registered hits
    }
    for (int ipos = 0; ipos < 260; ipos++) {
      triggerRate[ipos] = (double)totalHits[ipos] / tdc_max;
    }
    
    //REGISTERED COINCIDENCES
    std::cout << "Getting registered coincidences ...\n";
    for (int ref_slot = 0; ref_slot < 20; ref_slot++) {
      std::cout << "Board : " << ref_slot << "/20\n";
      for (int ref_channel = 0; ref_channel < 13; ref_channel++) {

	hit_list << "\n##### " << "Ref cube : " << ref_slot << "." << ref_channel << " #####\n";
	// Getting registered coincidence number for electronical (SAMLONG) neighbour of reference cube
	double TDC;
	int slot = ref_slot;
	int channel;
	int coinc = 0;
	if (ref_channel % 2 == 0) {
	  channel = ref_channel + 1;
	} else {
	  channel = ref_channel - 1;
	}
	for (int ientries = 0; ientries < entries; ientries++) {
	  input_tree->GetEntry(ientries);
	  
	  if (slot_id == ref_slot and channel_id == ref_channel) {       // If hit in ref cube
 	    TDC = tdc_ns;                                                // store time
	    hit_list << "\n" << hit_number << "\n";
	    for (int ipast = (ientries - 1); ipast > 0; ipast--) {	 // looking at past hits
	      input_tree->GetEntry(ipast);
	      
	      if (slot_id == slot and channel_id == channel) {           // If hit on the samlong neighbour
		if (tdc_ns > (TDC - app_params.ctWindow_low)) {          // If coincidence condition is validated
		  measured_coinc[ref_slot * 13 + ref_channel] += 1;      // increment array of registered coincidences
		  hit_list << hit_number << "\n";                        // storing coincidence hit number
        	} 
	      }
	      if (tdc_ns < (TDC - app_params.ctWindow_low)) {            // Since the root tree is sorted, if we get out
		break;                                                   // of the coincidence window we can break
	      }
	    }
	    for (int ifollow = (ientries + 1); ifollow < entries; ifollow++) {  // Idem for hits arriving after the one on the ref cube
	      input_tree->GetEntry(ifollow);
	      
	      if (slot_id == slot and channel_id == channel) {
		if (tdc_ns < (TDC + app_params.ctWindow_high)) {
		  measured_coinc[ref_slot * 13 + ref_channel] += 1;
		  hit_list << hit_number << "\n";                  
		} 
	      }
	      if (tdc_ns > (TDC + app_params.ctWindow_high)) {
		break;
	      }
	    } 
	  }
	} // end of loop on entries
      } // end of loop on channel
    } // end of loop on slot
    
    double expected_error[260] = {0};
    
    // CALCULATED COINCIDENCES AND ERROR
    std::cout << "Calculating expected coincidence number ... \n";
    for (int ref_slot = 0; ref_slot < 20; ref_slot++) {
      for (int ref_channel = 0; ref_channel < 13; ref_channel++ ) {
	// ref_slot and ref_channel are the coordinates of the reference cube, we want the number in
	// the electronically linked OM
	int slot = ref_slot;
	int channel;
	if (ref_channel % 2 == 0) {
	  channel = ref_channel + 1;
	} else {
	  channel = ref_channel - 1;
	}
	if (ref_channel != 12) {
	  // number of expected coincidence = window * trigger rate ref cube * trigger rate OM on same samlong * run duration
	  expected_coinc[ref_slot * 13 + ref_channel] = (app_params.ctWindow_low + app_params.ctWindow_high) * triggerRate[ref_slot * 13 + ref_channel] 
	    * triggerRate[slot * 13 + channel] * tdc_max;
	  
	  // a, b, c parameters of the relative error
	  double a = (app_params.ctWindow_low + app_params.ctWindow_high) / tdc_max;
	  double b = sqrt((double)totalHits[ref_slot * 13 + ref_channel] * totalHits[slot * 13 + channel]);
	  double c = sqrt((double)totalHits[ref_slot * 13 + ref_channel]) + sqrt((double)totalHits[slot * 13 + channel]);

	  expected_error[ref_slot * 13 + ref_channel] = expected_coinc[ref_slot * 13 + ref_channel] * a * b * c;
	} else {
	  expected_coinc[ref_slot * 13 + ref_channel] = 0;
	  expected_error[ref_slot * 13 + ref_channel] = 0;
	}
      }
    }
    // Creating output histograms
    TH1D *measured = new TH1D("measured","Measured number of coincidences in the electronically linked OM", 260, 0.0, 260.0);
    TH1D *expected = new TH1D("expected","Expected number of coincidences in the electronically linked OM", 260, 0.0, 260.0);
    // Filling output histograms
    for (int ipos = 0; ipos < 260; ipos++) {
      measured->SetBinContent(ipos + 1, (double)measured_coinc[ipos]);
      double measured_error = sqrt((double)measured_coinc[ipos]);
      measured->SetBinError(ipos + 1, measured_error);

      expected->SetBinContent(ipos+1, expected_coinc[ipos]);
      expected->SetBinError(ipos+1, expected_error[ipos]);
    }

    // histograms formatting
    for (int ipos = 0; ipos < 260; ipos++) {
      int slot = ipos / 13;
      int channel = ipos - slot * 13;
      std::string ref_cube = std::to_string(slot) + "." + std::to_string(channel);
      
      measured->GetXaxis()->SetBinLabel(ipos + 1, ref_cube.c_str());
      measured->GetXaxis()->SetTitle("Reference cube label");
      measured->GetYaxis()->SetTitle("Coincidence counts");
      expected->GetXaxis()->SetBinLabel(ipos + 1, ref_cube.c_str());
      expected->GetXaxis()->SetTitle("Reference cube label");
      expected->GetYaxis()->SetTitle("Coincidence counts");
    }
    
    measured->Write();
    expected->Write();
    hit_list.close();
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

      
