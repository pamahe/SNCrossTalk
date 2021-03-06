// Standard library :
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Third party :
// - Bayeux
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/io_factory.h>
// - Boost
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_real.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/phoenix/phoenix.hpp>
// - ROOT
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

// \brief Application configuration parameters
struct app_params_type 
{

  /// Input files
  std::string input_listname;                // name of the .list file containing the list of CRD files
  std::vector<std::string> input_filenames;  // used later to store CRD files names

  /// Output file
  std::string output_filename;

  /// Max CRD records
  int max_crd = 0;      // if different than 0, crd2root will stop after processing max_crd events

  /// Process conditions
  bool process_lt;        // only events with LT = 1
  bool process_ht;        // only events with HT = 1
  bool process_full;      // add branches to output root file (see README) 


  /// 
  int crd_counter_period = 100000; // use to display progress
  
};

int main(int argc_, char** argv_) {

  int error_code = EXIT_SUCCESS;
  try {

    // Configuration
    app_params_type app_params;
    
    // Parse options :
    namespace po = boost::program_options;
    po::options_description opts("Allowed options");
    opts.add_options()

      ("help","produce help message")

      ("input-list,I",
       po::value<std::string>(&app_params.input_listname)
       ->value_name("path"),
       "set filename containing the list of CRD input files")

      ("output-file,o",
       po::value<std::string>(&app_params.output_filename)
       ->value_name("path"),
       "set ROOT output file name")
      
      ("low-threshold,l",
       po::value<bool>(&app_params.process_lt)->zero_tokens()->default_value(false),
       "process only LT hits")
      
      ("high-threshold,h",
       po::value<bool>(&app_params.process_ht)->zero_tokens()->default_value(false),
       "process only  hits")

      ("process-full,l",
       po::value<bool>(&app_params.process_full)->zero_tokens()->default_value(false),
       "process all data from calo hits")

      ("max-crd,m",
       po::value<int>(&app_params.max_crd)->default_value(0)
       ->value_name("number"),
      "maximum number of crd records to read")
      
      ; // end of options descriptions
    
    // Describe command line arguments
    po::variables_map vm;
    po::store(po::command_line_parser(argc_,argv_)
	      .options(opts)
	      .run(), vm);
    po::notify(vm);

    // Use command line arguments :
    if (vm.count("help")) {
      std::cout << "crd2root : " 
		<< "Read commisionning raw data (CRD) and extract calo hits informations in a ROOT tree"
		<< std::endl << std::endl;
      std::cout << "Usage :" << std::endl << std::endl; 
      std::cout << " crd2root [OPTIONS]" << std::endl << std::endl;
      std::cout << opts << std::endl;
      std::cout << "Example :" << std::endl << std::endl;
      std::cout << "  Extract data from a list of CRD files :" << std::endl << std::endl;
      std::cout << "  ./_install.d/crd2root \\\n";
      std::cout << "    --input-list /sps/nemo/snemo/snemo-data/raw-data/RHD/run_100/snemo_run-100_crd_files.lis \\\n";
      std::cout << "    --output-file \"{YOUR_OUTPUT_PATH}/run_100.root \\\n";
      std::cout << "    --high-threshold \\\n";
      std::cout << "    --process-full \\\n";
      std::cout << std::endl << std::endl;
      return(-1);
    }

    // Getting input filenames
    // Open input_listname file and extract CRD files names
    if (!app_params.input_listname.empty()) {
      // Read a file containing a list of input filenames
      std::string listname = app_params.input_listname;
      std::ifstream finput_list(listname);
      DT_THROW_IF(!finput_list, std::logic_error, "Cannot open input files list.");
      while (finput_list and !finput_list.eof()) {
	std::string line;
	std::getline(finput_list,line);
	boost::trim_copy(line);
	if (line.empty()) {
	  continue;
	}
	std::istringstream ins(line);
	std::string filename;
	ins >> filename >> std::ws;
	if (!filename.empty() and filename[0]!='#') {
	  app_params.input_filenames.push_back(filename);
	}
      }
    }
    DT_THROW_IF(app_params.input_listname.empty(),
		std::logic_error, "Missing input files list!");

    // Creating output ROOT file
    TFile *output_file = new TFile(app_params.output_filename.c_str(),"RECREATE");
    TTree *output_tree = new TTree("Calo_Hits","Calorimeter hits data");
    
    Int_t hit_number_out;
    Int_t trigger_id_out;
    
    UInt_t slot_id_out, channel_id_out;
    UShort_t lto_flag_out, ht_flag_out; ///< Boolean
    UInt_t event_id_out;
    ULong64_t raw_tdc_out;
    Double_t tdc_ns_out; ///< Software computed, not raw data
    
    UInt_t lt_trig_count_out, lt_time_count_out;
    
    Int_t raw_baseline_out;
    Double_t baseline_volt_out;  ///< Software computed, not raw data
    
    Short_t raw_peak_out;
    UShort_t peak_cell_out;
    Double_t peak_volt_out;  ///< Software computed, not raw data
    
    Int_t raw_charge_out;
    Double_t charge_picocoulomb_out;  ///< Software computed, not raw data
    Short_t charge_overflow_out; ///< Boolean
    
    UInt_t rising_cell_out;
    UInt_t rising_offset_out;
    Double_t rising_ns_out;  ///< Software computed, not raw data
    
    UInt_t falling_cell_out;
    UInt_t falling_offset_out;
    Double_t falling_ns_out;  ///< Software computed, not raw data
    
    
    UShort_t fcr_out;
    Double_t unix_time_out;  ///< Software computed, not raw data
    
    Int_t waveform_samples_out[1024];

    
    output_tree->Branch("hit_number",&hit_number_out,"hit_number_out/I");
    output_tree->Branch("trigger_id",&trigger_id_out,"trigger_id_out/I");

    output_tree->Branch("event_id",&event_id_out,"event_id_out/i");

    output_tree->Branch("slot_id",&slot_id_out,"slot_id_out/i");
    output_tree->Branch("channel_id",&channel_id_out,"channel_id_out/i");
    output_tree->Branch("LT_flag",&lto_flag_out,"lto_flag_out/s");
    output_tree->Branch("HT_flag",&ht_flag_out,"ht_flag_out/s");
   
    output_tree->Branch("raw_tdc",&raw_tdc_out,"raw_tdc_out/l");
    output_tree->Branch("tdc_ns",&tdc_ns_out,"tdc_ns_out/D");
    
    output_tree->Branch("lt_trig_count",&lt_trig_count_out,"lt_trig_count_out/i");
    output_tree->Branch("lt_time_count",&lt_time_count_out,"lt_time_count_out/i");
      

    if (app_params.process_full) { //Allow for faster work if just position and time are needed -> !app_params.process_full
      
      output_tree->Branch("raw_baseline",&raw_baseline_out,"raw_baseline_out/I");
      output_tree->Branch("baseline_volt",&baseline_volt_out,"baseline_volt_out/D");
      
      output_tree->Branch("raw_peak",&raw_peak_out,"raw_peak_out/S");
      output_tree->Branch("peak_cell",&peak_cell_out,"peak_cell_out/s");
      output_tree->Branch("peak_volt",&peak_volt_out,"peak_volt_out/D");
      
      output_tree->Branch("raw_charge",&raw_charge_out,"raw_charge_out/I");
      output_tree->Branch("charge_picocoulomb",&charge_picocoulomb_out,"charge_picocoulomb_out/D");
      output_tree->Branch("charge_overflow",&charge_overflow_out,"charge_overflow_out/S");
      
      output_tree->Branch("rising_cell",&rising_cell_out,"rising_cell_out/i");
      output_tree->Branch("rising_offset",&rising_offset_out,"rising_offset_out/i");
      output_tree->Branch("rising_ns",&rising_ns_out,"rising_ns_out/D");
      
      output_tree->Branch("falling_cell",&falling_cell_out,"falling_cell_out/i");
      output_tree->Branch("falling_offset",&falling_ns_out,"falling_ns_out/i");
      output_tree->Branch("falling_ns",&falling_ns_out,"falling_ns_out/D");
      
      output_tree->Branch("fcr",&fcr_out,"fcr_out_out/s");
      output_tree->Branch("unix_time",&unix_time_out,"unix_time_out/D");
      
      output_tree->Branch("waveform_samples",&waveform_samples_out,"waveform_samples_out[1024]/I");
    }

    // Input file loop
    bool end_of_input = false;
    int crd_record_counter = 0;
    for (int i_input_filename = 0;
	 i_input_filename < (int) app_params.input_filenames.size();
	 i_input_filename++) {
      
      std::ifstream input_file;
      input_file.open(app_params.input_filenames[i_input_filename]);
      
      DT_THROW_IF(!input_file.is_open(), std::logic_error, "Could not open CRD input file : " + app_params.input_filenames[i_input_filename]);
      
      std::string line;
      int line_counter = 0;
      int crd_record_counter_for_this_file = 0;

      while (getline(input_file,line)) {
	  
	// Lines 0 to 8 of CRD files are file header lines
	// Parsing hit header 
	if (line_counter - 3 * crd_record_counter_for_this_file == 9) {
	  namespace qi = boost::spirit::qi;
	  
	  std::string::const_iterator str_iter = line.begin();
	  std::string::const_iterator end_iter = line.end();
	  
	  int32_t hit_number = 0;
	  int32_t trigger_id = 0;
	  bool res = false;
	  res = qi::phrase_parse(str_iter,
				 end_iter,
				 // Begin grammar
				 (
				  qi::lit("=")
				  >> qi::lit("HIT")
				  >> qi::int_[boost::phoenix::ref(hit_number) = boost::spirit::qi::_1]
				  >> qi::lit("=")
				  >> qi::lit("CALO")
				  >> qi::lit("=")
				  >> qi::lit("TRIG_ID")
				  >> qi::int_[boost::phoenix::ref(trigger_id) = boost::spirit::qi::_1]
				  >> qi::lit("=")
				  ),
				 // End grammar
				 qi::space);
	  
	  // Filling ROOT values
	  hit_number_out = hit_number;
	  trigger_id_out = trigger_id;
	}
	// Parsing hit data
	if (line_counter - 3 * crd_record_counter_for_this_file == 10) {
	  namespace qi = boost::spirit::qi;

	  std::string::const_iterator str_iter = line.begin();
	  std::string::const_iterator end_iter = line.end();
	  
	  uint32_t slot_id = 0;
	  uint32_t channel_id = 0;
	  uint16_t lto_flag = 0;
	  uint16_t ht_flag = 0; ///< Boolean
	  uint32_t event_id = 0;
	  uint64_t raw_tdc = 0;
	  double tdc_ns = 0.; ///< Software computed, not raw data
	  uint32_t lt_trig_count = 0;
	  uint32_t lt_time_count = 0;
	  
	  int32_t raw_baseline = 0;
	  double baseline_volt = 0;  ///< Software computed, not raw data
	  
	  int16_t raw_peak = 0;
	  uint16_t peak_cell = 0;
	  double peak_volt = 0;  ///< Software computed, not raw data
	  
	  int32_t raw_charge = 0;
	  double charge_picocoulomb = 0;  ///< Software computed, not raw data
	  int16_t charge_overflow = 0; ///< Boolean
	  
	  uint32_t rising_cell = 0;
	  uint32_t rising_offset = 0;
	  double rising_ns = 0;  ///< Software computed, not raw data
	  
	  uint32_t falling_cell = 0;
	  uint32_t falling_offset = 0;
	  double falling_ns = 0;  ///< Software computed, not raw data
	  
	  uint16_t fcr = 0;
	  double unix_time = 0;  ///< Software computed, not raw data
	  bool res = false;
	  res = qi::phrase_parse(str_iter,
				 end_iter,
				 // Begin grammar
				 (
				  qi::lit("Slot")          >> qi::uint_[boost::phoenix::ref(slot_id) = boost::spirit::qi::_1]
				  >> qi::lit("Ch")            >> qi::uint_[boost::phoenix::ref(channel_id) = boost::spirit::qi::_1]
				  >> qi::lit("LTO")           >> qi::uint_[boost::phoenix::ref(lto_flag) = boost::spirit::qi::_1]
				  >> qi::lit("HT")            >> qi::uint_[boost::phoenix::ref(ht_flag) = boost::spirit::qi::_1]
				  >> qi::lit("EvtID")         >> qi::uint_[boost::phoenix::ref(event_id) = boost::spirit::qi::_1]
				  >> qi::lit("RawTDC")        >> qi::ulong_long[boost::phoenix::ref(raw_tdc) = boost::spirit::qi::_1]
				  >> qi::lit("TDC")           >> qi::double_[boost::phoenix::ref(tdc_ns) = boost::spirit::qi::_1]
				  >> qi::lit("TrigCount")     >> qi::uint_[boost::phoenix::ref(lt_trig_count) = boost::spirit::qi::_1]
				  >> qi::lit("Timecount")     >> qi::uint_[boost::phoenix::ref(lt_time_count) = boost::spirit::qi::_1]
				  >> qi::lit("RawBaseline")   >> qi::int_[boost::phoenix::ref(raw_baseline) = boost::spirit::qi::_1]
				  >> qi::lit("Baseline")      >> qi::double_[boost::phoenix::ref(baseline_volt) = boost::spirit::qi::_1]
				  >> qi::lit("RawPeak")       >> qi::int_[boost::phoenix::ref(raw_peak) = boost::spirit::qi::_1]
				  >> qi::lit("Peak")          >> qi::double_[boost::phoenix::ref(peak_volt) = boost::spirit::qi::_1]
				  >> qi::lit("PeakCell")      >> qi::uint_[boost::phoenix::ref(peak_cell) = boost::spirit::qi::_1]
				  >> qi::lit("RawCharge")     >> qi::int_[boost::phoenix::ref(raw_charge) = boost::spirit::qi::_1]
				  >> qi::lit("Charge")        >> qi::double_[boost::phoenix::ref(charge_picocoulomb) = boost::spirit::qi::_1]
				  >> qi::lit("Overflow")      >> qi::uint_[boost::phoenix::ref(charge_overflow) = boost::spirit::qi::_1]
				  >> qi::lit("RisingCell")    >> qi::uint_[boost::phoenix::ref(rising_cell) = boost::spirit::qi::_1]
				  >> qi::lit("RisingOffset")  >> qi::uint_[boost::phoenix::ref(rising_offset) = boost::spirit::qi::_1]
				  >> qi::lit("RisingTime")    >> qi::double_[boost::phoenix::ref(rising_ns) = boost::spirit::qi::_1]
				  >> qi::lit("FallingCell")   >> qi::uint_[boost::phoenix::ref(falling_cell) = boost::spirit::qi::_1]
				  >> qi::lit("FallingOffset") >> qi::uint_[boost::phoenix::ref(falling_offset) = boost::spirit::qi::_1]
				  >> qi::lit("FallingTime")   >> qi::double_[boost::phoenix::ref(falling_ns) = boost::spirit::qi::_1]
				  >> qi::lit("FCR")           >> qi::uint_[boost::phoenix::ref(fcr) = boost::spirit::qi::_1]
				  >> qi::lit("UnixTime")      >> qi::double_[boost::phoenix::ref(unix_time) = boost::spirit::qi::_1]
				  ),
				 // End grammar
				 qi::space);
	  
	  // Filling ROOT tree values 
	  event_id_out      = event_id;
	  slot_id_out       = slot_id;
	  channel_id_out    = channel_id;
	  
	  raw_tdc_out       = raw_tdc;
	  tdc_ns_out        = tdc_ns;
	  
	  lto_flag_out      = lto_flag;
	  ht_flag_out       = ht_flag;

	  lt_trig_count_out = lt_trig_count;
	  lt_time_count_out = lt_time_count;

	  
	  if (app_params.process_full) {
	    
	    raw_baseline_out  = raw_baseline;
	    baseline_volt_out = baseline_volt;
	    
	    raw_peak_out      = raw_peak;
	    peak_cell_out     = peak_cell;
	    peak_volt_out     = peak_volt;  
	    
	    raw_charge_out          = raw_charge;
	    charge_picocoulomb_out  = charge_picocoulomb;
	    charge_overflow_out     = charge_overflow;
	    
	    rising_cell_out    = rising_cell;
	    rising_offset_out  = rising_offset;
	    rising_ns_out      = rising_ns;  
	    
	    falling_cell_out    = falling_cell;
	    falling_offset_out  = falling_offset;
	    falling_ns_out      = falling_ns;  
	    
	    fcr_out   = fcr;
	    
	    unix_time_out = unix_time;
	  }
	}
	
	
	// Parsing waveform
	if (line_counter - 3 * crd_record_counter_for_this_file == 11 and app_params.process_full) {
	  
	  namespace qi = boost::spirit::qi;
	  
	  std::string::const_iterator str_iter = line.begin();
	  std::string::const_iterator end_iter = line.end();
	  std::vector<int16_t> waveform_data;
	  
	  bool res = false;
	  res = qi::phrase_parse(str_iter,
				 end_iter,
				 // Begin grammar
				 (
				  +qi::int_
				  ),
				 // End grammar
				 qi::space,
				 waveform_data);
	  
	  DT_THROW_IF(waveform_data.size() != 1024, std::logic_error, "Wrong number of sample for the waveform!");
	  int16_t waveform_samples[1024];
	  for (int isample = 0; isample < 1024; isample++) {
	    waveform_samples[isample] = waveform_data[isample]; // Easier to declare a ROOT tree with an array
	    waveform_samples_out[isample] = waveform_samples[isample];
	  }
	}
	
	line_counter++;
	if (line_counter - 3 * crd_record_counter_for_this_file == 12) {
	  crd_record_counter++;
	  crd_record_counter_for_this_file++;

	  //Hit selection
	  bool selected_hit = true;
	  
	  if (app_params.process_lt and lto_flag_out == 0) {
	    selected_hit = false;
	  }
	  if (app_params.process_ht and ht_flag_out == 0) {
	    selected_hit = false;
	  }
	  
	  if (selected_hit) {
	    output_tree->Fill();
	  }
	  // Display progress
	  if (crd_record_counter % app_params.crd_counter_period == 0) {
	    std::cout << "Loaded CRD records : " << crd_record_counter << "\n";
	  }
	}
	// If we define a meximum number of hits to be parsed
	if (crd_record_counter !=0 and crd_record_counter == app_params.max_crd) {
	  break;
	}
      }
      
      if (crd_record_counter !=0 and crd_record_counter == app_params.max_crd) {
	break;
      }
      input_file.close();
    } // end of loop on input files

    output_file->Write();
    output_file->Close();
    

  } catch (std::exception & x) {
    std::cerr << "error : " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error : " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
