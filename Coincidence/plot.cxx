#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

#include <TFile.h>
#include <TStyle.h>
#include <TTree.h>
#include <TBranch.h>
#include <TH2D.h>

void plot() {

  std::vector<int> run_list = {101}; 
  
  for (auto run : run_list) {

    std::string output_file_name = "/sps/nemo/scratch/pamahe/SNCrossTalk/Coincidence/plots/plot_run-" + std::to_string(run) + ".root"; 

    std::string canvas_title = "Coincidence study run " + std::to_string(run) + " HT.";

    TCanvas *c = new TCanvas("c", canvas_title.c_str(), 800, 600);
    TPad *c1 = new TPad("c1","+/- 100 ns", 0.01, 0.51, 0.51, 0.99);
    TPad *c2 = new TPad("c2","+/- 1e3 ns", 0.51, 0.51, 0.99, 0.99);
    TPad *c3 = new TPad("c3","+/- 1e4 ns", 0.01, 0.01, 0.51, 0.51);
    TPad *c4 = new TPad("c4","+/- 1e5 ns", 0.51, 0.01, 0.99, 0.51);
    
    c1->Draw(); c2->Draw(); c3->Draw(); c4->Draw();
    
    c1->cd();
    TFile *input_file_1 = new TFile("/sps/nemo/scratch/pamahe/SNCrossTalk/Coincidence/results/run_104/CT_run-104_HT_100.root","READ");
    TH1D *input_histo_1 = (TH1D *)input_file_1->Get("cth_12.5");
    input_histo_1->SetBinContent(13,6,0);
    input_histo_1->GetXaxis()->SetLabelSize(0.055);
    input_histo_1->GetYaxis()->SetLabelSize(0.055);
    input_histo_1->SetTitleSize(2);
    input_histo_1->SetMarkerSize(2);
    c1->SetLogz();
    input_histo_1->Draw("colz,text");
    
    c2->cd();
    TFile *input_file_2 = new TFile("/sps/nemo/scratch/pamahe/SNCrossTalk/Coincidence/results/run_104/CT_run-104_HT_1e3.root","READ");
    TH1D *input_histo_2 = (TH1D *)input_file_2->Get("cth_12.5");
    input_histo_2->SetBinContent(13,6,0);
    input_histo_2->GetXaxis()->SetLabelSize(0.055);
    input_histo_2->GetYaxis()->SetLabelSize(0.055);
    input_histo_2->SetTitleSize(2);
    input_histo_2->SetMarkerSize(2);
    c2->SetLogz();
    input_histo_2->Draw("colz,text");
    
    c3->cd();
    TFile *input_file_3 = new TFile("/sps/nemo/scratch/pamahe/SNCrossTalk/Coincidence/results/run_104/CT_run-104_HT_1e4.root","READ");
    TH1D *input_histo_3 = (TH1D *)input_file_3->Get("cth_12.5");
    input_histo_3->SetBinContent(13,6,0);
    input_histo_3->GetXaxis()->SetLabelSize(0.055);
    input_histo_3->GetYaxis()->SetLabelSize(0.055);
    input_histo_3->SetTitleSize(2);
    input_histo_3->SetMarkerSize(2);
    c3->SetLogz();
    input_histo_3->Draw("colz,text");
    
    c4->cd();
    TFile *input_file_4 = new TFile("/sps/nemo/scratch/pamahe/SNCrossTalk/Coincidence/results/run_104/CT_run-104_HT_1e5.root","READ");
    TH1D *input_histo_4 = (TH1D *)input_file_4->Get("cth_12.5");
    input_histo_4->SetBinContent(13,6,0);
    input_histo_4->GetXaxis()->SetLabelSize(0.055);
    input_histo_4->GetYaxis()->SetLabelSize(0.055);
    input_histo_4->SetTitleSize(2);
    input_histo_4->SetMarkerSize(2);
    c4->SetLogz();
    input_histo_4->Draw("colz,text");
    
    c->Update();
    
    TFile *output_file = new TFile(output_file_name.c_str(), "RECREATE");

    c->Write();
  }
}
