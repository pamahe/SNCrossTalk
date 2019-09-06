#include <TCanvas.h>
#include <TH1.h>
#include <TMath.h>
#include <TVirtualFFT.h>
#include <TRandom3.h>
#include <TF1.h>

const Double_t pi = TMath::Pi();
const Double_t twoPi = TMath::TwoPi();

void FFT() {
  
  TCanvas *c = new TCanvas("c","FFT practice", 800, 600);
  TPad *c1 = new TPad("c1","", 0.01, 0.01, 0.99, 0.49); 
  TPad *c2 = new TPad("c2","", 0.01, 0.51, 0.99, 0.99);
  c1->Draw();
  c2->Draw();

  c2->cd();
  // Opening hit time distrinution
  TFile *input_file = new TFile("/sps/nemo/scratch/pamahe/SNCrossTalk/FFT/HTD_run-104_HT.root","READ");
  TH1D *histo = (TH1D *)input_file->Get("full");
  gStyle->SetOptStat(0);
  gStyle->SetTitleH(0.1);
  gStyle->SetTitleW(0.85);
  histo->SetTitleSize(0.05,"xy");
  histo->GetXaxis()->SetLabelSize(0.05);
  histo->GetYaxis()->SetLabelSize(0.05);
  histo->GetXaxis()->SetTitle("Time (seconds)");
  histo->GetYaxis()->SetTitle("Counts");
  histo->Rebin(100);
  histo->Draw();

  c1->cd();
  // Computing the tranform and look at the magnitude of the output
  TH1 *hmRaw = NULL;
  TVirtualFFT::SetTransform(0);
  hmRaw = histo->FFT(hmRaw, "MAG");
  TH1D *output_histo = new TH1D("FFT_HTD","Magnitude of the Hit Time Distribution FFT", hmRaw->GetXaxis()->GetNbins(), 0, 
				(hmRaw->GetXaxis()->GetXmax() - hmRaw->GetXaxis()->GetXmin()) / (histo->GetXaxis()->GetXmax() - histo->GetXaxis()->GetXmin()));
  for (Int_t bin = 1; bin <= hmRaw->GetXaxis()->GetNbins(); bin++) {
    output_histo->SetBinContent(bin, hmRaw->GetBinContent(bin));
  }

  // Style options
  gStyle->SetOptStat(0);
  gStyle->SetTitleH(0.1);
  gStyle->SetTitleW(0.85);
  output_histo->SetTitleSize(0.05,"xy");
  output_histo->GetXaxis()->SetLabelSize(0.05);
  output_histo->GetYaxis()->SetLabelSize(0.05);
  output_histo->GetXaxis()->SetRangeUser(0.0,18.5);
  output_histo->GetYaxis()->SetRangeUser(0.0,3000.0);
  output_histo->GetXaxis()->SetTitle("Frequency (Hz)");
  output_histo->GetYaxis()->SetTitle("Magnitude");
  output_histo->SetLineColor(4);
  output_histo->Draw();
  
  c->Update();
}
