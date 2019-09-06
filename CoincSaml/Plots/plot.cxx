#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <TFile.h>
#include <TH2D.h>
#include <TH1D.h>

#include <TMath.h>
#include <TCanvas.h>

int plot() {


  TFile *input_file = new TFile("/sps/nemo/scratch/pamahe/SNCrossTalk/CoincSaml/results/CS_run-104_HT.root","READ");

  
  TH1D *measured = (TH1D *)input_file->Get("measured");
  TH1D *expected = (TH1D *)input_file->Get("expected");

  TCanvas *c = new TCanvas("c1","Run 104, test coincidences SAMLONG");
  TPad *c1 = new TPad("c1","Expected",0.01,0.01,0.99,0.51);
  TPad *c2 = new TPad("c2","Measured",0.01,0.51,0.99,0.99);
  
  c1->Draw();
  c2->Draw();

  c1->cd();
  expected->SetLabelSize(0.03);
  expected->SetLineColor(2);
  expected->Draw("ehist");
  
  c2->cd();
  measured->SetLabelSize(0.03);
  measured->Draw("ehist");

  c->Update();
  
  TCanvas *d = new TCanvas("d","Run 104, error in coincidences", 800,600);
  
  TH1D *t = new TH1D("error","(measured - expected)/expected", 260, 0.0, 260.0);

  for (int ipos = 0; ipos < 260; ipos++) {
    
    double Pc = expected->GetBinContent(ipos + 1);
    double DPc = expected->GetBinError(ipos + 1);
    double Pm = measured->GetBinContent(ipos + 1);
    double DPm = measured->GetBinContent(ipos + 1);

    double error = abs(Pm - Pc) / Pc;
    double Derror = (Pc * DPm + Pm * DPc) / pow(Pc,2);
    
    
    if(error != error or Derror != Derror) { 
      t->SetBinContent(ipos + 1, 0);
       t->SetBinError(ipos + 1, 0);
    } else {
      t->SetBinContent(ipos + 1, error);
    t->SetBinError(ipos + 1, Derror);
    }
  }
  t->Draw("ehist");
  d->Update();

  return 0;
}

