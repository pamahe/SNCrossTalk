#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

#include <TFile.h>
#include <TH1D.h>

void plotSum() {

  TCanvas *c = new TCanvas("c","Coicidences Probabilities around reference cube 12.5, run 104", 800, 600);
  gStyle->SetOptStat(0);
  auto legend = new TLegend(0.8,0.7,0.9,0.9);
  
  std::vector<string> ct_list = {"1e6", "1e5", "1e4", "1e3" ,"100", "10", "1"};
  
  for (auto ct : ct_list) {
    
    std::string input_filename = "/sps/nemo/scratch/pamahe/SNCrossTalk/2Dto1D/SumProb_run-104_all.root";
    TFile *input_file = new TFile(input_filename.c_str(),"READ");
    
    std::string input_histoname = "sum_prob_" + ct;
    TH1D *input_histo = (TH1D *)input_file->Get(input_histoname.c_str());
    
    if (ct == "1") {
      input_histo->SetFillStyle(3001);
      input_histo->SetFillColor(2);
      input_histo->SetLineColor(2);
      input_histo->SetLineStyle(1);
      input_histo->Draw("hist,same");
      legend->AddEntry(input_histo,"1 ns", "f");
    }
    /*
    if (ct == "10") {
      input_histo->SetFillStyle(3001);
      input_histo->SetFillColor(30);
      input_histo->SetLineColor(30);
      input_histo->SetLineStyle(1);
      input_histo->Draw("hist,same");
      legend->AddEntry(input_histo,"10 ns", "f");
    }

    if (ct == "100") {
      input_histo->SetFillStyle(3001);
      input_histo->SetFillColor(7);
      input_histo->SetLineColor(7);
      input_histo->SetLineStyle(1);
      input_histo->Draw("hist,same");
      legend->AddEntry(input_histo,"100 ns", "f");
    }
    */ 
    if (ct == "1e3") {
      input_histo->SetFillStyle(3001);
      input_histo->SetFillColor(4);
      input_histo->SetLineColor(4);
      input_histo->SetLineStyle(1);
      input_histo->Draw("hist,same");
      legend->AddEntry(input_histo,"1e3 ns", "f");
    }
      
    if (ct == "1e4") { 
      input_histo->SetFillStyle(3001);
      input_histo->SetFillColor(6);
      input_histo->SetLineColor(6);
      input_histo->SetLineStyle(1);
      input_histo->Draw("hist,same");
      legend->AddEntry(input_histo,"1e4 ns", "f");
    }
      
    if (ct == "1e5") {
      input_histo->SetFillStyle(3001);
      input_histo->SetFillColor(5);
      input_histo->SetLineColor(5);
      input_histo->SetLineStyle(1);
      input_histo->Draw("hist,same");
      legend->AddEntry(input_histo,"1e5 ns", "f");
    }
    
    if (ct == "1e6") {
      input_histo->SetFillStyle(3001);
      input_histo->SetFillColor(3);
      input_histo->SetLineColor(3);
      input_histo->SetLineStyle(1);
      input_histo->Draw("hist,same");
      legend->AddEntry(input_histo,"1e6 ns", "f");
      input_histo->GetYaxis()->SetRangeUser(0.,0.02);
      input_histo->SetTitle("Coincidence Probabilities around reference cube 12.5, run 104, for different windows");
      c->Update();
    }
  
    legend->Draw();
    c->Update();
  }
}
