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
  // Criosstalk window (in ns)
  double ctWindow;
  // Output list file
  std::string output_listname;
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
      
      ("ct-window,c",
       po::value<double>(&app_params.ctWindow)
       ->value_name("number"),
       "set the CT window value (in ns)")
      
      ("output-list,l",
       po::value<std::string>(&app_params.output_listname)
       ->value_name("path"),
       "add the output hit list filename")

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
    
    Int_t hit_number;
    UInt_t slot_id, channel_id;
    UShort_t HT_flag;
    Double_t tdc_ns;
    Int_t raw_charge;
    
    input_tree->SetBranchAddress("hit_number",&hit_number);
    input_tree->SetBranchAddress("slot_id",&slot_id);
    input_tree->SetBranchAddress("channel_id",&channel_id);
    input_tree->SetBranchAddress("HT_flag",&HT_flag);    
    input_tree->SetBranchAddress("tdc_ns",&tdc_ns);
    input_tree->SetBranchAddress("raw_charge",&raw_charge);
    int entries = input_tree->GetEntries();

    // Creating output file
    TFile *output_file = new TFile(app_params.output_filename.c_str(),"RECREATE");

    std::ofstream hit_list;
    hit_list.open(app_params.output_listname.c_str(), std::ofstream::out);
    DT_THROW_IF(!hit_list.is_open(), std::logic_error, "Could not open output hit list file!");

    
    // Creating coincidence histo for each OM
    for (int ref_slot = 0; ref_slot < 20; ref_slot++) {
      std::cout << "Board : " << ref_slot << "\n";
      for (int ref_channel = 0; ref_channel < 12; ref_channel++) {
	std::cout << "\t" << ref_channel << "/13\n";

	// hit list store registered coincidences per OM and per reference hit (see README)
	hit_list << "===== " << std::to_string(ref_slot) << "." << std::to_string(ref_channel) << " ===== +/- " << app_params.ctWindow << " ns. =====\n";
	
	// Creating output histo
	std::string ref_cube = std::to_string(ref_slot) + "." + std::to_string(ref_channel);
	// cth_ histo store the number of coincidences per OM
	// ctp_ histo store the probability of coincidences per OM
	std::string output_histo_hits_name = "cth_" + ref_cube;
	std::string output_histo_hits_title = "Coincidence hits on ref cube : " + ref_cube + ", CT-window : +/- " + std::to_string(app_params.ctWindow);
	TH2D *output_histo_hits = new TH2D(output_histo_hits_name.c_str(), output_histo_hits_title.c_str(), 20, 0.0, 20.0, 13, 0.0, 13.0);

	std::string output_histo_prob_name = "ctp_" + ref_cube;
	std::string output_histo_prob_title = "Coincidence probabilities on ref cube : " + ref_cube + ", CT-window : +/- " + std::to_string(app_params.ctWindow);
	TH2D *output_histo_prob = new TH2D(output_histo_prob_name.c_str(), output_histo_prob_title.c_str(), 20, 0.0, 20.0, 13, 0.0, 13.0);

	// Used to locate the SAMLONG neigbhour OM
	int other_channel;
	if (ref_channel % 2 == 0) {
	  other_channel = ref_channel + 1;
	} else {
	  other_channel = ref_channel - 1;
	}

	// To study the charge ratio between reference and coincidence hit
	// On same SAMLONG
	std::string charge_ratio_samlong_name = "charge_ratio_" + ref_cube + "_" + std::to_string(ref_slot) + "." + std::to_string(other_channel) ;
	std::string charge_ratio_samlong_title = "samlong hit charge / reference hit charge ratio, reference cube : " + ref_cube + " ct window : " + std::to_string(app_params.ctWindow);
	TH1D *histo_charge_samlong = new TH1D(charge_ratio_samlong_name.c_str(), charge_ratio_samlong_title.c_str(),5000,0.0,10.0); 
	// For the other three closest neigbhours
	std::string charge_ratio_neighbour_name = "charge_ratio_" + ref_cube + "_" + std::to_string(ref_slot - 1) + "." + std::to_string(ref_channel);
	std::string charge_ratio_neighbour_title = "neighbour hit charge / reference hit charge ratio, reference cube : " + ref_cube + " ct window : " + std::to_string(app_params.ctWindow);
	TH1D  *histo_charge_neighbour = new TH1D(charge_ratio_neighbour_name.c_str(), charge_ratio_neighbour_title.c_str(),5000,0.0,10.0); 
	// For the rest of the wall
	std::string charge_ratio_other_name = "charge_ratio_other" + std::to_string(ref_slot) + "." + std::to_string(ref_channel);
	std::string charge_ratio_other_title = "other OM hit charge / reference hit charge ratio, reference cube : " + ref_cube + " ct window : " + std::to_string(app_params.ctWindow);
	TH1D  *histo_charge_other = new TH1D(charge_ratio_other_name.c_str(), charge_ratio_other_title.c_str(),5000,0.0,10.0); 

	// store the charge ratio 
	int charge_q1, charge_q2, charge_q3, charge_q4;

	// Initialize number of hits an coincidences at 0
	int totalHits[260];
	int coincHits[260];
	for (int i = 0; i < 260; i++) {
	  totalHits[i] = 0;
	  coincHits[i] = 0;
	}

	double TDC_max = 0;

	// Over the entire tree
	for (int ientries = 0; ientries < entries; ientries ++) {
	  
	  input_tree->GetEntry(ientries);
	  if (tdc_ns > TDC_max) TDC_max = tdc_ns; // Used to get the maximum time of events

	  // Get the total number of hits per OM
	  // slot_id -> column, channel_i -> row
	  totalHits[slot_id * 13 + channel_id] += 1; 

	  // if hit on reference cube 
	  if (slot_id == ref_slot and channel_id == ref_channel) {
	   
	    charge_q1 = raw_charge;
	    // hit_list is used to extract waveform (see WaveformDrawer)
	    hit_list << hit_number << "\t" << slot_id << "\t" << channel_id << "\t" << tdc_ns << "\t" << raw_charge << "\n"; 
	    double TDC = tdc_ns;

	    // Checking the coincidence window forward
	    for (int ifollow = (ientries + 1); ifollow < entries; ifollow++) {
	      input_tree->GetEntry(ifollow);
	      
	      // if not on the reference cube and in the coincidence window
	      if (slot_id != ref_slot or channel_id != ref_channel) {
		if (tdc_ns < TDC + app_params.ctWindow) {
		  coincHits[slot_id * 13 + channel_id] += 1; 
	
		  if (ref_channel % 2 == 0) { // Then SAMLONG neigbhour is above
		    if (slot_id == ref_slot and channel_id == (ref_channel + 1)) { // look at the OM on the same SAMLONG
		      charge_q2 = raw_charge;
		      Double_t division = (double)charge_q2/ (double)charge_q1;
		      bool test = (division != division);                // Check if NaN
		      if (!test) histo_charge_samlong->Fill(division);   // if not, fill ratio histogram
		    }
		    // sum over all closest neighbours except OM on the same SAMLONG
		    bool cube = false;
		    if (slot_id == (ref_slot - 1) and channel_id == ref_channel)       cube = true;
		    if (slot_id == (ref_slot + 1) and channel_id == ref_channel)       cube = true;
		    if (slot_id == ref_slot       and channel_id == (ref_channel - 1)) cube = true;
		    if (cube) {
		      charge_q3 = raw_charge;
		      Double_t division = (double) charge_q3 / (double)charge_q1;
		      bool test = (division != division);
		      if (!test) histo_charge_neighbour->Fill(division);  
		    }
		    if(!cube) { // A CHANGER, COMPTE LE MODULE OPTIQUE SUR LA MEME SAMLONG
		      charge_q4 = raw_charge;
		      Double_t division = double(charge_q4) / double(charge_q1);
		      bool test = (division != division);
		      if(!test) histo_charge_other->Fill(division);
		    }
		  }

		  if (ref_channel % 2 != 0) { // then SAMLONG neigbhours is below
		    if (slot_id == ref_slot and channel_id == (ref_channel - 1)) {
		      charge_q2 = raw_charge;
		      Double_t division = (double)charge_q2 / (double)charge_q1;
		      bool test = (division != division);
		      if (!test) histo_charge_samlong->Fill(division);
		    }
		    // sum over all neighbour except OM on the same SAMLONG
		    bool cube = false;
		    if (slot_id == (ref_slot - 1) and channel_id == ref_channel)       cube = true;
		    if (slot_id == (ref_slot + 1) and channel_id == ref_channel)       cube = true;
		    if (slot_id == ref_slot       and channel_id == (ref_channel + 1)) cube = true;
		    if (cube) {
		      charge_q3 = raw_charge;
		      Double_t division = (double)charge_q3 / (double)charge_q1;
		      bool test = (division != division);
		      if (!test) histo_charge_neighbour->Fill(division);
		    }
		    if(!cube) {
		      charge_q4 = raw_charge;
		      Double_t division = double(charge_q4) / double(charge_q1);
		      bool test = (division != division);
		      if(!test) histo_charge_other->Fill(division);
		    }
		  }
		  
		  hit_list << hit_number << "\t" << slot_id << "\t" << channel_id << "\t" << tdc_ns << "\t" << raw_charge << "\n";
		} // end coincidence window condition
		// stop if outside of coincidence window
		if (tdc_ns > TDC + app_params.ctWindow) {
		  break;
		}
	      }// end not in reference cube condition
	    } // end ifollow loop

	    // Next was used to check coincidence window backward
	    
	    /*	    for (int ipast = (ientries - 1); ipast > 0; ipast--) {
	      input_tree->GetEntry(ipast);
	      
	      // if not on the reference cube and in the coincidence window
	      if (slot_id != ref_slot or channel_id != ref_channel) {                   // Si le hit d'avant n'est PAS sur le cube de reference
		if (tdc_ns > TDC - app_params.ctWindow) {                               // Si il est dans la fenetre de coincidence
		  coincHits[slot_id * 13 + channel_id] += 1;                            // On ajoute 1 au cube ou il se trouve
		  
		  if (ref_channel % 2 == 0) {
		    if (slot_id == ref_slot and channel_id == (ref_channel + 1)) {
		      charge_q2 = raw_charge;
		      Double_t division = (double)charge_q2/ (double)charge_q1;
		      bool test = (division != division);
		      if (!test) histo_charge_samlong->Fill(division);
		    }
		    // sum over all neighbour except OM on the same SAMLONG
		    bool cube = false;
		    if (slot_id == (ref_slot - 1) and channel_id == ref_channel)       cube = true;
		    if (slot_id == (ref_slot + 1) and channel_id == ref_channel)       cube = true;
		    if (slot_id == ref_slot       and channel_id == (ref_channel - 1)) cube = true;
		    if (cube) {
		      charge_q3 = raw_charge;
		      Double_t division = (double) charge_q3 / (double)charge_q1;
		      bool test = (division != division);
		      if (!test) histo_charge_neighbour->Fill(division);
		    }
		    if(!cube) {
		      charge_q4 = raw_charge;
		      Double_t division = double(charge_q4) / double(charge_q1);
		      bool test = (division != division);
		      if(!test) histo_charge_other->Fill(division);
		    }
		  } // end channel even condition

		  if (ref_channel % 2 != 0) {                                           // si le numero de channel du cube de reference est impair (ex 12.5)
		    if (slot_id == ref_slot and channel_id == (ref_channel - 1)) {    
		      charge_q2 = raw_charge;
		      Double_t division = (double)charge_q2 / (double)charge_q1;
		      bool test = (division != division);
		      if (!test) histo_charge_samlong->Fill(division);
		    }
		    // sum over all neighbour except OM on the same SAMLONG
		    bool cube = false;
		    if (slot_id == (ref_slot - 1) and channel_id == ref_channel)       cube = true;
		    if (slot_id == (ref_slot + 1) and channel_id == ref_channel)       cube = true;
		    if (slot_id == ref_slot       and channel_id == (ref_channel + 1)) cube = true;
		    if (cube) {                                     // Ici on prend le voisin de gauche
		      charge_q3 = raw_charge;
		      Double_t division = (double)charge_q3 / (double)charge_q1; 
		      bool test = (division != division);
		      if (!test) histo_charge_neighbour->Fill(division);
		    }
		    if(!cube) {
		      charge_q4 = raw_charge;
		      Double_t division = double(charge_q4) / double(charge_q1);
		      bool test = (division != division);
		      if(!test) histo_charge_other->Fill(division);
		    }
		  } // end channel odd condition
		                                                                        // La on sort quelques donnees dans le fichier list
		  hit_list << hit_number << "\t" << slot_id << "\t" << channel_id << "\t" << tdc_ns << "\t" << raw_charge << "\n";
		} // end coincidence window condtion
		
		// stop if outside of coincidence window
		if (tdc_ns < TDC - app_params.ctWindow) {
		   break;
		}
	      } // end not in reference cube condition
	      } // end ipast condition*/
	  } // end if in reference cube condition
	} // end of loop on tree entries
	hit_list << "\n";

	// The reference OM value is the total number of hits in that OM
	// to differentiate it from OMs with 0 coincidences
	coincHits[ref_slot * 13 + ref_channel] = totalHits[ref_slot * 13 + ref_channel];
	
	for (int ipos = 0; ipos < 260; ipos++) {
	  
	  int slot = ipos / 13;
	  int channel = ipos - slot * 13;
	  
	  double prob =  (double) coincHits[ipos] / (double) totalHits[ipos]; //coincidence probability is computed
	  
	  if (slot == ref_slot and channel == ref_channel) { // Probability set to 1 if reference cube
	    prob = 1.0;
	  }
	  if (prob!=prob) { // else initialized at  0
	    prob = 0.0;
	  }
	  output_histo_hits->SetBinContent(slot + 1, channel + 1, coincHits[ipos]);
	  output_histo_prob->SetBinContent(slot + 1, channel + 1, prob);
	}

	output_histo_hits->Write();
	
	output_histo_prob->SetMarkerSize(0.8);
	
	output_histo_prob->Write();

	
	int charge_samlong_entries = histo_charge_samlong->GetEntries();
	int charge_neighbour_entries = histo_charge_neighbour->GetEntries();
	int charge_other_entries = histo_charge_other->GetEntries();

	std::cout << "Entries in saml = " << charge_samlong_entries << "\n";
	
	//histo_charge_samlong->Scale(1/double(charge_samlong_entries));
	//histo_charge_neighbour->Scale(1/double(charge_neighbour_entries));
	//histo_charge_other->Scale(1/double(charge_other_entries));
	histo_charge_samlong->Write();
	histo_charge_neighbour->Write();
	histo_charge_other->Write();
	
      } // end of loop on channels
    } // end of loop on slots
    
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

      
