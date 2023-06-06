/**************************************************************************
 * Copyright(c) 1998-2016, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include <TGrid.h>
#include <TChain.h>

#include <TClonesArray.h>
#include <TFile.h>
#include <THashList.h>
#include <TF1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TList.h>
#include <TCanvas.h>
#include <TString.h>

#include <TRandom3.h>
#include <TMath.h>

#include "AliOADBContainer.h"
#include "AliAODVertex.h"
#include "AliAODHandler.h"

#include <AliAnalysisTask.h>
#include <AliAnalysisManager.h>
#include <AliVEventHandler.h>
#include <AliVCluster.h>
#include <AliVParticle.h>
#include <AliLog.h>

#include "AliMultSelection.h"

#include "AliTLorentzVector.h"
#include "AliEmcalJet.h"
#include "AliJetContainer.h"
#include "AliParticleContainer.h"
#include "AliClusterContainer.h"

#include "AliJEQnVectorHandler.h"

#include "AliLocalRhoParameter.h"
#include "AliRhoParameter.h"

#include "AliAnalysisTaskFlowVectorCorrectionsPWGJE.h"
#include "AliAnalysisTaskEPCalibForJet.h"

#include "AliDataFile.h"

class AliAnalysisTaskEPCalibForJet;

/// \cond CLASSIMP
ClassImp(AliAnalysisTaskEPCalibForJet);
/// \endcond

//Default constructor. Needed by ROOT I/O
AliAnalysisTaskEPCalibForJet::AliAnalysisTaskEPCalibForJet() :
  AliAnalysisTaskEmcalJet(),
  fHistManager(),
  fEventCuts(),
  fYAMLConfig(),
  fUseRunList(),
  fAOD(nullptr),
  fOutputList(nullptr),
  fCalibRefObjList(nullptr),
  fCalibV0Ref(nullptr),
  fPileupCut(kFALSE),
  fTPCQnMeasure(kFALSE),
  fPileupCutQA(kFALSE),
  fCalibQA(kFALSE),
  fGainCalibQA(kFALSE),
  fReCentCalibQA(kFALSE),
  fEPQA(kFALSE),
  fEPResoQA(kFALSE),
  fTrackQA(kFALSE),
  fBkgQA(kFALSE),
  fV0Combin(kFALSE),
  fCalibType(0),
  fQnVCalibType(kOrig),
  fNormMethod(0),
  fV2ResoV0(0.), fV3ResoV0(0.),
  fV0CutPU(NULL),
  fSPDCutPU(NULL),
  fMultCutPU(NULL),
  fCenCutLowPU(NULL),
  fCenCutHighPU(NULL),
  fHCorrV0ChWeghts(NULL),
  fHCorrQ2xV0M(NULL),
  fHCorrQ2yV0M(NULL),
  fHCorrQ2xV0C(NULL),
  fHCorrQ2yV0C(NULL),
  fHCorrQ2xV0A(NULL),
  fHCorrQ2yV0A(NULL),
  fHCorrQ3xV0M(NULL),
  fHCorrQ3yV0M(NULL),
  fHCorrQ3xV0C(NULL),
  fHCorrQ3yV0C(NULL),
  fHCorrQ3xV0A(NULL),
  fHCorrQ3yV0A(NULL), 
  fFitModulationType(kNoFit),  fFitModulation(0), hBkgTracks(0),
  fEtaMinTPC(0.),
  fEtaMaxTPC(0.8),
  fPtMinTPC(0.2),
  fPtMaxTPC(5.),
  fUsedTrackPosIDs(),
  fUsedTrackNegIDs(),
  fFractionOfTracksForQnTPC(1.1),
  fV0(nullptr),
  fRun(0),
  fZvtx(-9999.),
  fCentrality(-9999.),
  fIsOADBFileOpen(false),
  fMultV0BefCorPfpx(0),
  fOADBzArray_contQx2am(0),
  fOADBzArray_contQy2am(0),
  fOADBzArray_contQx2as(0),
  fOADBzArray_contQy2as(0),
  fOADBzArray_contQx2cm(0),
  fOADBzArray_contQy2cm(0),
  fOADBzArray_contQx2cs(0),
  fOADBzArray_contQy2cs(0),
  fOADBzArray_contQx3am(0),
  fOADBzArray_contQy3am(0),
  fOADBzArray_contQx3as(0),
  fOADBzArray_contQy3as(0),
  fOADBzArray_contQx3cm(0),
  fOADBzArray_contQy3cm(0),
  fOADBzArray_contQx3cs(0),
  fOADBzArray_contQy3cs(0),
  fOADBcentArray_contTPCposEta(0),
  fOADBcentArray_contTPCnegEta(0),
  fCalibObjRun(-9999),
  fHistMultV0(nullptr),
  fV0CalibZvtxDiff(true),
  fEnablePhiDistrHistos(false),
  CheckRunNum(0), fQaEventNum(-1)
{
    
    for(Int_t i(0); i < 2; i++){
      befGainCalibQ2VecV0M[i] = 0.;
      befGainCalibQ2VecV0C[i] = 0.;
      befGainCalibQ2VecV0A[i] = 0.;
      befGainCalibQ3VecV0M[i] = 0.;
      befGainCalibQ3VecV0C[i] = 0.;
      befGainCalibQ3VecV0A[i] = 0.;

      q2VecV0M[i] = 0.;
      q2VecV0C[i] = 0.;
      q2VecV0A[i] = 0.;
      q3VecV0M[i] = 0.;
      q3VecV0C[i] = 0.;
      q3VecV0A[i] = 0.;

      q2VecTpcM[i] = 0.;
      q2VecTpcP[i] = 0.;
      q2VecTpcN[i] = 0.;
      q3VecTpcM[i] = 0.;
      q3VecTpcP[i] = 0.;
      q3VecTpcN[i] = 0.;
    }

    for(Int_t i(0); i < 3; i++){
      V0Mult2[i] = 0.;
      V0Mult3[i] = 0.;
      TpcMult2[i] = 0.;
      TpcMult3[i] = 0.;

      befGainCalibQ2V0[i] = 0.;
      befGainCalibQ3V0[i] = 0.;

      q2V0[i] = 0.;
      q3V0[i] = 0.;
      q2Tpc[i] = 0.;
      q3Tpc[i] = 0.;

      psi2V0[i] = 0.;
      psi3V0[i] = 0.;
      psi2Tpc[i] = 0.;
      psi3Tpc[i] = 0.;

      q2NormV0[i] = -9999.;
      q3NormV0[i] = -9999.;
      q2NormTpc[i] = -9999.;
      q3NormTpc[i] = -9999.;
    }
  

    fUsedTrackPosIDs = TBits(1000);
    fUsedTrackNegIDs = TBits(1000);

    for(int iZvtx = 0; iZvtx < 14; iZvtx++) {
        fQx2mV0A[iZvtx] = nullptr;
        fQy2mV0A[iZvtx] = nullptr;
        fQx2sV0A[iZvtx] = nullptr;
        fQy2sV0A[iZvtx] = nullptr;
        fQx2mV0C[iZvtx] = nullptr;
        fQy2mV0C[iZvtx] = nullptr;
        fQx2sV0C[iZvtx] = nullptr;
        fQy2sV0C[iZvtx] = nullptr;

        fQx3mV0A[iZvtx] = nullptr;
        fQy3mV0A[iZvtx] = nullptr;
        fQx3sV0A[iZvtx] = nullptr;
        fQy3sV0A[iZvtx] = nullptr;
        fQx3mV0C[iZvtx] = nullptr;
        fQy3mV0C[iZvtx] = nullptr;
        fQx3sV0C[iZvtx] = nullptr;
        fQy3sV0C[iZvtx] = nullptr;  
    }

    for(int iCent = 0; iCent < 11; iCent++) {
        fWeightsTPCPosEta[iCent] = nullptr;
        fWeightsTPCNegEta[iCent] = nullptr;
    }

    fPhiVsCentrTPC[0]=nullptr;
    fPhiVsCentrTPC[1]=nullptr;

    fOADBzArray_contQx2am = new TObjArray();
    fOADBzArray_contQy2am = new TObjArray();
    fOADBzArray_contQx2as = new TObjArray();
    fOADBzArray_contQy2as = new TObjArray();
    fOADBzArray_contQx2cm = new TObjArray();
    fOADBzArray_contQy2cm = new TObjArray();
    fOADBzArray_contQx2cs = new TObjArray();
    fOADBzArray_contQy2cs = new TObjArray();
    
    fOADBzArray_contQx3am = new TObjArray();
    fOADBzArray_contQy3am = new TObjArray();
    fOADBzArray_contQx3as = new TObjArray();
    fOADBzArray_contQy3as = new TObjArray();
    fOADBzArray_contQx3cm = new TObjArray();
    fOADBzArray_contQy3cm = new TObjArray();
    fOADBzArray_contQx3cs = new TObjArray();
    fOADBzArray_contQy3cs = new TObjArray();

    fOADBcentArray_contTPCposEta = new TObjArray();
    fOADBcentArray_contTPCnegEta = new TObjArray();
}

// Standard constructor. Should be used by the user. @param[in] name Name of the task
AliAnalysisTaskEPCalibForJet::AliAnalysisTaskEPCalibForJet(const char *name):
  AliAnalysisTaskEmcalJet(name, kTRUE),
  fHistManager(name),
  fEventCuts(),
  fYAMLConfig(),
  fUseRunList(),
  fAOD(nullptr),
  fOutputList(nullptr),
  fCalibRefObjList(nullptr),
  fCalibV0Ref(nullptr),
  fPileupCut(kFALSE),
  fTPCQnMeasure(kFALSE),
  fPileupCutQA(kFALSE),
  fCalibQA(kFALSE),
  fGainCalibQA(kFALSE),
  fReCentCalibQA(kFALSE),
  fEPQA(kFALSE),
  fEPResoQA(kFALSE),
  fTrackQA(kFALSE),
  fBkgQA(kFALSE),
  fV0Combin(kFALSE),
  fCalibType(0),
  fQnVCalibType(kOrig),
  fNormMethod(0),
  fV2ResoV0(0.), fV3ResoV0(0.),
  fV0CutPU(NULL),
  fSPDCutPU(NULL),
  fMultCutPU(NULL),
  fCenCutLowPU(NULL),
  fCenCutHighPU(NULL),
  fHCorrV0ChWeghts(NULL),
  fHCorrQ2xV0M(NULL),
  fHCorrQ2yV0M(NULL),
  fHCorrQ2xV0C(NULL),
  fHCorrQ2yV0C(NULL),
  fHCorrQ2xV0A(NULL),
  fHCorrQ2yV0A(NULL),
  fHCorrQ3xV0M(NULL),
  fHCorrQ3yV0M(NULL),
  fHCorrQ3xV0C(NULL),
  fHCorrQ3yV0C(NULL),
  fHCorrQ3xV0A(NULL),
  fHCorrQ3yV0A(NULL), 
  fFitModulationType(kNoFit),  fFitModulation(0), hBkgTracks(0),
  fEtaMinTPC(0.),
  fEtaMaxTPC(0.8),
  fPtMinTPC(0.2),
  fPtMaxTPC(5.),
  fUsedTrackPosIDs(),
  fUsedTrackNegIDs(),
  fFractionOfTracksForQnTPC(1.1),
  fV0(nullptr),
  fRun(0),
  fZvtx(-9999.),
  fCentrality(-9999.),
  fIsOADBFileOpen(false),
  fMultV0BefCorPfpx(0),
  fOADBzArray_contQx2am(0),
  fOADBzArray_contQy2am(0),
  fOADBzArray_contQx2as(0),
  fOADBzArray_contQy2as(0),
  fOADBzArray_contQx2cm(0),
  fOADBzArray_contQy2cm(0),
  fOADBzArray_contQx2cs(0),
  fOADBzArray_contQy2cs(0),
  fOADBzArray_contQx3am(0),
  fOADBzArray_contQy3am(0),
  fOADBzArray_contQx3as(0),
  fOADBzArray_contQy3as(0),
  fOADBzArray_contQx3cm(0),
  fOADBzArray_contQy3cm(0),
  fOADBzArray_contQx3cs(0),
  fOADBzArray_contQy3cs(0),
  fOADBcentArray_contTPCposEta(0),
  fOADBcentArray_contTPCnegEta(0),
  fCalibObjRun(-9999),
  fHistMultV0(nullptr),
  fV0CalibZvtxDiff(true),
  fEnablePhiDistrHistos(false),
  CheckRunNum(0), fQaEventNum(-1)
{
  
    for(Int_t i(0); i < 2; i++){
      befGainCalibQ2VecV0M[i] = 0.;
      befGainCalibQ2VecV0C[i] = 0.;
      befGainCalibQ2VecV0A[i] = 0.;
      befGainCalibQ3VecV0M[i] = 0.;
      befGainCalibQ3VecV0C[i] = 0.;
      befGainCalibQ3VecV0A[i] = 0.;

      q2VecV0M[i] = 0.;
      q2VecV0C[i] = 0.;
      q2VecV0A[i] = 0.;
      q3VecV0M[i] = 0.;
      q3VecV0C[i] = 0.;
      q3VecV0A[i] = 0.;

      q2VecTpcM[i] = 0.;
      q2VecTpcP[i] = 0.;
      q2VecTpcN[i] = 0.;
      q3VecTpcM[i] = 0.;
      q3VecTpcP[i] = 0.;
      q3VecTpcN[i] = 0.;
    }

    for(Int_t i(0); i < 3; i++){
      V0Mult2[i] = 0.;
      V0Mult3[i] = 0.;
      TpcMult2[i] = 0.;
      TpcMult3[i] = 0.;

      q2V0[i] = 0.;
      q3V0[i] = 0.;
      q2Tpc[i] = 0.;
      q3Tpc[i] = 0.;

      psi2V0[i] = 0.;
      psi3V0[i] = 0.;
      psi2Tpc[i] = 0.;
      psi3Tpc[i] = 0.;

      q2NormV0[i] = -9999.;
      q3NormV0[i] = -9999.;
      q2NormTpc[i] = -9999.;
      q3NormTpc[i] = -9999.;
    }
  
  
    fUsedTrackPosIDs = TBits(1000);
    fUsedTrackNegIDs = TBits(1000);

    for(int iZvtx = 0; iZvtx < 14; iZvtx++) {
        fQx2mV0A[iZvtx] = nullptr;
        fQy2mV0A[iZvtx] = nullptr;
        fQx2sV0A[iZvtx] = nullptr;
        fQy2sV0A[iZvtx] = nullptr;
        fQx2mV0C[iZvtx] = nullptr;
        fQy2mV0C[iZvtx] = nullptr;
        fQx2sV0C[iZvtx] = nullptr;
        fQy2sV0C[iZvtx] = nullptr;

        fQx3mV0A[iZvtx] = nullptr;
        fQy3mV0A[iZvtx] = nullptr;
        fQx3sV0A[iZvtx] = nullptr;
        fQy3sV0A[iZvtx] = nullptr;
        fQx3mV0C[iZvtx] = nullptr;
        fQy3mV0C[iZvtx] = nullptr;
        fQx3sV0C[iZvtx] = nullptr;
        fQy3sV0C[iZvtx] = nullptr;  
    }

    for(int iCent = 0; iCent < 11; iCent++) {
        fWeightsTPCPosEta[iCent] = nullptr;
        fWeightsTPCNegEta[iCent] = nullptr;
    }

    fPhiVsCentrTPC[0]=nullptr;
    fPhiVsCentrTPC[1]=nullptr;

    fOADBzArray_contQx2am = new TObjArray();
    fOADBzArray_contQy2am = new TObjArray();
    fOADBzArray_contQx2as = new TObjArray();
    fOADBzArray_contQy2as = new TObjArray();
    fOADBzArray_contQx2cm = new TObjArray();
    fOADBzArray_contQy2cm = new TObjArray();
    fOADBzArray_contQx2cs = new TObjArray();
    fOADBzArray_contQy2cs = new TObjArray();
    
    fOADBzArray_contQx3am = new TObjArray();
    fOADBzArray_contQy3am = new TObjArray();
    fOADBzArray_contQx3as = new TObjArray();
    fOADBzArray_contQy3as = new TObjArray();
    fOADBzArray_contQx3cm = new TObjArray();
    fOADBzArray_contQy3cm = new TObjArray();
    fOADBzArray_contQx3cs = new TObjArray();
    fOADBzArray_contQy3cs = new TObjArray();

    fOADBcentArray_contTPCposEta = new TObjArray();
    fOADBcentArray_contTPCnegEta = new TObjArray();
  
  if(fLocalRhoName=="") fLocalRhoName = Form("LocalRhoFrom_%s", GetName());
  SetMakeGeneralHistograms(kTRUE);
}

// Destructor
AliAnalysisTaskEPCalibForJet::~AliAnalysisTaskEPCalibForJet()
{
  if(fCalibV0Ref)   {delete fCalibV0Ref;   fCalibV0Ref = 0x0;}
  if(fCalibRefObjList) {delete fCalibRefObjList; fCalibRefObjList = 0x0;}
  if(fFitModulation) {delete fFitModulation; fFitModulation = 0x0;}
  if(hBkgTracks) {delete hBkgTracks; hBkgTracks = 0x0;}
}


void AliAnalysisTaskEPCalibForJet::SetRunList(bool removeDummyTask)
{
  fYAMLConfig.AddConfiguration(fRunListFileName, "runlist");
  fYAMLConfig.Initialize();
  fYAMLConfig.GetProperty("runlist", fUseRunList);
}


/**
 * Performing run-independent initialization.
 * Here the histograms should be instantiated.
 */
void AliAnalysisTaskEPCalibForJet::UserCreateOutputObjects()
{
  SetRunList();
  fOutputList = new TList();
  fOutputList->SetOwner(true);

  AliAnalysisTaskEmcalJet::UserCreateOutputObjects();
  
  fEventCuts.AddQAplotsToList(fOutput);
  fEventCuts.OverrideAutomaticTriggerSelection(AliVEvent::kAnyINT | AliVEvent::kCentral | AliVEvent::kSemiCentral);
  
  // == s == Set Out put Hist grams  ###########################################
  if(fPileupCutQA) AllocatePileupCutHistograms();
  if(fGainCalibQA) AllocateGainCalibHistograms();
  if(fReCentCalibQA) AllocateReCentCalibHistograms();
  if(fEPQA) AllocateEventPlaneHistograms();
  if(fEPResoQA) AllocateEPResoHistograms();
  if(fTrackQA) AllocateTrackHistograms();
  if(fBkgQA) AllocateBkgHistograms();
  // == e == Set Out put Hist grams  ###########################################
  
  
  // == s == Add Objects into output file  #####################################
  TIter next(fHistManager.GetListOfHistograms());
  TObject* obj = 0;
  while ((obj = next())) {
    fOutput->Add(obj);
  }
  // == e == Add Objects into output file  #####################################


    PostData(1, fOutput); // Post data for ALL output slots > 0 here.
  
}

void AliAnalysisTaskEPCalibForJet::AllocatePileupCutHistograms(){
  TString histName;
  TString histtitle;
  TString groupName;
  groupName="PileupCutQA";

  Int_t gMaxGlobalmult  = 4000;
  Int_t gMaxTPCcorrmult = 5000;
  Int_t gMaxESDtracks   = 20000;

  fHistManager.CreateHistoGroup(groupName);

  histName  = TString::Format("%s/fHistCentCL0VsV0MBefore", groupName.Data());
  histtitle = TString::Format("%s;Cent(V0M); Cent(CL0)", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100,0,100,100,0,100);
  histName  = TString::Format("%s/fHistTPCVsESDTrkBefore", groupName.Data());
  histtitle = TString::Format("%s;TPC1; ESD trk", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100,0,5000,200,0,20000);
  histName  = TString::Format("%s/fHistTPConlyVsCL1Before", groupName.Data());
  histtitle = TString::Format("%s;Cent(CL1); TPC(FB128)", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100,0,100,250,0,gMaxTPCcorrmult);
  histName  = TString::Format("%s/fHistTPConlyVsV0MBefore", groupName.Data());
  histtitle = TString::Format("%s;Cent(V0M); TPC(FB128)", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100,0,100,250,0,gMaxTPCcorrmult);

  histName  = TString::Format("%s/fHistCentCL0VsV0MAfter", groupName.Data());
  histtitle = TString::Format("%s;Cent(V0M); Cent(CL0)", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100,0,100,100,0,100);
  histName  = TString::Format("%s/fHistTPCVsESDTrkAfter", groupName.Data());
  histtitle = TString::Format("%s;TPC1; ESD trk", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100,0,5000,200,0,20000);
  histName  = TString::Format("%s/fHistTPConlyVsCL1After", groupName.Data());
  histtitle = TString::Format("%s;Cent(CL1); TPC(FB128)", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100,0,100,250,0,gMaxTPCcorrmult);
  histName  = TString::Format("%s/fHistTPConlyVsV0MAfter", groupName.Data());
  histtitle = TString::Format("%s;Cent(V0M); TPC(FB128)", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100,0,100,250,0,gMaxTPCcorrmult);

}

void AliAnalysisTaskEPCalibForJet::AllocateGainCalibHistograms(){
  TString histName;
  TString histtitle;
  TString groupName;
  groupName="GainCalib";

  fHistManager.CreateHistoGroup(groupName);
  THashList *parent = static_cast<THashList *>(fHistManager.FindObject(groupName.Data()));
  
  std::cout << " =================   runListSize : " << fUseRunList.size() << std::endl;

  for(Int_t eventNumBin = 0; eventNumBin < fUseRunList.size(); eventNumBin++){
    Int_t runEventNum = stoi(fUseRunList.at(eventNumBin));
    histName = TString::Format("hAvgV0ChannelsvsVz_%d", runEventNum);
    histtitle = histName;
    TProfile2D *tempHist = new TProfile2D(histName, histtitle, 10,-10,10,64,0,64);
    parent->Add(tempHist);
    
    histName = TString::Format("GainCalib/hTPCPosiTrkVzPhiEta_%d", runEventNum);
    fHistManager.CreateTH3(histName, histtitle,10,-10,10,50,0,6.283185,16,-0.8,0.8);
    histName = TString::Format("GainCalib/hTPCNegaTrkVzPhiEta_%d", runEventNum);
    fHistManager.CreateTH3(histName, histtitle,10,-10,10,50,0,6.283185,16,-0.8,0.8);
    
  }

    histName  = TString::Format("%s/CentGainCalibImpQ2x_V0M", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q2x_V0M", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 200, -0.01, 0.01);
    histName  = TString::Format("%s/CentGainCalibImpQ2y_V0M", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q2y_V0M", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 200, -0.01, 0.01);
    histName  = TString::Format("%s/CentGainCalibImpQ2x_V0C", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q2x_V0C", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 200, -0.01, 0.01);
    histName  = TString::Format("%s/CentGainCalibImpQ2y_V0C", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q2y_V0C", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 200, -0.01, 0.01);
    histName  = TString::Format("%s/CentGainCalibImpQ2x_V0A", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q2x_V0A", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 200, -0.01, 0.01);
    histName  = TString::Format("%s/CentGainCalibImpQ2y_V0A", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q2y_V0A", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 200, -0.01, 0.01);
    
    histName  = TString::Format("%s/CentGainCalibImpQ3x_V0M", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q3x_V0M", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 200, -0.01, 0.01);
    histName  = TString::Format("%s/CentGainCalibImpQ3y_V0M", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q3y_V0M", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 200, -0.01, 0.01);
    histName  = TString::Format("%s/CentGainCalibImpQ3x_V0C", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q3x_V0C", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 200, -0.01, 0.01);
    histName  = TString::Format("%s/CentGainCalibImpQ3y_V0C", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q3y_V0C", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 200, -0.01, 0.01);
    histName  = TString::Format("%s/CentGainCalibImpQ3x_V0A", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q3x_V0A", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 200, -0.01, 0.01);
    histName  = TString::Format("%s/CentGainCalibImpQ3y_V0A", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q3y_V0A", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 200, -0.01, 0.01);
}

void AliAnalysisTaskEPCalibForJet::AllocateReCentCalibHistograms(){
  TString histName;
  TString histtitle;
  TString groupName;
  groupName="ReCentCalib";

  fHistManager.CreateHistoGroup(groupName);
  THashList *parent = static_cast<THashList *>(fHistManager.FindObject(groupName.Data()));
  
  
  for(Int_t eventNumBin = 0; eventNumBin < fUseRunList.size(); eventNumBin++){
    Int_t runEventNum = stoi(fUseRunList.at(eventNumBin));

    // == s ==  V0 recentering Measurement  ################################################
    //  Q2
    histName = TString::Format("%s/hAvgQ2XvsCentV0MBef_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q2_{x}>^{V0M}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ2YvsCentV0MBef_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q2_{y}>^{V0M}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);

    histName = TString::Format("%s/hAvgQ2XvsCentV0CBef_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q2_{x}>^{V0C}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ2YvsCentV0CBef_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q2_{y}>^{V0C}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);

    histName = TString::Format("%s/hAvgQ2XvsCentV0ABef_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q2_{x}>^{V0A}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ2YvsCentV0ABef_%d", groupName.Data(), runEventNum);
    histtitle = histtitle = TString::Format("%s;Centrality;<Q2_{y}>^{V0A}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    

    //  Q3
    histName = TString::Format("%s/hAvgQ3XvsCentV0MBef_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q3_{x}>^{V0M}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ3YvsCentV0MBef_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q3_{y}>^{V0M}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);

    histName = TString::Format("%s/hAvgQ3XvsCentV0CBef_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q_3{x}>^{V0C}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ3YvsCentV0CBef_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q3_{y}>^{V0C}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);

    histName = TString::Format("%s/hAvgQ3XvsCentV0ABef_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q3_{x}>^{V0A}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ3YvsCentV0ABef_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q3_{y}>^{V0A}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);

    // Calib After
    //  Q2    
    histName = TString::Format("%s/hAvgQ2XvsCentV0MAft_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q2_{x}>^{V0M}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ2YvsCentV0MAft_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q2_{y}>^{V0M}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);

    histName = TString::Format("%s/hAvgQ2XvsCentV0CAft_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q2_{x}>^{V0C}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ2YvsCentV0CAft_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q2_{y}>^{V0C}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);

    histName = TString::Format("%s/hAvgQ2XvsCentV0AAft_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q2_{x}>^{V0A}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ2YvsCentV0AAft_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q2_{y}>^{V0A}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    
    //  Q3
    histName = TString::Format("%s/hAvgQ3XvsCentV0MAft_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q3_{x}>^{V0M}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ3YvsCentV0MAft_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q3_{y}>^{V0M}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);

    histName = TString::Format("%s/hAvgQ3XvsCentV0CAft_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q_3{x}>^{V0C}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ3YvsCentV0CAft_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q3_{y}>^{V0C}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);

    histName = TString::Format("%s/hAvgQ3XvsCentV0AAft_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q3_{x}>^{V0A}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ3YvsCentV0AAft_%d", groupName.Data(), runEventNum);
    histtitle = TString::Format("%s;Centrality;<Q3_{y}>^{V0A}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);

    // == e ==  V0 recentering Measurement  ################################################


    // == s ==  TPC recentering Measurement  ###############################################
    //  Q2
    histName = TString::Format("%s/hAvgQ2XvsCentTPCPBef_%d", groupName.Data(), runEventNum);
    histtitle = histtitle = TString::Format("%s;Centrality;<Q2_{x}>^{TPC_Posi}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ2YvsCentTPCPBef_%d", groupName.Data(), runEventNum);
    histtitle = histtitle = TString::Format("%s;Centrality;<Q2_{y}>^{TPC_Posi}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ2XvsCentTPCNBef_%d", groupName.Data(), runEventNum);
    histtitle = histtitle = TString::Format("%s;Centrality;<Q2_{x}>^{TPC_Nega}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ2YvsCentTPCNBef_%d", groupName.Data(), runEventNum);
    histtitle = histtitle = TString::Format("%s;Centrality;<Q2_{y}>^{TPC_Nega}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);

    //  Q3
    histName = TString::Format("%s/hAvgQ3XvsCentTPCPBef_%d", groupName.Data(), runEventNum);
    histtitle = histtitle = TString::Format("%s;Centrality;<Q_3{x}>^{TPC_Posi}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ3YvsCentTPCPBef_%d", groupName.Data(), runEventNum);
    histtitle = histtitle = TString::Format("%s;Centrality;<Q3_{y}>^{TPC_Posi}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ3XvsCentTPCNBef_%d", groupName.Data(), runEventNum);
    histtitle = histtitle = TString::Format("%s;Centrality;<Q3_{x}>^{TPC_Nega}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ3YvsCentTPCNBef_%d", groupName.Data(), runEventNum);
    histtitle = histtitle = TString::Format("%s;Centrality;<Q3_{y}>^{TPC_Nega}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);


    //  Q2
    histName = TString::Format("%s/hAvgQ2XvsCentTPCPAft_%d", groupName.Data(), runEventNum);
    histtitle = histtitle = TString::Format("%s;Centrality;<Q2_{x}>^{TPC_Posi}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ2YvsCentTPCPAft_%d", groupName.Data(), runEventNum);
    histtitle = histtitle = TString::Format("%s;Centrality;<Q2_{y}>^{TPC_Posi}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ2XvsCentTPCNAft_%d", groupName.Data(), runEventNum);
    histtitle = histtitle = TString::Format("%s;Centrality;<Q2_{x}>^{TPC_Nega}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ2YvsCentTPCNAft_%d", groupName.Data(), runEventNum);
    histtitle = histtitle = TString::Format("%s;Centrality;<Q2_{y}>^{TPC_Nega}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);

    //  Q3
    histName = TString::Format("%s/hAvgQ3XvsCentTPCPAft_%d", groupName.Data(), runEventNum);
    histtitle = histtitle = TString::Format("%s;Centrality;<Q_3{x}>^{TPC_Posi}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ3YvsCentTPCPAft_%d", groupName.Data(), runEventNum);
    histtitle = histtitle = TString::Format("%s;Centrality;<Q3_{y}>^{TPC_Posi}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ3XvsCentTPCNAft_%d", groupName.Data(), runEventNum);
    histtitle = histtitle = TString::Format("%s;Centrality;<Q3_{x}>^{TPC_Nega}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);
    histName = TString::Format("%s/hAvgQ3YvsCentTPCNAft_%d", groupName.Data(), runEventNum);
    histtitle = histtitle = TString::Format("%s;Centrality;<Q3_{y}>^{TPC_Nega}", histName.Data());
    fHistManager.CreateTProfile(histName, histtitle,90,0,90);

    // == e ==  TPC recentering Measurement  ###############################################
  }

    histName  = TString::Format("%s/CentReCentCalibImpQ2x_V0M", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q2x_V0M", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 100, -0.5, 0.5);
    histName  = TString::Format("%s/CentReCentCalibImpQ2y_V0M", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q2y_V0M", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 100,  -0.5, 0.5);
    histName  = TString::Format("%s/CentReCentCalibImpQ2x_V0C", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q2x_V0C", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 100, -0.5, 0.5);
    histName  = TString::Format("%s/CentReCentCalibImpQ2y_V0C", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q2y_V0C", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 100, -0.5, 0.5);
    histName  = TString::Format("%s/CentReCentCalibImpQ2x_V0A", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q2x_V0A", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 100, -0.5, 0.5);
    histName  = TString::Format("%s/CentReCentCalibImpQ2y_V0A", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q2y_V0A", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 100, -0.5, 0.5);
    
    histName  = TString::Format("%s/CentReCentCalibImpQ3x_V0M", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q3x_V0M", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 100, -0.5, 0.5);
    histName  = TString::Format("%s/CentReCentCalibImpQ3y_V0M", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q3y_V0M", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 100,  -0.5, 0.5);
    histName  = TString::Format("%s/CentReCentCalibImpQ3x_V0C", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q3x_V0C", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 100, -0.5, 0.5);
    histName  = TString::Format("%s/CentReCentCalibImpQ3y_V0C", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q3y_V0C", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 100, -0.5, 0.5);
    histName  = TString::Format("%s/CentReCentCalibImpQ3x_V0A", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q3x_V0A", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 100, -0.5, 0.5);
    histName  = TString::Format("%s/CentReCentCalibImpQ3y_V0A", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q3y_V0A", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 20, 0, 100, 100, -0.5, 0.5);


    // =V0M= 
    histName  = TString::Format("%s/hCentVsQxV0MBefCalib", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q_{x} V0M", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 10, 0, 10, 2000, -10, 10);
    histName  = TString::Format("%s/hCentVsQyV0MBefCalib", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q_{y} V0M", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 10, 0, 10, 2000, -10, 10);

    histName  = TString::Format("%s/hCentVsQxV0MAftCalib", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q_{x} V0M", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 10, 0, 10, 2000, -10, 10);
    histName  = TString::Format("%s/hCentVsQyV0MAftCalib", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q_{y} V0M", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 10, 0, 10, 2000, -10, 10);

    // =V0C= 
    histName  = TString::Format("%s/hCentVsQxV0CBefCalib", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q_{x} V0C", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 10, 0, 10, 2000, -10, 10);
    histName  = TString::Format("%s/hCentVsQyV0CBefCalib", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q_{y} V0C", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 10, 0, 10, 2000, -10, 10);

    histName  = TString::Format("%s/hCentVsQxV0CAftCalib", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q_{x} V0C", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 10, 0, 10, 2000, -10, 10);
    histName  = TString::Format("%s/hCentVsQyV0CAftCalib", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q_{y} V0C", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 10, 0, 10, 2000, -10, 10);
    
    // =V0A= 
    histName  = TString::Format("%s/hCentVsQxV0ABefCalib", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q_{x} V0A", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 10, 0, 10, 2000, -10, 10);
    histName  = TString::Format("%s/hCentVsQyV0ABefCalib", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q_{y} V0A", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 10, 0, 10, 2000, -10, 10);

    histName  = TString::Format("%s/hCentVsQxV0AAftCalib", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q_{x} V0A", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 10, 0, 10, 2000, -10, 10);
    histName  = TString::Format("%s/hCentVsQyV0AAftCalib", groupName.Data());
    histtitle = TString::Format("%s;centrality;Q_{y} V0A", histName.Data());
    fHistManager.CreateTH2(histName, histtitle, 10, 0, 10, 2000, -10, 10);

}


void AliAnalysisTaskEPCalibForJet::AllocateEventPlaneHistograms()
{
  TString histName;
  TString histtitle;
  TString groupName;
  groupName="EventPlane";
  fHistManager.CreateHistoGroup(groupName);

  histName  = TString::Format("%s/hCentrality", groupName.Data());
  histtitle = TString::Format("%s;Centrality;counts", histName.Data());
  fHistManager.CreateTH1(histName, histtitle, 100, 0, 100);

  // == s == gain calibration QA ============================================
  histName  = TString::Format("%s/hV0CellChGains", groupName.Data());
  fHistManager.CreateTH1(histName, histtitle, 64, 0, 64);
  histName  = TString::Format("%s/hV0CellChRatio", groupName.Data());
  histtitle = TString::Format("%s;cell ch number;Ch ratio", histName.Data());
  fHistManager.CreateTH1(histName, histtitle, 64, 0, 64);

  histName  =TString::Format("%s/hV0CellChVsMultBefEq", groupName.Data());
  histtitle =TString::Format("%s;cell ch number;multiplisity before equalization",histName.Data());
  fHistManager.CreateTH1(histName, histtitle, 64, 0, 64);

  histName  =TString::Format("%s/hV0CellChVsMultAftEq", groupName.Data());
  histtitle =TString::Format("%s;cell ch number;multiplisity before equalization",histName.Data());
  fHistManager.CreateTH1(histName, histtitle, 64, 0, 64);
  // == e == gain calibration QA ============================================

  histName  = TString::Format("%s/CentQ2x_V0M", groupName.Data());
  histtitle = TString::Format("%s;centrality;q2x_V0M", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 20);
  histName  = TString::Format("%s/CentQ2y_V0M", groupName.Data());
  histtitle = TString::Format("%s;centrality;q2y_V0M", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 20);
  histName  = TString::Format("%s/CentQ2x_V0C", groupName.Data());
  histtitle = TString::Format("%s;centrality;q2x_V0C", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 20);
  histName  = TString::Format("%s/CentQ2y_V0C", groupName.Data());
  histtitle = TString::Format("%s;centrality;q2y_V0C", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 20);
  histName  = TString::Format("%s/CentQ2x_V0A", groupName.Data());
  histtitle = TString::Format("%s;centrality;q2x_V0A", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 20);
  histName  = TString::Format("%s/CentQ2y_V0A", groupName.Data());
  histtitle = TString::Format("%s;centrality;q2y_V0A", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 20);
    
  histName  = TString::Format("%s/CentQ3x_V0M", groupName.Data());
  histtitle = TString::Format("%s;centrality;q3x_V0M", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 20);
  histName  = TString::Format("%s/CentQ3y_V0M", groupName.Data());
  histtitle = TString::Format("%s;centrality;q3y_V0M", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 20);
  histName  = TString::Format("%s/CentQ3x_V0C", groupName.Data());
  histtitle = TString::Format("%s;centrality;q3x_V0C", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 20);
  histName  = TString::Format("%s/CentQ3y_V0C", groupName.Data());
  histtitle = TString::Format("%s;centrality;q3y_V0C", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 20);
  histName  = TString::Format("%s/CentQ3x_V0A", groupName.Data());
  histtitle = TString::Format("%s;centrality;q3x_V0A", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 20);
  histName  = TString::Format("%s/CentQ3y_V0A", groupName.Data());
  histtitle = TString::Format("%s;centrality;q3y_V0A", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 20);

  histName  = TString::Format("%s/Centq2_V0M", groupName.Data());
  histtitle = TString::Format("%s;centrality;q2_V0M", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 20);
  histName  = TString::Format("%s/Centq2_V0C", groupName.Data());
  histtitle = TString::Format("%s;centrality;q2_V0C", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 20);
  histName  = TString::Format("%s/Centq2_V0A", groupName.Data());
  histtitle = TString::Format("%s;centrality;q2_V0A", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 20);

  histName  = TString::Format("%s/Centq3_V0M", groupName.Data());
  histtitle = TString::Format("%s;centrality;q3_V0M", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 20);
  histName  = TString::Format("%s/Centq3_V0C", groupName.Data());
  histtitle = TString::Format("%s;centrality;q3_V0C", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 20);
  histName  = TString::Format("%s/Centq3_V0A", groupName.Data());
  histtitle = TString::Format("%s;centrality;q3_V0A", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 20);


  histName  = TString::Format("%s/q2_V0MV0C", groupName.Data());
  histtitle = TString::Format("%s;q2_V0M;q2_V0C", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 20, 100, 0, 20);
  histName  = TString::Format("%s/q2_V0MV0A", groupName.Data());
  histtitle = TString::Format("%s;q2_V0M;q2_V0A", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 20, 100, 0, 20);
  histName  = TString::Format("%s/q2_V0CV0A", groupName.Data());
  histtitle = TString::Format("%s;q2_V0C;q2_V0A", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 20, 100, 0, 20);

  histName  = TString::Format("%s/q3_V0MV0C", groupName.Data());
  histtitle = TString::Format("%s;q3_V0M;q3_V0C", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 20, 100, 0, 20);
  histName  = TString::Format("%s/q3_V0MV0A", groupName.Data());
  histtitle = TString::Format("%s;q3_V0M;q3_V0A", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 20, 100, 0, 20);
  histName  = TString::Format("%s/q3_V0CV0A", groupName.Data());
  histtitle = TString::Format("%s;q3_V0C;q3_V0A", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 20, 100, 0, 20);

  for (Int_t cent = 0; cent < 11; cent++) {
        // == s == Event plane angle histograms Setting
        histName = TString::Format("%s/hPsi2V0AVsV0C_%d", groupName.Data(), cent);
        histtitle = "Psi2 from V0A vs V0C";
        fHistManager.CreateTH2(histName, histtitle, 50, 0.0, TMath::Pi(), 50, 0.0,TMath::Pi());
        histName = TString::Format("%s/hPsi2V0MVsV0C_%d", groupName.Data(), cent);
        histtitle = "Psi2 from V0M vs V0C";
        fHistManager.CreateTH2(histName, histtitle, 50, 0.0, TMath::Pi(), 50, 0.0,TMath::Pi());
        histName = TString::Format("%s/hPsi2V0MVsV0A_%d", groupName.Data(), cent);
        histtitle = "Psi2 from V0M vs V0A";
        fHistManager.CreateTH2(histName, histtitle, 50, 0.0,TMath::Pi(), 50, 0.0,TMath::Pi());

        histName = TString::Format("%s/hPsi2V0MVsTPCP_%d", groupName.Data(), cent);
        histtitle = "Psi2 from V0M vs TPC postitive";
        fHistManager.CreateTH2(histName, histtitle, 50, 0.0,TMath::Pi(), 50, 0.0,TMath::Pi());
        histName = TString::Format("%s/hPsi2V0MVsTPCN_%d", groupName.Data(), cent);
        histtitle = "Psi2 from V0M vs TPC negative";
        fHistManager.CreateTH2(histName, histtitle, 50, 0.0,TMath::Pi(), 50, 0.0,TMath::Pi());
        histName = TString::Format("%s/hPsi2TPCPVsTPCN_%d", groupName.Data(), cent);
        histtitle = "Psi2 from TPC posi vs nega";
        fHistManager.CreateTH2(histName, histtitle, 50, 0.0,TMath::Pi(), 50, 0.0,TMath::Pi());

      // Comp Dist
      histName = TString::Format("%s/hPsi2V0AMinaV0C_%d", groupName.Data(), cent);
      histtitle = "Psi2 from V0A vs V0C";
      fHistManager.CreateTH1(histName, histtitle, 50, -TMath::Pi(), TMath::Pi());
      histName = TString::Format("%s/hPsi2V0MMinaV0C_%d", groupName.Data(), cent);
      histtitle = "Psi2 from V0M vs V0C";
      fHistManager.CreateTH1(histName, histtitle, 50, -TMath::Pi(), TMath::Pi());
      histName = TString::Format("%s/hPsi2V0MMinaV0A_%d", groupName.Data(), cent);
      histtitle = "Psi2 from V0M vs V0A";
      fHistManager.CreateTH1(histName, histtitle, 50, -TMath::Pi(), TMath::Pi());

      histName = TString::Format("%s/hPsi2V0MMinaTPCP_%d", groupName.Data(), cent);
      histtitle = "Psi2 from V0M vs TPC postitive";
      fHistManager.CreateTH1(histName, histtitle, 50, -TMath::Pi(), TMath::Pi());
      histName = TString::Format("%s/hPsi2V0MMinaTPCN_%d", groupName.Data(), cent);
      histtitle = "Psi2 from V0M vs TPC negative";
      fHistManager.CreateTH1(histName, histtitle, 50, -TMath::Pi(), TMath::Pi());
      histName = TString::Format("%s/hPsi2TPCPMinaTPCN_%d", groupName.Data(), cent);
      histtitle = "Psi2 from TPC posi vs nega";
      fHistManager.CreateTH1(histName, histtitle, 50, -TMath::Pi(), TMath::Pi());


        // profiles for all correlator permutations which are necessary to calculate each second and third order event plane resolution
        TProfile *tempHist;
        THashList *parent;
        histName = TString::Format("hProfV2Resolution_%d", cent);
        histtitle = histName;
        parent = static_cast<THashList *>(fHistManager.FindObject(groupName.Data()));
        tempHist = new TProfile(histName, histtitle, 12, -0.5, 11.5);
        tempHist->GetXaxis()->SetBinLabel(2,  "<cos(2(#Psi_{V0C} - #Psi_{V0A}))>");
        tempHist->GetXaxis()->SetBinLabel(3,  "<cos(2(#Psi_{V0C} - #Psi_{TPC}))>");
        tempHist->GetXaxis()->SetBinLabel(4,  "<cos(2(#Psi_{V0C} - #Psi_{TPC_N}))>");
        tempHist->GetXaxis()->SetBinLabel(5,  "<cos(2(#Psi_{V0C} - #Psi_{TPC_P}))>");
        tempHist->GetXaxis()->SetBinLabel(6,  "<cos(2(#Psi_{V0A} - #Psi_{TPC}))>");
        tempHist->GetXaxis()->SetBinLabel(7,  "<cos(2(#Psi_{V0A} - #Psi_{TPC_N}))>");
        tempHist->GetXaxis()->SetBinLabel(8,  "<cos(2(#Psi_{V0A} - #Psi_{TPC_P}))>");
        tempHist->GetXaxis()->SetBinLabel(9,  "<cos(2(#Psi_{V0} - #Psi_{TPC_N}))>");
        tempHist->GetXaxis()->SetBinLabel(10, "<cos(2(#Psi_{V0} - #Psi_{TPC_P}))>");
        tempHist->GetXaxis()->SetBinLabel(11, "<cos(2(#Psi_{TPC_N} - #Psi_{TPC_P}))>");
        parent->Add(tempHist);

        histName = TString::Format("hProfV3Resolution_%d", cent);
        histtitle = histName;
        parent = static_cast<THashList *>(fHistManager.FindObject(groupName.Data()));
        tempHist = new TProfile(histName, histtitle, 12, -0.5, 11.5);
        tempHist->GetXaxis()->SetBinLabel(2,  "<cos(3(#Psi_{V0C} - #Psi_{V0A}))>");
        tempHist->GetXaxis()->SetBinLabel(3,  "<cos(3(#Psi_{V0C} - #Psi_{TPC}))>");
        tempHist->GetXaxis()->SetBinLabel(4,  "<cos(3(#Psi_{V0C} - #Psi_{TPC_N}))>");
        tempHist->GetXaxis()->SetBinLabel(5,  "<cos(3(#Psi_{V0C} - #Psi_{TPC_P}))>");
        tempHist->GetXaxis()->SetBinLabel(6,  "<cos(3(#Psi_{V0A} - #Psi_{TPC}))>");
        tempHist->GetXaxis()->SetBinLabel(7,  "<cos(3(#Psi_{V0A} - #Psi_{TPC_N}))>");
        tempHist->GetXaxis()->SetBinLabel(8,  "<cos(3(#Psi_{V0A} - #Psi_{TPC_P}))>");
        tempHist->GetXaxis()->SetBinLabel(9,  "<cos(3(#Psi_{V0} - #Psi_{TPC_N}))>");
        tempHist->GetXaxis()->SetBinLabel(10, "<cos(3(#Psi_{V0} - #Psi_{TPC_P}))>");
        tempHist->GetXaxis()->SetBinLabel(11, "<cos(3(#Psi_{TPC_N} - #Psi_{TPC_P}))>");
        parent->Add(tempHist);
  }

}

void AliAnalysisTaskEPCalibForJet::AllocateEPResoHistograms(){
  TString histName;
  TString histtitle;
  TString groupName;
  groupName="EPReso";

  fHistManager.CreateHistoGroup(groupName);
  THashList *parent = static_cast<THashList *>(fHistManager.FindObject(groupName.Data()));
  
  std::cout << " =================   runListSize : " << fUseRunList.size() << std::endl;

  for(Int_t eventNumBin = 0; eventNumBin < fUseRunList.size(); eventNumBin++){
    Int_t runEventNum = stoi(fUseRunList.at(eventNumBin));

    for (Int_t cent = 0; cent < fNcentBins; cent++) {
          // profiles for all correlator permutations which are necessary to calculate each second and third order event plane resolution
          TProfile *tempHist;
          THashList *parent;
          histName = TString::Format("hProfV2Resolution_Cent%d_Run%d", cent, runEventNum);
          histtitle = histName;
          parent = static_cast<THashList *>(fHistManager.FindObject(groupName.Data()));
          tempHist = new TProfile(histName, histtitle, 12, -0.5, 11.5);
          tempHist->GetXaxis()->SetBinLabel(2,  "<cos(2(#Psi_{V0C} - #Psi_{V0A}))>");
          tempHist->GetXaxis()->SetBinLabel(3,  "<cos(2(#Psi_{V0C} - #Psi_{TPC}))>");
          tempHist->GetXaxis()->SetBinLabel(4,  "<cos(2(#Psi_{V0C} - #Psi_{TPC_N}))>");
          tempHist->GetXaxis()->SetBinLabel(5,  "<cos(2(#Psi_{V0C} - #Psi_{TPC_P}))>");
          tempHist->GetXaxis()->SetBinLabel(6,  "<cos(2(#Psi_{V0A} - #Psi_{TPC}))>");
          tempHist->GetXaxis()->SetBinLabel(7,  "<cos(2(#Psi_{V0A} - #Psi_{TPC_N}))>");
          tempHist->GetXaxis()->SetBinLabel(8,  "<cos(2(#Psi_{V0A} - #Psi_{TPC_P}))>");
          tempHist->GetXaxis()->SetBinLabel(9,  "<cos(2(#Psi_{V0} - #Psi_{TPC_N}))>");
          tempHist->GetXaxis()->SetBinLabel(10, "<cos(2(#Psi_{V0} - #Psi_{TPC_P}))>");
          tempHist->GetXaxis()->SetBinLabel(11, "<cos(2(#Psi_{TPC_N} - #Psi_{TPC_P}))>");
          parent->Add(tempHist);
          
          histName = TString::Format("hProfV3Resolution_Cent%d_Run%d", cent, runEventNum);
          histtitle = histName;
          parent = static_cast<THashList *>(fHistManager.FindObject(groupName.Data()));
          tempHist = new TProfile(histName, histtitle, 12, -0.5, 11.5);
          tempHist->GetXaxis()->SetBinLabel(2,  "<cos(3(#Psi_{V0C} - #Psi_{V0A}))>");
          tempHist->GetXaxis()->SetBinLabel(3,  "<cos(3(#Psi_{V0C} - #Psi_{TPC}))>");
          tempHist->GetXaxis()->SetBinLabel(4,  "<cos(3(#Psi_{V0C} - #Psi_{TPC_N}))>");
          tempHist->GetXaxis()->SetBinLabel(5,  "<cos(3(#Psi_{V0C} - #Psi_{TPC_P}))>");
          tempHist->GetXaxis()->SetBinLabel(6,  "<cos(3(#Psi_{V0A} - #Psi_{TPC}))>");
          tempHist->GetXaxis()->SetBinLabel(7,  "<cos(3(#Psi_{V0A} - #Psi_{TPC_N}))>");
          tempHist->GetXaxis()->SetBinLabel(8,  "<cos(3(#Psi_{V0A} - #Psi_{TPC_P}))>");
          tempHist->GetXaxis()->SetBinLabel(9,  "<cos(3(#Psi_{V0} - #Psi_{TPC_N}))>");
          tempHist->GetXaxis()->SetBinLabel(10, "<cos(3(#Psi_{V0} - #Psi_{TPC_P}))>");
          tempHist->GetXaxis()->SetBinLabel(11, "<cos(3(#Psi_{TPC_N} - #Psi_{TPC_P}))>");
          parent->Add(tempHist);
    }
    
  }


}


void AliAnalysisTaskEPCalibForJet::AllocateBkgHistograms()
{
  TString histName;
  TString histtitle;
  TString groupName;
  groupName="BackgroundFit";
  fHistManager.CreateHistoGroup(groupName);

  // == s == cdf and pdf of chisquare distribution #############################
  //  = v2 and v3 combind fit local rho =
  histName = TString::Format("%s/hPvalueCDF_lRhoCombinFit", groupName.Data());
  histtitle = TString::Format("%s; CDF #chi^{2}", histName.Data());
  fHistManager.CreateTH1(histName, histtitle, 50, 0, 1);
  
  histName = TString::Format("%s/hPvalueCDFCent_lRhoCombinFit", groupName.Data());
  histtitle = TString::Format("%s; centrality; p-value", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 40, 0, 100, 40, 0, 1);
  
  histName = TString::Format("%s/hChi2Cent_lRhoCombinFit", groupName.Data());
  histtitle = TString::Format("%s; CDF #chi^{2}; #tilde{#chi^{2}}", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 5);

  histName = TString::Format("%s/hPChi2_lRhoCombinFit", groupName.Data());
  histtitle = TString::Format("%s; p-value; #tilde{#chi^{2}}", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 1000, 0, 1, 100, 0, 5);

  histName = TString::Format("%s/hPvalueCDFROOT_lRhoCombinFit", groupName.Data());
  histtitle = TString::Format("%s; CDF #chi^{2} ROOT", histName.Data());
  fHistManager.CreateTH1(histName, histtitle, 50, 0, 1);

  histName = TString::Format("%s/hPvalueCDFROOTCent_lRhoCombinFit", groupName.Data());
  histtitle = TString::Format("%s; centrality; p-value ROOT", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 40, 0, 100, 45, 0, 1);

  histName = TString::Format("%s/hChi2ROOTCent_lRhoCombinFit", groupName.Data());
  histtitle = TString::Format("%s; p-value; #tilde{#chi^{2}}", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 45, 0, 1);

  histName = TString::Format("%s/hPChi2ROOT_lRhoCombinFit", groupName.Data());
  histtitle = TString::Format("%s;CDF #chi^{2}; #tilde{#chi^{2}} ROOT", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 1000, 0, 1, 100, 0, 5);

  //  = v2  fit local rho =
  histName = TString::Format("%s/hPvalueCDF_lRhoV2Fit", groupName.Data());
  histtitle = TString::Format("%s; CDF #chi^{2}", histName.Data());
  fHistManager.CreateTH1(histName, histtitle, 50, 0, 1);
  
  histName = TString::Format("%s/hPvalueCDFCent_lRhoV2Fit", groupName.Data());
  histtitle = TString::Format("%s; centrality; p-value", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 40, 0, 100, 40, 0, 1);
  
  histName = TString::Format("%s/hChi2Cent_lRhoV2Fit", groupName.Data());
  histtitle = TString::Format("%s; CDF #chi^{2}; #tilde{#chi^{2}}", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 5);

  histName = TString::Format("%s/hPChi2_lRhoV2Fit", groupName.Data());
  histtitle = TString::Format("%s; p-value; #tilde{#chi^{2}}", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 1000, 0, 1, 100, 0, 5);

  histName = TString::Format("%s/hPvalueCDFROOT_lRhoV2Fit", groupName.Data());
  histtitle = TString::Format("%s; CDF #chi^{2} ROOT", histName.Data());
  fHistManager.CreateTH1(histName, histtitle, 50, 0, 1);

  histName = TString::Format("%s/hPvalueCDFROOTCent_lRhoV2Fit", groupName.Data());
  histtitle = TString::Format("%s; centrality; p-value ROOT", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 40, 0, 100, 45, 0, 1);

  histName = TString::Format("%s/hChi2ROOTCent_lRhoV2Fit", groupName.Data());
  histtitle = TString::Format("%s; p-value; #tilde{#chi^{2}}", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 45, 0, 1);

  histName = TString::Format("%s/hPChi2ROOT_lRhoV2Fit", groupName.Data());
  histtitle = TString::Format("%s;CDF #chi^{2}; #tilde{#chi^{2}} ROOT", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 1000, 0, 1, 100, 0, 5);

    //  = fit global rho (rho0) =
  histName = TString::Format("%s/hPvalueCDF_gRhoFit", groupName.Data());
  histtitle = TString::Format("%s; CDF #chi^{2}", histName.Data());
  fHistManager.CreateTH1(histName, histtitle, 50, 0, 1);
  
  histName = TString::Format("%s/hPvalueCDFCent_gRhoFit", groupName.Data());
  histtitle = TString::Format("%s; centrality; p-value", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 40, 0, 100, 40, 0, 1);
  
  histName = TString::Format("%s/hChi2Cent_gRhoFit", groupName.Data());
  histtitle = TString::Format("%s; CDF #chi^{2}; #tilde{#chi^{2}}", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 5);

  histName = TString::Format("%s/hPChi2_gRhoFit", groupName.Data());
  histtitle = TString::Format("%s; p-value; #tilde{#chi^{2}}", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 1000, 0, 1, 100, 0, 5);

  histName = TString::Format("%s/hPvalueCDFROOT_gRhoFit", groupName.Data());
  histtitle = TString::Format("%s; CDF #chi^{2} ROOT", histName.Data());
  fHistManager.CreateTH1(histName, histtitle, 50, 0, 1);

  histName = TString::Format("%s/hPvalueCDFROOTCent_gRhoFit", groupName.Data());
  histtitle = TString::Format("%s; centrality; p-value ROOT", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 40, 0, 100, 45, 0, 1);

  histName = TString::Format("%s/hChi2ROOTCent_gRhoFit", groupName.Data());
  histtitle = TString::Format("%s; p-value; #tilde{#chi^{2}}", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 45, 0, 1);

  histName = TString::Format("%s/hPChi2ROOT_gRhoFit", groupName.Data());
  histtitle = TString::Format("%s;CDF #chi^{2}; #tilde{#chi^{2}} ROOT", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 1000, 0, 1, 100, 0, 5);
  // == e == cdf and pdf of chisquare distribution #############################


  
  histName = TString::Format("%s/hRhoVsMult", groupName.Data());
  histtitle = TString::Format("%s; multiplicity; #rho [GeV/c]", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 4000, 100, 0, 250);
  
  histName = TString::Format("%s/hRhoVsCent", groupName.Data());
  histtitle = TString::Format("%s; centrality; #rho [GeV/c]", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 250);
  
  histName = TString::Format("%s/hRhoAVsMult", groupName.Data());
  histtitle = TString::Format("%s; multiplicity; #rho * A (jet) [GeV/c]", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 4000, 100, 0, 50);
  
  histName = TString::Format("%s/hRhoAVsCent", groupName.Data());
  histtitle = TString::Format("%s; centrality; #rho * A (jet) [GeV/c]", histName.Data());
  fHistManager.CreateTH2(histName, histtitle, 100, 0, 100, 100, 0, 50);

}
/*
 * This function allocates the histograms for basic tracking QA.
 * A set of histograms (pT, eta, phi, difference between kinematic properties
 * at the vertex and at the EMCal surface, number of tracks) is allocated
 * per each particle container and per each centrality bin.
 */
void AliAnalysisTaskEPCalibForJet::AllocateTrackHistograms()
{
  TString histName;
  TString histtitle;
  TString groupName;
  AliParticleContainer* partCont = 0;
  TIter next(&fParticleCollArray);
  while ((partCont = static_cast<AliParticleContainer*>(next()))) {
    groupName = partCont->GetName();
    // Protect against creating the histograms twice
    if (fHistManager.FindObject(groupName)) {
      AliWarning(TString::Format("%s: Found groupName %s in hist manager. The track containers will be filled into the same histograms.", GetName(), groupName.Data()));
      continue;
    }
    fHistManager.CreateHistoGroup(groupName);

    // adding histo for counting events
    histName = TString::Format("Hist nEvents");
    histtitle = TString::Format("Number of Events");
    fHistManager.CreateTH1(histName, histtitle, 1, 0.0, 1.0);

    for (Int_t cent = 0; cent < 11; cent++) {
      histName = TString::Format("%s/hTrackPt_%d", groupName.Data(), cent);
      histtitle = TString::Format("%s;#it{p}_{T,track} (GeV/#it{c});counts", histName.Data());
      fHistManager.CreateTH1(histName, histtitle, fNbins / 2, fMinBinPt, fMaxBinPt / 2);

      histName = TString::Format("%s/hTrackPhi_%d", groupName.Data(), cent);
      histtitle = TString::Format("%s;#it{#phi}_{track};counts", histName.Data());
      fHistManager.CreateTH1(histName, histtitle, fNbins / 2, 0, TMath::TwoPi());

      histName = TString::Format("%s/hTrackEta_%d", groupName.Data(), cent);
      histtitle = TString::Format("%s;#it{#eta}_{track};counts", histName.Data());
      fHistManager.CreateTH1(histName, histtitle, fNbins / 6, -1, 1);

      histName = TString::Format("%s/hNTracks_%d", groupName.Data(), cent);
      histtitle = TString::Format("%s;number of tracks;events", histName.Data());
      if (fForceBeamType != kpp) fHistManager.CreateTH1(histName, histtitle, 500, 0, 5000);
      else fHistManager.CreateTH1(histName, histtitle, 200, 0, 200);
      
      // == e == Event plane angle histograms Setting
    }
  }

  histName = "fHistSumNTracks";
  histtitle = TString::Format("%s;Sum of n tracks;events", histName.Data());
  if (fForceBeamType != kpp) fHistManager.CreateTH1(histName, histtitle, 500, 0, 5000);
  else fHistManager.CreateTH1(histName, histtitle, 200, 0, 200);
}


/**
 * This function is executed automatically for the first event.
 * Some extra initialization can be performed here.
 */
void AliAnalysisTaskEPCalibForJet::ExecOnce()
{
  fAOD = dynamic_cast<AliAODEvent*>(InputEvent());
  
  if(!fLocalRho) {
    fLocalRho = new AliLocalRhoParameter(fLocalRhoName.Data(), 0); 
      if(!(InputEvent()->FindListObject(fLocalRho->GetName()))) {
        InputEvent()->AddObject(fLocalRho);
      } else {
        AliFatal(Form("%s: Container with name %s already present. Aborting", \
        GetName(), fLocalRho->GetName()));
      }
  }
  
  
  AliAnalysisTaskEmcalJet::ExecOnce();
  if(!GetJetContainer()) AliFatal(Form("%s: Couldn't find jet container. Aborting !", GetName()));
  
}


/**
 * Run analysis code here, if needed.
 * It will be executed before FillHistograms().
 * If this function return kFALSE, FillHistograms() will *not*
 * be executed for the current event
 * @return Always kTRUE
 */
Bool_t AliAnalysisTaskEPCalibForJet::Run()
{
  // std::cout << "ChecKuma Run Number === " << CheckRunNum << "==================" << std::endl;
  
  CheckRunNum++;
  if(!fEventCuts.AcceptEvent(InputEvent())) return kFALSE;
  
  if(fRejectTPCPileup)   {
    fEventCuts.SetRejectTPCPileupWithITSTPCnCluCorr(kTRUE,1);
    Bool_t acceptEventCuts = fEventCuts.AcceptEvent(InputEvent());
    if(!acceptEventCuts) { 
        // fHistNEvents->Fill(5);
        return kFALSE;}
  }

  if(fPileupCut){
    SetupPileUpRemovalFunctions();
    Bool_t kPileupCutEvent = CheckEventIsPileUp2018();
    if(kPileupCutEvent) return kFALSE;
  }

  ReSetValuable();
  if(fGainCalibQA) DoMeasureChGainDiff();
  if(fEPQA) DoEventPlane();
  if(fBkgQA){
    SetModulationRhoFit();
    MeasureBkg();
  }
  
  return kTRUE;
}

void AliAnalysisTaskEPCalibForJet::DoMeasureChGainDiff(){
  TString histName;
  TString groupName;
  groupName="GainCalib";
  
  AliAODVZERO* fAodV0 = dynamic_cast<AliAODVZERO*>(fAOD->GetVZEROData());
  AliVEvent *fVevent = dynamic_cast<AliVEvent*>(InputEvent());
  const AliVVertex *pointVtx = fVevent->GetPrimaryVertex();
  Double_t fVtxZ = -999;
  fVtxZ  = pointVtx->GetZ();
  
  Int_t ibinV0 = 0;
  Double_t fMultV0 = 0.;
  
  
  for(int iV0 = 0; iV0 < 64; iV0++) { //0-31 is V0C, 32-63 VOA
    fMultV0 = fAodV0->GetMultiplicity(iV0);
    
    histName = TString::Format("%s/hAvgV0ChannelsvsVz_%d", groupName.Data(), fRunNumber);
    TProfile2D* tempProfile2DHist = dynamic_cast<TProfile2D *>(fHistManager.FindObject(histName));
    if(!tempProfile2DHist){
      Fatal("THistManager::FillTProfile", "Histogram %s not found in parent group %s", histName.Data(), groupName.Data());
    }
    tempProfile2DHist->Fill(fVtxZ, iV0, fMultV0);//?????
    // fHistManager.FillProfile(histName, fVtxZ, iV0, fMultV0);
  }
  
}


Bool_t AliAnalysisTaskEPCalibForJet::DoEventPlane(){

  if (!fAOD && AODEvent() && IsStandardAOD()) {
      // In case there is an AOD handler writing a standard AOD, use the AOD
      // event in memory rather than the input (ESD) event.
      fAOD = dynamic_cast<AliAODEvent*> (AODEvent());
  }
  if (!fAOD) {
      AliWarning("AliAnalysisTaskJetQnVectors::Exec(): bad AOD");
      return kFALSE;
  }
  AliAODHandler* aodHandler = static_cast<AliAODHandler*>((AliAnalysisManager::GetAnalysisManager())->GetInputEventHandler());
  if(!aodHandler) {
    AliWarning("AliAnalysisTaskJetQnVectors::Exec(): No AliInputEventHandler!");
    return kFALSE;
  }
  
  Int_t iCentBin = GetCentBin();

  if(fQnVCalibType == "kOrig"){
    //== s == qn Calibration  111111111111111111111111111111111111111111111111111
    // std::cout << "bef calib (qx,qy) = " << q2VecV0M[0] << "," << q2VecV0M[1] << ")" << std::endl;
    QnV0GainCalibration();
    
    // std::cout << "gain calib (qx,qy) = " << q2VecV0M[0] << "," << q2VecV0M[1] << ")" << std::endl;
    QnRecenteringCalibration();
    // std::cout << "recent calib (qx,qy) = " << q2VecV0C[0] << "," << q2VecV0C[1] << ")" << std::endl;

    MeasureQnTPC();
  }
  else if(fQnVCalibType == "kJeHand"){
      if(!fMultV0BefCorPfpx){
          AliWarning("AliAnalysisTaskEPCalibForJet::: No fMultV0BefCorPfpx!!!!!!");
          return kFALSE;
      }
      QnJEHandlarEPGet();
  }

  //== s == combin V0C and V0A  ################################################
  Double_t q2ChiV0C = 0.;
  Double_t q2ChiV0A = 0.;
  Double_t q3ChiV0C = 0.;
  Double_t q3ChiV0A = 0.;
  if(fV0Combin){
    Double_t fV2ResoV0C[11] = {0.5458621293332865,0.5458621293332865, 0.7409031796402847, 0.7694233658175791, 0.736982496675723, 0.6567838857944003, 0.5264298565489421, 0.3576314845145673, 0.1895442026317924, 0.06248559291441631, 1.};
    Double_t fV2ResoV0A[11] = {0.4516199203482921,0.4516199203482921, 0.651079134099348, 0.6816279497060079, 0.6454100760087523, 0.5601254261892471, 0.43354044533091385, 0.2838938710925333, 0.14436788175906987, 0.04627416839868842, 1.};
    q2ChiV0A = fV2ResoV0C[iCentBin];
    q2ChiV0C = fV2ResoV0A[iCentBin];
    // q2ChiV0A = CalculateEventPlaneChi(psiReso);
    // q2ChiV0C = CalculateEventPlaneChi(psiReso);

    Double_t fV3ResoV0C[11] = {0.32536469218249076,0.32536469218249076, 0.32542870963185283, 0.2948265276595313, 0.2487153491419922, 0.19116966926486725, 0.12589673300846377, 0.06232287365736147, 0.023816671467611652, 0.0012792198401852786, 1.};
    Double_t fV3ResoV0A[11] = {0.24342843174412196,0.24342843174412196, 0.24312473236181362, 0.2194925746532911, 0.18294633371296457, 0.13776071180957172, 0.09003456785584461, 0.04453092389483927, 0.00924581119180068, 0.0006331736643643343, 1.};
    q3ChiV0A = fV3ResoV0C[iCentBin];
    q3ChiV0C = fV3ResoV0A[iCentBin];
    // q3ChiV0A = CalculateEventPlaneChi(psiReso);
    // q3ChiV0C = CalculateEventPlaneChi(psiReso);
  }else {
    q2ChiV0A = 1.0;
    q3ChiV0A = 1.0;
  }
  q2VecV0M[0] = q2ChiV0C*q2ChiV0C*q2VecV0C[0] + q2ChiV0A*q2ChiV0A*q2VecV0A[0];
  q2VecV0M[1] = q2ChiV0C*q2ChiV0C*q2VecV0C[1] + q2ChiV0A*q2ChiV0A*q2VecV0A[1];
  q3VecV0M[0] = q3ChiV0C*q2ChiV0C*q3VecV0C[0] + q3ChiV0A*q2ChiV0A*q3VecV0A[0];
  q3VecV0M[1] = q3ChiV0C*q2ChiV0C*q3VecV0C[1] + q3ChiV0A*q2ChiV0A*q3VecV0A[1];

  if(0){
      std::cout << "Aft Recent Calib Qx (V0C, V0A) = ("  << q2VecV0C[0] << ", " << q2VecV0A[0] << ")"\
        << ", Qy (V0C, V0A) = ("  << q2VecV0C[1] << ", " << q2VecV0A[1] << ")"<< std::endl;
      
      std::cout << "q2 (V0M, V0C, V0A) = ("  << q2V0[0] << ", " << q2V0[1] << ", "  << q2V0[2] << ")"<< std::endl;
    
    }
  //== s == combin V0C and V0A  ################################################

  psi2V0[0] = CalcEPAngle(q2VecV0M[0], q2VecV0M[1]);
  psi2V0[1] = CalcEPAngle(q2VecV0C[0], q2VecV0C[1]);
  psi2V0[2] = CalcEPAngle(q2VecV0A[0], q2VecV0A[1]);

  psi3V0[0] = CalcEPAngle(q3VecV0M[0], q3VecV0M[1]);
  psi3V0[1] = CalcEPAngle(q3VecV0C[0], q3VecV0C[1]);
  psi3V0[2] = CalcEPAngle(q3VecV0A[0], q3VecV0A[1]);

  psi2Tpc[0] = CalcEPAngle(q2VecTpcM[0], q2VecTpcM[1]);
  psi2Tpc[1] = CalcEPAngle(q2VecTpcN[0], q2VecTpcN[1]);
  psi2Tpc[2] = CalcEPAngle(q2VecTpcP[0], q2VecTpcP[1]);

  psi3Tpc[0] = CalcEPAngle(q3VecTpcM[0], q3VecTpcM[1]);
  psi3Tpc[1] = CalcEPAngle(q3VecTpcN[0], q3VecTpcN[1]);
  psi3Tpc[2] = CalcEPAngle(q3VecTpcP[0], q3VecTpcP[1]);
  //== e == qn Calibration  111111111111111111111111111111111111111111111111111
  
  TString histName;
  TString groupName;
  groupName="EventPlane";

  histName = TString::Format("%s/hCentrality", groupName.Data());
  fHistManager.FillTH1(histName, fCent);
  
  histName = TString::Format("%s/CentQ2x_V0M", groupName.Data());
  fHistManager.FillTH2(histName, fCent, q2VecV0M[0]);
  histName = TString::Format("%s/CentQ2y_V0M", groupName.Data());
  fHistManager.FillTH2(histName, fCent, q2VecV0M[1]);
  histName = TString::Format("%s/CentQ2x_V0C", groupName.Data());
  fHistManager.FillTH2(histName, fCent, q2VecV0C[0]);
  histName = TString::Format("%s/CentQ2y_V0C", groupName.Data());
  fHistManager.FillTH2(histName, fCent, q2VecV0C[1]);
  histName = TString::Format("%s/CentQ2x_V0A", groupName.Data());
  fHistManager.FillTH2(histName, fCent, q2VecV0A[0]);
  histName = TString::Format("%s/CentQ2y_V0A", groupName.Data());
  fHistManager.FillTH2(histName, fCent, q2VecV0A[1]);
  
  histName = TString::Format("%s/CentQ3x_V0M", groupName.Data());
  fHistManager.FillTH2(histName, fCent, q3VecV0M[0]);
  histName = TString::Format("%s/CentQ3y_V0M", groupName.Data());
  fHistManager.FillTH2(histName, fCent, q3VecV0M[1]);
  histName = TString::Format("%s/CentQ3x_V0C", groupName.Data());
  fHistManager.FillTH2(histName, fCent, q3VecV0C[0]);
  histName = TString::Format("%s/CentQ3y_V0C", groupName.Data());
  fHistManager.FillTH2(histName, fCent, q3VecV0C[1]);
  histName = TString::Format("%s/CentQ3x_V0A", groupName.Data());
  fHistManager.FillTH2(histName, fCent, q3VecV0A[0]);
  histName = TString::Format("%s/CentQ3y_V0A", groupName.Data());
  fHistManager.FillTH2(histName, fCent, q3VecV0A[1]);

  
  histName = TString::Format("%s/hPsi2V0AVsV0C_%d", groupName.Data(), iCentBin);
  fHistManager.FillTH2(histName, psi2V0[2], psi2V0[1]);
  histName = TString::Format("%s/hPsi2V0MVsV0C_%d", groupName.Data(), iCentBin);
  fHistManager.FillTH2(histName, psi2V0[0], psi2V0[1]);
  histName = TString::Format("%s/hPsi2V0MVsV0A_%d", groupName.Data(), iCentBin);
  fHistManager.FillTH2(histName, psi2V0[0], psi2V0[2]);
  histName = TString::Format("%s/hPsi2V0MVsTPCP_%d", groupName.Data(), iCentBin);
  fHistManager.FillTH2(histName, psi2V0[0], psi2Tpc[1]);
  histName = TString::Format("%s/hPsi2V0MVsTPCN_%d", groupName.Data(), iCentBin);
  fHistManager.FillTH2(histName, psi2V0[0], psi2Tpc[2]);
  histName = TString::Format("%s/hPsi2TPCPVsTPCN_%d", groupName.Data(), iCentBin);
  fHistManager.FillTH2(histName, psi2Tpc[1], psi2Tpc[2]);


  histName = TString::Format("%s/hPsi2V0AMinaV0C_%d", groupName.Data(), iCentBin);
  fHistManager.FillTH1(histName, psi2V0[2]-psi2V0[1]);
  histName = TString::Format("%s/hPsi2V0MMinaV0C_%d", groupName.Data(), iCentBin);
  fHistManager.FillTH1(histName, psi2V0[0]-psi2V0[1]);
  histName = TString::Format("%s/hPsi2V0MMinaV0A_%d", groupName.Data(), iCentBin);
  fHistManager.FillTH1(histName, psi2V0[0]-psi2V0[2]);
  histName = TString::Format("%s/hPsi2V0MMinaTPCP_%d", groupName.Data(), iCentBin);
  fHistManager.FillTH1(histName, psi2V0[0]-psi2Tpc[1]);
  histName = TString::Format("%s/hPsi2V0MMinaTPCN_%d", groupName.Data(), iCentBin);
  fHistManager.FillTH1(histName, psi2V0[0]-psi2Tpc[2]);
  histName = TString::Format("%s/hPsi2TPCPMinaTPCN_%d", groupName.Data(), iCentBin);
  fHistManager.FillTH1(histName, psi2Tpc[1]-psi2Tpc[2]);
  
  
  histName = TString::Format("%s/hProfV2Resolution_%d", groupName.Data(), iCentBin);
  fHistManager.FillProfile(histName, 1., TMath::Cos(2.*(psi2V0[1] - psi2V0[2])));
  fHistManager.FillProfile(histName, 2., TMath::Cos(2.*(psi2V0[1] - psi2Tpc[0])));
  fHistManager.FillProfile(histName, 3., TMath::Cos(2.*(psi2V0[1] - psi2Tpc[1])));
  fHistManager.FillProfile(histName, 4., TMath::Cos(2.*(psi2V0[1] - psi2Tpc[2])));
  fHistManager.FillProfile(histName, 5., TMath::Cos(2.*(psi2V0[2] - psi2Tpc[0])));
  fHistManager.FillProfile(histName, 6., TMath::Cos(2.*(psi2V0[2] - psi2Tpc[1])));
  fHistManager.FillProfile(histName, 7., TMath::Cos(2.*(psi2V0[2] - psi2Tpc[2])));
  fHistManager.FillProfile(histName, 8., TMath::Cos(2.*(psi2V0[0] - psi2Tpc[1]))); 
  fHistManager.FillProfile(histName, 9., TMath::Cos(2.*(psi2V0[0] - psi2Tpc[2])));
  fHistManager.FillProfile(histName, 10., TMath::Cos(2.*(psi2Tpc[1] - psi2Tpc[2])));
      
  histName = TString::Format("%s/hProfV3Resolution_%d", groupName.Data(), iCentBin);
  fHistManager.FillProfile(histName, 1., TMath::Cos(2.*(psi3V0[1] - psi3V0[2])));
  fHistManager.FillProfile(histName, 2., TMath::Cos(2.*(psi3V0[1] - psi3Tpc[0])));
  fHistManager.FillProfile(histName, 3., TMath::Cos(2.*(psi3V0[1] - psi3Tpc[1])));
  fHistManager.FillProfile(histName, 4., TMath::Cos(2.*(psi3V0[1] - psi3Tpc[2])));
  fHistManager.FillProfile(histName, 5., TMath::Cos(2.*(psi3V0[2] - psi3Tpc[0])));
  fHistManager.FillProfile(histName, 6., TMath::Cos(2.*(psi3V0[2] - psi3Tpc[1])));
  fHistManager.FillProfile(histName, 7., TMath::Cos(2.*(psi3V0[2] - psi3Tpc[2])));
  fHistManager.FillProfile(histName, 8., TMath::Cos(2.*(psi3V0[0] - psi3Tpc[1])));
  fHistManager.FillProfile(histName, 9., TMath::Cos(2.*(psi3V0[0] - psi3Tpc[2])));
  fHistManager.FillProfile(histName, 10., TMath::Cos(2.*(psi3Tpc[1] - psi3Tpc[2])));

  histName = TString::Format("%s/Centq2_V0M", groupName.Data());
  fHistManager.FillTH2(histName, fCent, q2V0[0]);
  histName = TString::Format("%s/Centq2_V0C", groupName.Data());
  fHistManager.FillTH2(histName, fCent, q2V0[1]);
  histName = TString::Format("%s/Centq2_V0A", groupName.Data());
  fHistManager.FillTH2(histName, fCent, q2V0[2]);

  histName = TString::Format("%s/q2_V0MV0C", groupName.Data());
  fHistManager.FillTH2(histName, q2V0[0], q2V0[1]);
  histName = TString::Format("%s/q2_V0MV0A", groupName.Data());
  fHistManager.FillTH2(histName, q2V0[0], q2V0[2]);
  histName = TString::Format("%s/q2_V0CV0A", groupName.Data());
  fHistManager.FillTH2(histName, q2V0[1], q2V0[2]);


  histName = TString::Format("%s/Centq3_V0M", groupName.Data());
  fHistManager.FillTH2(histName, fCent, q3V0[0]);
  histName = TString::Format("%s/Centq3_V0C", groupName.Data());
  fHistManager.FillTH2(histName, fCent, q3V0[1]);
  histName = TString::Format("%s/Centq3_V0A", groupName.Data());
  fHistManager.FillTH2(histName, fCent, q3V0[2]);

  histName = TString::Format("%s/q3_V0MV0C", groupName.Data());
  fHistManager.FillTH2(histName, q3V0[0], q3V0[1]);
  histName = TString::Format("%s/q3_V0MV0A", groupName.Data());
  fHistManager.FillTH2(histName, q3V0[0], q3V0[2]);
  histName = TString::Format("%s/q3_V0CV0A", groupName.Data());
  fHistManager.FillTH2(histName, q3V0[1], q3V0[2]);

  if(fEPResoQA){
    groupName="EPReso";

    histName = TString::Format("%s/hProfV2Resolution_Cent%d_Run%d", groupName.Data(), iCentBin, fRunNumber);
    fHistManager.FillProfile(histName, 1., TMath::Cos(2.*(psi2V0[1] - psi2V0[2])));
    fHistManager.FillProfile(histName, 2., TMath::Cos(2.*(psi2V0[1] - psi2Tpc[0])));
    fHistManager.FillProfile(histName, 3., TMath::Cos(2.*(psi2V0[1] - psi2Tpc[1])));
    fHistManager.FillProfile(histName, 4., TMath::Cos(2.*(psi2V0[1] - psi2Tpc[2])));
    fHistManager.FillProfile(histName, 5., TMath::Cos(2.*(psi2V0[2] - psi2Tpc[0])));
    fHistManager.FillProfile(histName, 6., TMath::Cos(2.*(psi2V0[2] - psi2Tpc[1])));
    fHistManager.FillProfile(histName, 7., TMath::Cos(2.*(psi2V0[2] - psi2Tpc[2])));
    fHistManager.FillProfile(histName, 8., TMath::Cos(2.*(psi2V0[0] - psi2Tpc[1])));
    fHistManager.FillProfile(histName, 9., TMath::Cos(2.*(psi2V0[0] - psi2Tpc[2])));
    fHistManager.FillProfile(histName, 10., TMath::Cos(2.*(psi2Tpc[1] - psi2Tpc[2])));
    
    histName = TString::Format("%s/hProfV3Resolution_Cent%d_Run%d", groupName.Data(), iCentBin, fRunNumber);
    fHistManager.FillProfile(histName, 1., TMath::Cos(2.*(psi3V0[1] - psi3V0[2])));
    fHistManager.FillProfile(histName, 2., TMath::Cos(2.*(psi3V0[1] - psi3Tpc[0])));
    fHistManager.FillProfile(histName, 3., TMath::Cos(2.*(psi3V0[1] - psi3Tpc[1])));
    fHistManager.FillProfile(histName, 4., TMath::Cos(2.*(psi3V0[1] - psi3Tpc[2])));
    fHistManager.FillProfile(histName, 5., TMath::Cos(2.*(psi3V0[2] - psi3Tpc[0])));
    fHistManager.FillProfile(histName, 6., TMath::Cos(2.*(psi3V0[2] - psi3Tpc[1])));
    fHistManager.FillProfile(histName, 7., TMath::Cos(2.*(psi3V0[2] - psi3Tpc[2])));
    fHistManager.FillProfile(histName, 8., TMath::Cos(2.*(psi3V0[0] - psi3Tpc[1])));
    fHistManager.FillProfile(histName, 9., TMath::Cos(2.*(psi3V0[0] - psi3Tpc[2])));
    fHistManager.FillProfile(histName, 10., TMath::Cos(2.*(psi3Tpc[1] - psi3Tpc[2])));
  }

  return kTRUE;
}


void AliAnalysisTaskEPCalibForJet::SetModulationRhoFit() 
{
  // set modulation fit
  TString histName;

  if(fFitModulation) delete fFitModulation;
  fFitModulation = 0x0;
  const char * fitFunction = "[0]*(1.+2.*([1]*TMath::Cos(2.*(x-[2]))+[3]*TMath::Cos(3.*(x-[4]))))";
  switch (fFitModulationType)  {
    case kNoFit : { fFitModulation = new TF1("fix_kNoFit", "[0]", 0, TMath::TwoPi()); } break;
    case kV2 : {
      fitFunction = "[0]*([1]+[2]*[3]*TMath::Cos([2]*(x-[4])))";
      fFitModulation = new TF1("fit_kV2", fitFunction, 0, TMath::TwoPi());
      fFitModulation->SetParameter(0, 0.);  // normalization
      fFitModulation->SetParameter(1, 0.2); // v2
    } break;
    case kCombined: {
      fitFunction = "[0]*(1.+2.*([1]*TMath::Cos(2.*(x-[2]))+[3]*TMath::Cos(3.*(x-[4]))))";
      fFitModulation = new TF1("fit_kCombined", fitFunction, 0, TMath::TwoPi());
      fFitModulation->SetParameter(0, 0.);       // normalization
      fFitModulation->SetParameter(1, 0.2);      // v2
      fFitModulation->SetParameter(3, 0.2);      // v3
    } break;
    default : { // for the combined fit, the 'direct fourier series' or the user supplied vn values we use v2 and v3
      fitFunction = "[0]*(1.+2.*([1]*TMath::Cos(2.*(x-[2]))+[3]*TMath::Cos(3.*(x-[4]))))";
      fFitModulation = new TF1("fit_kCombined", fitFunction, 0, TMath::TwoPi());
      fFitModulation->SetParameter(0, 0.);       // normalization
      fFitModulation->SetParameter(1, 0.2);      // v2
      fFitModulation->SetParameter(3, 0.2);      // v3
    } break;
  }

  

  if(hBkgTracks) delete hBkgTracks;
  histName = "hBkgTracks";
  // hBkgTracks = new TH1F(histName, histName, 100, 0.0, TMath::TwoPi());
  hBkgTracks = new TH1F(histName, histName, 25, 0.0, TMath::TwoPi());
}


void AliAnalysisTaskEPCalibForJet::MeasureBkg(){
  TString histName;
  
  UInt_t sumAcceptedTracks = 0;
  AliParticleContainer* partCont = 0;
  TIter next(&fParticleCollArray);
  while ((partCont = static_cast<AliParticleContainer*>(next()))) {
    // groupname = partCont->GetName();
    for(auto part : partCont->accepted()) {
      if (!part) continue;
      if (partCont->GetLoadedClass()->InheritsFrom("AliVTrack")) {
        const AliVTrack* track = static_cast<const AliVTrack*>(part);

        Float_t trackDeltaPhi = track->Phi() - psi2V0[0];
        if (trackDeltaPhi < 0.0) trackDeltaPhi += TMath::TwoPi();
        hBkgTracks->Fill(trackDeltaPhi);
      }
    }
  }
  
  fLocalRho->SetVal(fRho->GetVal());
  fFitModulation->SetParameter(0, fLocalRho->GetVal());
  fFitModulation->FixParameter(2, psi2V0[0]);
  fFitModulation->FixParameter(4, psi3V0[0]);
  
  hBkgTracks->Fit(fFitModulation, "N0Q");

  fLocalRho->SetLocalRho(fFitModulation);
  // fLocalRho->SetVal(fRho->GetVal());

  BkgFitEvaluation();
}


Double_t AliAnalysisTaskEPCalibForJet::CalcEPReso(Int_t n, \
  Double_t &psiA, Double_t &psiB, Double_t &psiC){
  
  Double_t vnReso = -999.;
  vnReso = TMath::Sqrt((TMath::Abs(TMath::Cos(n*(psiA - psiB))) \
                          * TMath::Abs(TMath::Cos(n*(psiA - psiC)))) \
                        / TMath::Abs(TMath::Cos(n*(psiB - psiC))));

  return vnReso;
}


void AliAnalysisTaskEPCalibForJet::MeasureTpcEPQA(){
  TString histName;
  TString groupName;

  Double_t EtaAcc = 0.9;
  UInt_t sumAcceptedTracks = 0;
  AliParticleContainer* partCont = 0;
  
  TIter next(&fParticleCollArray);
  
  while ((partCont = static_cast<AliParticleContainer*>(next()))) {
    Int_t iCentBin = GetCentBin();

    groupName = partCont->GetName();
    // counting events
    histName = TString::Format("Hist nEvents");
    fHistManager.FillTH1(histName, 0.5);
    
    UInt_t count = 0;
    for(auto part : partCont->accepted()) {
      if (!part) continue;
      count++;
      
      histName = TString::Format("%s/hTrackPt_%d", groupName.Data(), iCentBin);
      fHistManager.FillTH1(histName, part->Pt());

      histName = TString::Format("%s/hTrackPhi_%d", groupName.Data(), iCentBin);
      fHistManager.FillTH1(histName, part->Phi());

      histName = TString::Format("%s/hTrackEta_%d", groupName.Data(), iCentBin);
      fHistManager.FillTH1(histName, part->Eta());

      if (partCont->GetLoadedClass()->InheritsFrom("AliVTrack")) {
        const AliVTrack* track = static_cast<const AliVTrack*>(part);

        // Filling histos for angle relative to event plane
        Double_t phiMinusPsi2 = track->Phi() - psi2V0[0];
        Double_t phiMinusPsi3 = track->Phi() - psi3V0[0];
        if (phiMinusPsi2 < 0.0) phiMinusPsi2 += TMath::TwoPi();
        if (phiMinusPsi3 < 0.0) phiMinusPsi3 += TMath::TwoPi();
        // histName = TString::Format("%s/hTrackPhiMinusPsi2_%d", groupName.Data(), iCentBin);
        // fHistManager.FillTH1(histName, phiMinusPsi2);
        // histName = TString::Format("%s/hTrackPhiMinusPsi3_%d", groupName.Data(), iCentBin);
        // fHistManager.FillTH1(histName, phiMinusPsi3);
      }
    }
    sumAcceptedTracks += count;

    histName = TString::Format("%s/hNTracks_%d", groupName.Data(), iCentBin);
    fHistManager.FillTH1(histName, count);
    
    
  }

  histName = "fHistSumNTracks";
  fHistManager.FillTH1(histName, sumAcceptedTracks);
  
}

void  AliAnalysisTaskEPCalibForJet::MeasureQnTPC(){
  TString histName;
  TString groupName;

  Double_t centrV0M=-99.0;
  Double_t centrCL1=-99.0;
  Double_t centrCL0=-99.0;

  
  AliMultSelection* fMultSelection \
    = (AliMultSelection*) InputEvent()->FindListObject("MultSelection");

  if(!fMultSelection) {
    printf("\n\n **WARNING** ::UserExec() AliMultSelection object not found.\n\n");
    exit(111);
  }
  
  
  AliVEvent *fVevent = dynamic_cast<AliVEvent*>(InputEvent());
  const AliVVertex *pointVtx = fVevent->GetPrimaryVertex();
  Double_t fVtxZ = -999.;
  fVtxZ  = pointVtx->GetZ();

  centrV0M = fMultSelection->GetMultiplicityPercentile("V0M");
  centrCL1 = fMultSelection->GetMultiplicityPercentile("CL1");
  centrCL0 = fMultSelection->GetMultiplicityPercentile("CL0");

  AliAODTracklets* aodTrkl = (AliAODTracklets*)fAOD->GetTracklets();
  Int_t nITSTrkls = aodTrkl->GetNumberOfTracklets();

  const Int_t nTracks = fAOD->GetNumberOfTracks();
  Int_t multTrk = 0;
  
  Int_t    fTPCclustMinforEP= 70;      // Fixed for EP calculation
  Int_t    trkChrg=0,trkNClus=0;     //
  Double_t fMinPtCutforEP   = 0.2;    // Fixed for EP calculation
  Double_t fMaxPtCutforEP   = 2.0;    // Fixed for EP calculation
  Double_t fEtaGapPosforEP  = 0.1;    // could be made variable in AddTask Macro.
  Double_t fEtaGapNegforEP  =-0.1;    // could be made variable in AddTask Macro.

  Double_t trkPt=0, trkPhi=0, trkEta=0, trkChi2=0, trkdEdx=0, trkWgt=1.0;  
  Double_t SumQ2xTPCPos = 0., SumQ2yTPCPos = 0., SumQ2xTPCNeg = 0., SumQ2yTPCNeg = 0;
  Double_t SumQ3xTPCPos = 0., SumQ3yTPCPos = 0., SumQ3xTPCNeg = 0., SumQ3yTPCNeg = 0;
  
  Double_t fWgtMultTPCPos=0., fWgtMultTPCNeg=0;
  
  for (Int_t it = 0; it < nTracks; it++) {
    AliAODTrack* aodTrk = (AliAODTrack*)fAOD->GetTrack(it);
    if (!aodTrk) continue;
    
    if (aodTrk->TestFilterBit(32)){
      if((TMath::Abs(aodTrk->Eta()) < 0.8)&&(aodTrk->GetTPCNcls() >= 70)&&(aodTrk->Pt() >= 0.2))
        multTrk++;
    }
    
    /// Now get TPC Q-Vectors:
    // if(!aodTrk->TestFilterBit(fFilterBit))    continue;  //// Only use FB tracks. 
    trkPt    = aodTrk->Pt();
    trkPhi   = aodTrk->Phi();
    trkEta   = aodTrk->Eta();
    trkChrg  = aodTrk->Charge();
    trkChi2  = aodTrk->Chi2perNDF();
    trkNClus = aodTrk->GetTPCNcls();
    // trkdEdx  = aodTrk->GetDetPid()->GetTPCsignal(); //?????????????????????????
    
    
    //Apply track cuts for EP  here:
    if(trkNClus< fTPCclustMinforEP)        continue;
    if((trkEta < -0.8) || (trkEta > 0.8)) continue;    
    if(trkPt < fMinPtCutforEP) continue;
    if(trkPt > fMaxPtCutforEP) continue;
    if(trkChi2 < 0.1)          continue;
    if(trkChi2 > 4.0)          continue;
    // if(trkdEdx < 10)           continue;
    if(!TMath::Abs(trkChrg))   continue;
    
    groupName="GainCalib";
    if(trkChrg > 0){ ///+ve Ch done
      histName = TString::Format("%s/hTPCPosiTrkVzPhiEta_%d", groupName.Data(), fRunNumber);
      fHistManager.FillTH3(histName, fVtxZ,trkPhi,trkEta);
    }else{  //-Ve charge
      histName = TString::Format("%s/hTPCNegaTrkVzPhiEta_%d", groupName.Data(), fRunNumber);
      fHistManager.FillTH3(histName, fVtxZ,trkPhi,trkEta);
    }
    

    Int_t trkID = aodTrk->GetID();
    // trkWgt = GetNUAWeightForTrack(fVertexZEvent,trkPhi,trkEta,trkChrg); //???????
    
    ///Used Pt as weight for Better resolution:
    if(trkEta >= fEtaGapPosforEP){
      SumQ2xTPCPos += trkWgt*TMath::Cos(2*trkPhi);
      SumQ2yTPCPos += trkWgt*TMath::Sin(2*trkPhi);
      SumQ3xTPCPos += trkWgt*TMath::Cos(3*trkPhi);
      SumQ3yTPCPos += trkWgt*TMath::Sin(3*trkPhi);
      
      fWgtMultTPCPos += trkWgt; 
      // vecPosEPTrkID.push_back(trkID);
      // vecPosEPTrkPhi.push_back(trkPhi);
      // vecPosEPTrkNUAWgt.push_back(trkWgt);
    }
    else if(trkEta <= fEtaGapNegforEP){
      SumQ2xTPCNeg += trkWgt*TMath::Cos(2*trkPhi);
      SumQ2yTPCNeg += trkWgt*TMath::Sin(2*trkPhi);
      SumQ3xTPCNeg += trkWgt*TMath::Cos(3*trkPhi);
      SumQ3yTPCNeg += trkWgt*TMath::Sin(3*trkPhi);
      
      fWgtMultTPCNeg += trkWgt;
      // vecNegEPTrkID.push_back(trkID);
      // vecNegEPTrkPhi.push_back(trkPhi);
      // vecNegEPTrkNUAWgt.push_back(trkWgt);
    }
    
  }//AOD track loop
  
    /// Set The q vector values for Event Plane: 
  if(fWgtMultTPCPos<0.1 || fWgtMultTPCNeg<0.1){        /// this means there is not enough tracks in this event!!
    q2VecTpcP[0] = 0;
    q2VecTpcP[1] = 0;
    q2VecTpcN[0] = 0;
    q2VecTpcN[1] = 0;

    q3VecTpcP[0] = 0;
    q3VecTpcP[1] = 0;
    q3VecTpcN[0] = 0;
    q3VecTpcN[1] = 0;
  }
  else{
    q2VecTpcM[0] = (SumQ2xTPCPos+SumQ2xTPCNeg)/(fWgtMultTPCPos+fWgtMultTPCNeg);
    q2VecTpcM[1] = (SumQ2yTPCPos+SumQ2xTPCNeg)/(fWgtMultTPCPos+fWgtMultTPCNeg);
    q2VecTpcP[0] = SumQ2xTPCPos/fWgtMultTPCPos;
    q2VecTpcP[1] = SumQ2yTPCPos/fWgtMultTPCPos;
    q2VecTpcN[0] = SumQ2xTPCNeg/fWgtMultTPCNeg;
    q2VecTpcN[1] = SumQ2yTPCNeg/fWgtMultTPCNeg;

    q3VecTpcM[0] = (SumQ3xTPCPos+SumQ3xTPCNeg)/(fWgtMultTPCPos+fWgtMultTPCNeg);
    q3VecTpcM[1] = (SumQ3yTPCPos+SumQ3xTPCNeg)/(fWgtMultTPCPos+fWgtMultTPCNeg);
    q3VecTpcP[0] = SumQ3xTPCPos/fWgtMultTPCPos;
    q3VecTpcP[1] = SumQ3yTPCPos/fWgtMultTPCPos;
    q3VecTpcN[0] = SumQ3xTPCNeg/fWgtMultTPCNeg;
    q3VecTpcN[1] = SumQ3yTPCNeg/fWgtMultTPCNeg;
  }

  // std::cout << "SumQ2xTPCPos:SumQ2xTPCNeg = " << q2VecTpcP[0] <<" : "<< q2VecTpcP[1] << std::endl;

  if(fReCentCalibQA){
    Int_t iCentBin = GetCentBin();

    groupName="ReCentCalib";

    histName = TString::Format("%s/hAvgQ2XvsCentTPCPBef_%d", groupName.Data(), fRunNumber);
    fHistManager.FillProfile(histName, iCentBin, q2VecTpcP[0]);
    histName = TString::Format("%s/hAvgQ2YvsCentTPCPBef_%d", groupName.Data(), fRunNumber);
    fHistManager.FillProfile(histName, iCentBin, q2VecTpcP[1]);
    histName = TString::Format("%s/hAvgQ2XvsCentTPCNBef_%d", groupName.Data(), fRunNumber);
    fHistManager.FillProfile(histName, iCentBin, q2VecTpcN[0]);
    histName = TString::Format("%s/hAvgQ2YvsCentTPCNBef_%d", groupName.Data(), fRunNumber);
    fHistManager.FillProfile(histName, iCentBin, q2VecTpcN[1]);

    histName = TString::Format("%s/hAvgQ3XvsCentTPCPBef_%d", groupName.Data(), fRunNumber);
    fHistManager.FillProfile(histName, iCentBin, q3VecTpcP[0]);
    histName = TString::Format("%s/hAvgQ3YvsCentTPCPBef_%d", groupName.Data(), fRunNumber);
    fHistManager.FillProfile(histName, iCentBin, q3VecTpcP[1]);
    histName = TString::Format("%s/hAvgQ3XvsCentTPCNBef_%d", groupName.Data(), fRunNumber);
    fHistManager.FillProfile(histName, iCentBin, q3VecTpcN[0]);
    histName = TString::Format("%s/hAvgQ3YvsCentTPCNBef_%d", groupName.Data(), fRunNumber);
    fHistManager.FillProfile(histName, iCentBin, q3VecTpcN[1]);

  }
  
}


Bool_t  AliAnalysisTaskEPCalibForJet::QnV0GainCalibration(){
  //V0 Channel Gains:
  fHCorrV0ChWeghts = (TH2F *)fCalibRefObjList->FindObject(Form("hWgtV0ChannelsvsVzRun%d",fRunNumber));
  if(fHCorrV0ChWeghts){
    printf("\n ===========> Info:: V0 Channel Weights Found for Run %d \n ",fRunNumber);
  }

  AliAODVZERO* fAodV0 = dynamic_cast<AliAODVZERO*>(fAOD->GetVZEROData());
  AliVEvent *fVevent = dynamic_cast<AliVEvent*>(InputEvent());
  const AliVVertex *pointVtx = fVevent->GetPrimaryVertex();
  Double_t fVtxZ = -999;
  fVtxZ  = pointVtx->GetZ();
  
  Double_t fMultV0 = 0.;
  Int_t ibinV0 = 0;
  Double_t fSumMV0A = 0.;
  Double_t fSumMV0C = 0.;
  Double_t fSumMV0M = 0.;
  for(int i = 0; i < 2; i++){
    q2VecV0M[i] = 0.;
    q2VecV0C[i] = 0.;
    q2VecV0A[i] = 0.;
    q3VecV0M[i] = 0.;
    q3VecV0C[i] = 0.;
    q3VecV0A[i] = 0.;
  }
  for(int i = 0; i < 3; i++){
    V0Mult2[i] = 0.;
  }
  

  for(int iV0 = 0; iV0 < 64; iV0++) { //0-31 is V0C, 32-63 VOA
    fMultV0 = fAodV0->GetMultiplicity(iV0);
    Double_t fV0chGain = 1.;
    Double_t fPhiV0  = TMath::PiOver4()*(0.5 + iV0 % 8);

    // == s == Bef Calib Qn ==========================
    befGainCalibQ2VecV0M[0] += TMath::Cos(2*fPhiV0) * fMultV0;
    befGainCalibQ2VecV0M[1] += TMath::Sin(2*fPhiV0) * fMultV0;
    befGainCalibQ3VecV0M[0] += TMath::Cos(3*fPhiV0) * fMultV0;
    befGainCalibQ3VecV0M[1] += TMath::Sin(3*fPhiV0) * fMultV0;

    if(iV0 < 32){
      befGainCalibQ2VecV0C[0] += TMath::Cos(2*fPhiV0) * fMultV0;
      befGainCalibQ2VecV0C[1] += TMath::Sin(2*fPhiV0) * fMultV0;
      befGainCalibQ3VecV0C[0] += TMath::Cos(3*fPhiV0) * fMultV0;
      befGainCalibQ3VecV0C[1] += TMath::Sin(3*fPhiV0) * fMultV0;
    }
    else if(iV0 >= 32){
      befGainCalibQ2VecV0A[0] += TMath::Cos(2*fPhiV0) * fMultV0;
      befGainCalibQ2VecV0A[1] += TMath::Sin(2*fPhiV0) * fMultV0;
      befGainCalibQ3VecV0A[0] += TMath::Cos(3*fPhiV0) * fMultV0;
      befGainCalibQ3VecV0A[1] += TMath::Sin(3*fPhiV0) * fMultV0;
    }
    // == e == Bef Calib Qn ==========================


    /// V0 Channel Gain Correction:
    if(fHCorrV0ChWeghts){
        ibinV0    = fHCorrV0ChWeghts->FindBin(fVtxZ,iV0);
        fV0chGain = fHCorrV0ChWeghts->GetBinContent(ibinV0);
    }
    
    fMultV0 = fMultV0*fV0chGain;
    // std::cout << "(fV0chGain, fMultV0) = (" << fV0chGain << ","  << fMultV0 << ")" << std::endl;
    
    q2VecV0M[0] += TMath::Cos(2*fPhiV0) * fMultV0;
    q2VecV0M[1] += TMath::Sin(2*fPhiV0) * fMultV0;
    q3VecV0M[0] += TMath::Cos(3*fPhiV0) * fMultV0;
    q3VecV0M[1] += TMath::Sin(3*fPhiV0) * fMultV0;
    V0Mult2[0] += fMultV0;

    if(iV0 < 32){
      q2VecV0C[0] += TMath::Cos(2*fPhiV0) * fMultV0;
      q2VecV0C[1] += TMath::Sin(2*fPhiV0) * fMultV0;
      q3VecV0C[0] += TMath::Cos(3*fPhiV0) * fMultV0;
      q3VecV0C[1] += TMath::Sin(3*fPhiV0) * fMultV0;
      V0Mult2[1] += fMultV0;
    }
    else if(iV0 >= 32){
      q2VecV0A[0] += TMath::Cos(2*fPhiV0) * fMultV0;
      q2VecV0A[1] += TMath::Sin(2*fPhiV0) * fMultV0;
      q3VecV0A[0] += TMath::Cos(3*fPhiV0) * fMultV0;
      q3VecV0A[1] += TMath::Sin(3*fPhiV0) * fMultV0;
      V0Mult2[2] += fMultV0;
    }

    
  }///V0 Channel loop
  
  if(1){
    std::cout << "(Mult V0M, V0C, V0A) = (" << V0Mult2[0] << ", " << V0Mult2[1] << ", " << V0Mult2[2]<< ")" << std::endl;
    std::cout << "Aft Gain Calib Qx (V0C, V0A) = ("  << q2VecV0C[0] << ", " << q2VecV0A[0] << ")"\
      << ", Qy (V0C, V0A) = ("  << q2VecV0C[1] << ", " << q2VecV0A[1] << ")"<< std::endl;
  }

  /// Now the q vectors:
  if(V0Mult2[1]<=1e-4 || V0Mult2[2]<=1e-4){
    befGainCalibQ2VecV0M[0] = 0.;
    befGainCalibQ2VecV0M[1] = 0.;
    befGainCalibQ3VecV0M[0] = 0.;
    befGainCalibQ3VecV0M[1] = 0.;
    befGainCalibQ2VecV0C[0] = 0.;
    befGainCalibQ2VecV0C[1] = 0.;
    befGainCalibQ3VecV0C[0] = 0.;
    befGainCalibQ3VecV0C[1] = 0.;
    befGainCalibQ2VecV0A[0] = 0.;
    befGainCalibQ2VecV0A[1] = 0.;
    befGainCalibQ3VecV0A[0] = 0.;
    befGainCalibQ3VecV0A[1] = 0.;
    
    q2VecV0M[0] = 0.;
    q2VecV0M[1] = 0.;
    q3VecV0M[0] = 0.;
    q3VecV0M[1] = 0.;
    q2VecV0C[0] = 0.;
    q2VecV0C[1] = 0.;
    q3VecV0C[0] = 0.;
    q3VecV0C[1] = 0.;
    q2VecV0A[0] = 0.;
    q2VecV0A[1] = 0.;
    q3VecV0A[0] = 0.;
    q3VecV0A[1] = 0.;
    
    return kFALSE;       
  }
  else{
    TString histName;
    TString groupName;
    groupName="GainCalib";

    histName = TString::Format("%s/CentGainCalibImpQ2x_V0M", groupName.Data());
    fHistManager.FillTH2(histName, fCent, (q2VecV0M[0]-befGainCalibQ2VecV0M[0])/V0Mult2[0]);
    histName = TString::Format("%s/CentGainCalibImpQ2y_V0M", groupName.Data());
    fHistManager.FillTH2(histName, fCent, (q2VecV0M[1]-befGainCalibQ2VecV0M[1])/V0Mult2[0]);
    histName = TString::Format("%s/CentGainCalibImpQ2x_V0C", groupName.Data());
    fHistManager.FillTH2(histName, fCent, (q2VecV0C[0]-befGainCalibQ2VecV0C[0])/V0Mult2[1]);
    histName = TString::Format("%s/CentGainCalibImpQ2y_V0C", groupName.Data());
    fHistManager.FillTH2(histName, fCent, (q2VecV0C[1]-befGainCalibQ2VecV0C[1])/V0Mult2[1]);
    histName = TString::Format("%s/CentGainCalibImpQ2x_V0A", groupName.Data());
    fHistManager.FillTH2(histName, fCent, (q2VecV0A[0]-befGainCalibQ2VecV0A[0])/V0Mult2[2]);
    histName = TString::Format("%s/CentGainCalibImpQ2y_V0A", groupName.Data());
    fHistManager.FillTH2(histName, fCent, (q2VecV0A[1]-befGainCalibQ2VecV0A[1])/V0Mult2[2]);


    histName = TString::Format("%s/CentGainCalibImpQ3x_V0M", groupName.Data());
    fHistManager.FillTH2(histName, fCent, (q3VecV0M[0]-befGainCalibQ3VecV0M[0])/V0Mult2[0]);
    histName = TString::Format("%s/CentGainCalibImpQ3y_V0M", groupName.Data());
    fHistManager.FillTH2(histName, fCent, (q3VecV0M[1]-befGainCalibQ3VecV0M[1])/V0Mult2[0]);
    histName = TString::Format("%s/CentGainCalibImpQ3x_V0C", groupName.Data());
    fHistManager.FillTH2(histName, fCent, (q3VecV0C[0]-befGainCalibQ3VecV0C[0])/V0Mult2[1]);
    histName = TString::Format("%s/CentGainCalibImpQ3y_V0C", groupName.Data());
    fHistManager.FillTH2(histName, fCent, (q3VecV0C[1]-befGainCalibQ3VecV0C[1])/V0Mult2[1]);
    histName = TString::Format("%s/CentGainCalibImpQ3x_V0A", groupName.Data());
    fHistManager.FillTH2(histName, fCent, (q3VecV0A[0]-befGainCalibQ3VecV0A[0])/V0Mult2[2]);
    histName = TString::Format("%s/CentGainCalibImpQ3y_V0A", groupName.Data());
    fHistManager.FillTH2(histName, fCent, (q3VecV0A[1]-befGainCalibQ3VecV0A[1])/V0Mult2[2]);

    return kTRUE;  
  }

}


Bool_t  AliAnalysisTaskEPCalibForJet::QnRecenteringCalibration(){
  // if(V0Mult2[0] != 0.){
  //   q2VecV0M[0] /= V0Mult2[0];
  //   q2VecV0M[1] /= V0Mult2[0];
  //   q3VecV0M[0] /= V0Mult2[0];
  //   q3VecV0M[1] /= V0Mult2[0];
  // }

  // if(V0Mult2[1] != 0.){
  //   q2VecV0C[0] /= V0Mult2[1];
  //   q2VecV0C[1] /= V0Mult2[1];
  //   q3VecV0C[0] /= V0Mult2[1];
  //   q3VecV0C[1] /= V0Mult2[1];
  // }

  // if(V0Mult2[2] != 0.){
  //   q2VecV0A[0] /= V0Mult2[2];
  //   q2VecV0A[1] /= V0Mult2[2];
  //   q3VecV0A[0] /= V0Mult2[2];
  //   q3VecV0A[1] /= V0Mult2[2];
  // }




  if(fReCentCalibQA){
    TString histName;
    TString groupName;
    groupName="ReCentCalib";

    histName = TString::Format("%s/hAvgQ2XvsCentV0MBef_%d", groupName.Data(), fRunNumber);
    fHistManager.FillProfile(histName, fCent, q2VecV0M[0]);
    histName = TString::Format("%s/hAvgQ2YvsCentV0MBef_%d", groupName.Data(), fRunNumber);
    fHistManager.FillProfile(histName, fCent, q2VecV0M[1]);
    histName = TString::Format("%s/hAvgQ2XvsCentV0CBef_%d", groupName.Data(), fRunNumber);
    fHistManager.FillProfile(histName, fCent, q2VecV0C[0]);
    histName = TString::Format("%s/hAvgQ2YvsCentV0CBef_%d", groupName.Data(), fRunNumber);
    fHistManager.FillProfile(histName, fCent, q2VecV0C[1]);
    histName = TString::Format("%s/hAvgQ2XvsCentV0ABef_%d", groupName.Data(), fRunNumber);
    fHistManager.FillProfile(histName, fCent, q2VecV0A[0]);
    histName = TString::Format("%s/hAvgQ2YvsCentV0ABef_%d", groupName.Data(), fRunNumber);
    fHistManager.FillProfile(histName, fCent, q2VecV0A[1]);

    histName = TString::Format("%s/hAvgQ3XvsCentV0MBef_%d", groupName.Data(), fRunNumber);
    fHistManager.FillProfile(histName, fCent, q3VecV0M[0]);
    histName = TString::Format("%s/hAvgQ3YvsCentV0MBef_%d", groupName.Data(), fRunNumber);
    fHistManager.FillProfile(histName, fCent, q3VecV0M[1]);
    histName = TString::Format("%s/hAvgQ3XvsCentV0CBef_%d", groupName.Data(), fRunNumber);
    fHistManager.FillProfile(histName, fCent, q3VecV0C[0]);
    histName = TString::Format("%s/hAvgQ3YvsCentV0CBef_%d", groupName.Data(), fRunNumber);
    fHistManager.FillProfile(histName, fCent, q3VecV0C[1]);
    histName = TString::Format("%s/hAvgQ3XvsCentV0ABef_%d", groupName.Data(), fRunNumber);
    fHistManager.FillProfile(histName, fCent, q3VecV0A[0]);
    histName = TString::Format("%s/hAvgQ3YvsCentV0ABef_%d", groupName.Data(), fRunNumber);
    fHistManager.FillProfile(histName, fCent, q3VecV0A[1]);

    histName = TString::Format("%s/hCentVsQxV0MBefCalib", groupName.Data());
    fHistManager.FillTH2(histName, fCent, q2VecV0M[0]);
    histName = TString::Format("%s/hCentVsQyV0MBefCalib", groupName.Data());
    fHistManager.FillTH2(histName, fCent, q2VecV0M[1]);
    histName = TString::Format("%s/hCentVsQxV0CBefCalib", groupName.Data());
    fHistManager.FillTH2(histName, fCent, q2VecV0C[0]);
    histName = TString::Format("%s/hCentVsQyV0CBefCalib", groupName.Data());
    fHistManager.FillTH2(histName, fCent, q2VecV0C[1]);
    histName = TString::Format("%s/hCentVsQxV0ABefCalib", groupName.Data());
    fHistManager.FillTH2(histName, fCent, q2VecV0A[0]);
    histName = TString::Format("%s/hCentVsQyV0ABefCalib", groupName.Data());
    fHistManager.FillTH2(histName, fCent, q2VecV0A[1]);
  }

  if(fDoReCentCalib){
    //Get V0A, V0C <Q> Vectors:
    fHCorrQ2xV0M = (TH1D *)fCalibRefObjList->FindObject(Form("fHisAvgQ2xvsCentV0MRun%d",fRunNumber));
    fHCorrQ2yV0M = (TH1D *)fCalibRefObjList->FindObject(Form("fHisAvgQ2yvsCentV0MRun%d",fRunNumber));  
    fHCorrQ2xV0C = (TH1D *)fCalibRefObjList->FindObject(Form("fHisAvgQ2xvsCentV0CRun%d",fRunNumber));
    fHCorrQ2yV0C = (TH1D *)fCalibRefObjList->FindObject(Form("fHisAvgQ2yvsCentV0CRun%d",fRunNumber));    
    fHCorrQ2xV0A = (TH1D *)fCalibRefObjList->FindObject(Form("fHisAvgQ2xvsCentV0ARun%d",fRunNumber));
    fHCorrQ2yV0A = (TH1D *)fCalibRefObjList->FindObject(Form("fHisAvgQ2yvsCentV0ARun%d",fRunNumber));
    
    fHCorrQ3xV0M = (TH1D *)fCalibRefObjList->FindObject(Form("fHisAvgQ3xvsCentV0MRun%d",fRunNumber));
    fHCorrQ3yV0M = (TH1D *)fCalibRefObjList->FindObject(Form("fHisAvgQ3yvsCentV0MRun%d",fRunNumber));  
    fHCorrQ3xV0C = (TH1D *)fCalibRefObjList->FindObject(Form("fHisAvgQ3xvsCentV0CRun%d",fRunNumber));
    fHCorrQ3yV0C = (TH1D *)fCalibRefObjList->FindObject(Form("fHisAvgQ3yvsCentV0CRun%d",fRunNumber));    
    fHCorrQ3xV0A = (TH1D *)fCalibRefObjList->FindObject(Form("fHisAvgQ3xvsCentV0ARun%d",fRunNumber));
    fHCorrQ3yV0A = (TH1D *)fCalibRefObjList->FindObject(Form("fHisAvgQ3yvsCentV0ARun%d",fRunNumber));    

    Int_t icentbin = 0;
    Double_t avgqx=0,avgqy=0; 

    // std::cout<<"before V0C q2x:q2y = "<< q2VecV0C[0] <<" : "<< q2VecV0C[1]<<std::endl;
    // std::cout<<"before V0A q2x:q2y = "<< q2VecV0A[0] <<" : "<< q2VecV0A[1]<<std::endl;
    if(fHCorrQ2xV0M && fHCorrQ2yV0M){
      icentbin = fHCorrQ2xV0M->FindBin(fCent);
      avgqx = fHCorrQ2xV0M->GetBinContent(icentbin);
      avgqy = fHCorrQ2yV0M->GetBinContent(icentbin);
      q2VecV0M[0] -= avgqx;
      q2VecV0M[1] -= avgqy;
    }
    if(fHCorrQ2xV0C && fHCorrQ2yV0C){
      icentbin = fHCorrQ2xV0C->FindBin(fCent);
      avgqx = fHCorrQ2xV0C->GetBinContent(icentbin);
      avgqy = fHCorrQ2yV0C->GetBinContent(icentbin);
      q2VecV0C[0] -= avgqx;
      q2VecV0C[1] -= avgqy;
    }
    if(fHCorrQ2xV0A && fHCorrQ2yV0A){
      icentbin = fHCorrQ2xV0A->FindBin(fCent);
      avgqx = fHCorrQ2xV0A->GetBinContent(icentbin);
      avgqy = fHCorrQ2yV0A->GetBinContent(icentbin);
      q2VecV0A[0] -= avgqx;
      q2VecV0A[1] -= avgqy;
    }

    if(fHCorrQ3xV0M && fHCorrQ3yV0M){
      icentbin = fHCorrQ3xV0M->FindBin(fCent);
      avgqx = fHCorrQ3xV0M->GetBinContent(icentbin);
      avgqy = fHCorrQ3yV0M->GetBinContent(icentbin);
      q3VecV0M[0] -= avgqx;
      q3VecV0M[1] -= avgqy;
    }
    if(fHCorrQ3xV0C && fHCorrQ3yV0C){
      icentbin = fHCorrQ3xV0C->FindBin(fCent);
      avgqx = fHCorrQ3xV0C->GetBinContent(icentbin);
      avgqy = fHCorrQ3yV0C->GetBinContent(icentbin);
      q3VecV0C[0] -= avgqx;
      q3VecV0C[1] -= avgqy;
    }
    if(fHCorrQ3xV0A && fHCorrQ3yV0A){
      icentbin = fHCorrQ3xV0A->FindBin(fCent);
      avgqx = fHCorrQ3xV0A->GetBinContent(icentbin);
      avgqy = fHCorrQ3yV0A->GetBinContent(icentbin);
      q3VecV0A[0] -= avgqx;
      q3VecV0A[1] -= avgqy;           
    }
    
    if(fReCentCalibQA){
      TString histName;
      TString groupName;
      groupName="ReCentCalib";

      histName = TString::Format("%s/hAvgQ2XvsCentV0MAft_%d", groupName.Data(), fRunNumber);
      fHistManager.FillProfile(histName, fCent, q2VecV0M[0]);
      histName = TString::Format("%s/hAvgQ2YvsCentV0MAft_%d", groupName.Data(), fRunNumber);
      fHistManager.FillProfile(histName, fCent, q2VecV0M[1]);
      histName = TString::Format("%s/hAvgQ2XvsCentV0CAft_%d", groupName.Data(), fRunNumber);
      fHistManager.FillProfile(histName, fCent, q2VecV0C[0]);
      histName = TString::Format("%s/hAvgQ2YvsCentV0CAft_%d", groupName.Data(), fRunNumber);
      fHistManager.FillProfile(histName, fCent, q2VecV0C[1]);
      histName = TString::Format("%s/hAvgQ2XvsCentV0AAft_%d", groupName.Data(), fRunNumber);
      fHistManager.FillProfile(histName, fCent, q2VecV0A[0]);
      histName = TString::Format("%s/hAvgQ2YvsCentV0AAft_%d", groupName.Data(), fRunNumber);
      fHistManager.FillProfile(histName, fCent, q2VecV0A[1]);

      histName = TString::Format("%s/hAvgQ3XvsCentV0MAft_%d", groupName.Data(), fRunNumber);
      fHistManager.FillProfile(histName, fCent, q3VecV0M[0]);
      histName = TString::Format("%s/hAvgQ3YvsCentV0MAft_%d", groupName.Data(), fRunNumber);
      fHistManager.FillProfile(histName, fCent, q3VecV0M[1]);
      histName = TString::Format("%s/hAvgQ3XvsCentV0CAft_%d", groupName.Data(), fRunNumber);
      fHistManager.FillProfile(histName, fCent, q3VecV0C[0]);
      histName = TString::Format("%s/hAvgQ3YvsCentV0CAft_%d", groupName.Data(), fRunNumber);
      fHistManager.FillProfile(histName, fCent, q3VecV0C[1]);
      histName = TString::Format("%s/hAvgQ3XvsCentV0AAft_%d", groupName.Data(), fRunNumber);
      fHistManager.FillProfile(histName, fCent, q3VecV0A[0]);
      histName = TString::Format("%s/hAvgQ3YvsCentV0AAft_%d", groupName.Data(), fRunNumber);
      fHistManager.FillProfile(histName, fCent, q3VecV0A[1]);

      histName = TString::Format("%s/hCentVsQxV0MAftCalib", groupName.Data());
      fHistManager.FillTH2(histName, fCent, q2VecV0M[0]);
      histName = TString::Format("%s/hCentVsQyV0MAftCalib", groupName.Data());
      fHistManager.FillTH2(histName, fCent, q2VecV0M[1]);
      histName = TString::Format("%s/hCentVsQxV0CAftCalib", groupName.Data());
      fHistManager.FillTH2(histName, fCent, q2VecV0C[0]);
      histName = TString::Format("%s/hCentVsQyV0CAftCalib", groupName.Data());
      fHistManager.FillTH2(histName, fCent, q2VecV0C[1]);
      histName = TString::Format("%s/hCentVsQxV0AAftCalib", groupName.Data());
      fHistManager.FillTH2(histName, fCent, q2VecV0A[0]);
      histName = TString::Format("%s/hCentVsQyV0AAftCalib", groupName.Data());
      fHistManager.FillTH2(histName, fCent, q2VecV0A[1]);
    }

  }

  if(V0Mult2[0] != 0.){
    q2VecV0M[0] /= V0Mult2[0];
    q2VecV0M[1] /= V0Mult2[0];
    q3VecV0M[0] /= V0Mult2[0];
    q3VecV0M[1] /= V0Mult2[0];
  }

  if(V0Mult2[1] != 0.){
    q2VecV0C[0] /= V0Mult2[1];
    q2VecV0C[1] /= V0Mult2[1];
    q3VecV0C[0] /= V0Mult2[1];
    q3VecV0C[1] /= V0Mult2[1];
  }

  if(V0Mult2[2] != 0.){
    q2VecV0A[0] /= V0Mult2[2];
    q2VecV0A[1] /= V0Mult2[2];
    q3VecV0A[0] /= V0Mult2[2];
    q3VecV0A[1] /= V0Mult2[2];
  }

  q2V0[0] = TMath::Sqrt(q2VecV0M[0]*q2VecV0M[0]+q2VecV0M[1]*q2VecV0M[1])*TMath::Sqrt(V0Mult2[0]);
  q2V0[1] = TMath::Sqrt(q2VecV0C[0]*q2VecV0C[0]+q2VecV0C[1]*q2VecV0C[1])*TMath::Sqrt(V0Mult2[1]);
  q2V0[2] = TMath::Sqrt(q2VecV0A[0]*q2VecV0A[0]+q2VecV0A[1]*q2VecV0A[1])*TMath::Sqrt(V0Mult2[2]);

  q3V0[0] = TMath::Sqrt(q3VecV0M[0]*q3VecV0M[0]+q3VecV0M[1]*q3VecV0M[1])*TMath::Sqrt(V0Mult2[0]);
  q3V0[1] = TMath::Sqrt(q3VecV0C[0]*q3VecV0C[0]+q3VecV0C[1]*q3VecV0C[1])*TMath::Sqrt(V0Mult2[1]);
  q3V0[2] = TMath::Sqrt(q3VecV0A[0]*q3VecV0A[0]+q3VecV0A[1]*q3VecV0A[1])*TMath::Sqrt(V0Mult2[2]);

  return kTRUE;
}


Bool_t AliAnalysisTaskEPCalibForJet::QnJEHandlarEPGet()
{
    ResetAODEvent();
    fAOD = dynamic_cast<AliAODEvent*>(InputEvent());
    SetAODEvent(fAOD); 
    if(!fIsOADBFileOpen || fCalibObjRun!=fRun) {
        fIsOADBFileOpen = OpenInfoCalbration();
        if(!fIsOADBFileOpen)
            return kFALSE;
        fCalibObjRun = fRun;
    }
    
    //== Q2 Vector ######################################## 
    Double_t harmonic = 2.;
    ComputeQvecV0(q2VecV0M, q2VecV0C, q2VecV0A, q2V0, V0Mult2, harmonic);
    ComputeQvecTpc(q2VecTpcM, q2VecTpcN, q2VecTpcP, q2Tpc, TpcMult2, harmonic);
    
    // == s == ComputeEventPlaneAngle ======================
    // Inisialize
    for(Int_t i = 0; i<3; i++){
        psi2V0[i] = -1;
        psi2Tpc[i] = -1;
    }
    
    psi2V0[0] = ComputeEventPlaneAngle(q2VecV0M, harmonic);
    psi2V0[1] = ComputeEventPlaneAngle(q2VecV0C, harmonic);
    psi2V0[2] = ComputeEventPlaneAngle(q2VecV0A, harmonic);
    
    psi2Tpc[0] = ComputeEventPlaneAngle(q2VecTpcM, harmonic);
    psi2Tpc[1] = ComputeEventPlaneAngle(q2VecTpcN, harmonic);
    psi2Tpc[2] = ComputeEventPlaneAngle(q2VecTpcP, harmonic);
    
    // std::cout << "psi2V0[0] = " << psi2V0[0] << std::endl;
    
    // Getqn(q2V0, q2NormV0, V0Mult2);
    // Getqn(q2Tpc, q2NormTpc, TpcMult2);

    // std::cout << "q2V0[0] = " << q2V0[0] << std::endl;
    // std::cout << "q2NormV0[0] = " << q2NormV0[0] << std::endl;

    //== Q3 Vector ######################################## 
    harmonic = 3.;
    ComputeQvecV0(q3VecV0M, q3VecV0C, q3VecV0A, q3V0, V0Mult3, harmonic);
    ComputeQvecTpc(q3VecTpcM, q3VecTpcN, q3VecTpcP, q3Tpc, TpcMult3, harmonic);
    
    // Inisialize
    for(Int_t i = 0; i<3; i++){
        psi3V0[i] = -1;
        psi3Tpc[i] = -1;
    }
    

    psi3V0[0] = ComputeEventPlaneAngle(q3VecV0M, harmonic);
    psi3V0[1] = ComputeEventPlaneAngle(q3VecV0C, harmonic);
    psi3V0[2] = ComputeEventPlaneAngle(q3VecV0A, harmonic);

    psi3Tpc[0] = ComputeEventPlaneAngle(q3VecTpcM, harmonic);
    psi3Tpc[1] = ComputeEventPlaneAngle(q3VecTpcN, harmonic);
    psi3Tpc[2] = ComputeEventPlaneAngle(q3VecTpcP, harmonic);

    // Getqn(q3V0, q3NormV0, V0Mult3);
    // Getqn(q3Tpc, q3NormTpc, TpcMult3);
    
    // std::cout << "V0C(qx,qy) = " << q2VecV0C[0] << "," << q2VecV0C[1] << ")" << std::endl;

    return kTRUE;
}

//_____________________________________________________________________________
TH1F* AliAnalysisTaskEPCalibForJet::GetResoFromOutputFile(detectorType det, Int_t h, TArrayD* cen)
{
    if(!fOutputList) {
        printf(" > Please add fOutputList first < \n");
        return 0x0;
    }
    TH1F* r(0x0);
    (cen) ? r = new TH1F("R", "R", cen->GetSize()-1, cen->GetArray()) : r = new TH1F("R", "R", 10, 0, 10);
    if(!cen) r->GetXaxis()->SetTitle("number of centrality bin");
    r->GetYaxis()->SetTitle(Form("Resolution #Psi_{%i}", h));
    for(Int_t i(0); i < 10; i++) {
        TProfile* temp((TProfile*)fOutputList->FindObject(Form("fProfV%iResolution_%i", h, i)));
        if(!temp) break;
        Double_t a(temp->GetBinContent(3)); //cos(2[psi_V0A - psi_V0C])
        Double_t b(temp->GetBinContent(5)); //cos(2[psi_TPC - psi_V0C])
        Double_t c(temp->GetBinContent(7)); //cos(2[psi_TPC - psi_V0A])

        Double_t d(temp->GetBinContent(9));  //cos(2[psi_V0M - psi_TPCnega])
        Double_t e(temp->GetBinContent(10)); //cos(2[psi_V0M - psi_TPCposi])
        Double_t f(temp->GetBinContent(11)); //cos(2[psi_TPCnega - psi_TPCposi])

        Double_t _a(temp->GetBinError(3)), _b(temp->GetBinError(5)), _c(temp->GetBinError(7));
        Double_t _d(temp->GetBinError(9)), _e(temp->GetBinError(10)), _f(temp->GetBinError(11));
        Double_t error(0);
        if(a <= 0 || b <= 0 || c <= 0 || d <= 0 || e <= 0 || f <= 0) continue;
        switch (det) {
            case kVZEROA : {
                r->SetBinContent(1+i, TMath::Sqrt((a*b)/c));
                if(i==0) r->SetNameTitle("VZEROA resolution", "VZEROA resolution");
                error = TMath::Power((2.*a*TMath::Sqrt((a*b)/c))/3.,2.)*_a*_a+TMath::Power((2.*b*TMath::Sqrt((a*b)/c))/3.,2.)*_b*_b+TMath::Power(2.*c*TMath::Sqrt((a*b)/c),2.)*_c*_c;
                if(error > 0.) error = TMath::Sqrt(error);
                r->SetBinError(1+i, error);
            } break;
            case kVZEROC : {
                r->SetBinContent(1+i, TMath::Sqrt((a*c)/b));
                error = TMath::Power((2.*a*TMath::Sqrt((a*c)/b))/3.,2.)*_a*_a+TMath::Power((2.*b*TMath::Sqrt((a*c)/b)),2.)*_b*_b+TMath::Power(2.*c*TMath::Sqrt((a*c)/b)/3.,2.)*_c*_c;
                if(error > 0.) error = TMath::Sqrt(error);
                if(i==0) r->SetNameTitle("VZEROC resolution", "VZEROC resolution");
                r->SetBinError(1+i, error);
            } break;
            case kTPC : {
                r->SetBinContent(1+i, TMath::Sqrt((b*c)/a));
                if(i==0) r->SetNameTitle("TPC resolution", "TPC resolution");
                r->SetBinError(1+i, TMath::Sqrt(_a*_a+_b*_b+_c*_c));
            } break;
            case kVZEROComb : {
                r->SetBinContent(1+i, TMath::Sqrt((d*e)/f));
                if(i==0) r->SetNameTitle("VZEROComb resolution", "VZEROComb resolution");
                r->SetBinError(1+i, TMath::Sqrt(_d*_d+_e*_e+_f*_f));
            } break;
            default : break;
        }
    }
    return r;
}

//_____________________________________________________________________________
Double_t AliAnalysisTaskEPCalibForJet::CalculateEventPlaneChi(Double_t res)
{
    // return chi for given resolution to combine event plane estimates from two subevents
    // see Phys. Rev. C no. CS6346 (http://arxiv.org/abs/nucl-ex/9805001)
    Double_t chi(2.), delta(1.), con((TMath::Sqrt(TMath::Pi()))/(2.*TMath::Sqrt(2)));
    for (Int_t i(0); i < 15; i++) {
        chi = ((con*chi*TMath::Exp(-chi*chi/4.)*(TMath::BesselI0(chi*chi/4.)+TMath::BesselI1(chi*chi/4.))) < res) ? chi + delta : chi - delta;
        delta = delta / 2.;
    }
    return chi;
}

void AliAnalysisTaskEPCalibForJet::SetupPileUpRemovalFunctions(){
  
  ////==========> LHC18q/r PileUp Removal Functions: ---- Do not Remove them !!! -----
  Double_t parV0[8] = {43.8011, 0.822574, 8.49794e-02, 1.34217e+02, 7.09023e+00, 4.99720e-02, -4.99051e-04, 1.55864e-06};
  fV0CutPU  = new TF1("fV0CutPU", "[0]+[1]*x - 6.*[2]*([3] + [4]*sqrt(x) + [5]*x + [6]*x*sqrt(x) + [7]*x*x)", 0, 100000);
  fV0CutPU->SetParameters(parV0);
  
  fSPDCutPU = new TF1("fSPDCutPU", "400. + 4.*x", 0, 10000);

  Double_t parFB32[8] = {2093.36, -66.425, 0.728932, -0.0027611, 1.01801e+02, -5.23083e+00, -1.03792e+00, 5.70399e-03};
  fMultCutPU = new TF1("fMultCutPU", "[0]+[1]*x+[2]*x*x+[3]*x*x*x - 6.*([4]+[5]*sqrt(x)+[6]*x+[7]*x*x)", 0, 90);
  fMultCutPU->SetParameters(parFB32);
  
  Double_t parV0CL0[6] = {0.320462, 0.961793, 1.02278, 0.0330054, -0.000719631, 6.90312e-06};
  fCenCutLowPU  = new TF1("fCenCutLowPU", "[0]+[1]*x - 6.5*([2]+[3]*x+[4]*x*x+[5]*x*x*x)",  0, 100);
  fCenCutLowPU->SetParameters(parV0CL0);
  
  fCenCutHighPU = new TF1("fCenCutHighPU", "[0]+[1]*x + 5.5*([2]+[3]*x+[4]*x*x+[5]*x*x*x)", 0, 100);
  fCenCutHighPU->SetParameters(parV0CL0);
  //--------------------------------------------------------------------------------------

}

//_____________________________________________________________________________
void AliAnalysisTaskEPCalibForJet::BkgFitEvaluation()
{
  // the quality of the fit is evaluated from 1 - the cdf of the chi square distribution
  // three methods are available, all with their drawbacks. 
  // all are stored, one is selected to do the cut
  Int_t numOfFreePara = 2; //v2, v3
  Int_t NDF = 1;
  NDF = (Int_t)fFitModulation->GetXaxis()->GetNbins() - numOfFreePara;
  if(NDF == 0 || (float)NDF <= 0.) return;
  
  Double_t ChiSqr = 999.;
  Double_t CDF = 1.;
  Double_t CDFROOT = 1.;
  ChiSqr = ChiSquare(*hBkgTracks, fFitModulation);
  // CDF = 1. - ChiSquareCDF(NDF, ChiSqr);  
  // CDFROOT = 1.-ChiSquareCDF(NDF, fFitModulation->GetChisquare());
  CDF = 1. - ChiSquarePDF(NDF, ChiSqr);  
  CDFROOT = 1.-ChiSquarePDF(NDF, fFitModulation->GetChisquare());
  // std::cout << "CDF = " << ChiSquarePDF(NDF, ChiSqr) << std::endl;

  TString histName;
  TString groupName;
  groupName="BackgroundFit";

  Int_t iCentBin = GetCentBin();
  
  // == v2 and v3 combind fit ==
  histName = TString::Format("%s/hPvalueCDF_lRhoCombinFit", groupName.Data());
  fHistManager.FillTH1(histName, CDF);
  histName = TString::Format("%s/hPvalueCDFCent_lRhoCombinFit", groupName.Data());
  fHistManager.FillTH2(histName, iCentBin, CDF);
  histName = TString::Format("%s/hChi2Cent_lRhoCombinFit", groupName.Data());
  fHistManager.FillTH2(histName, iCentBin, ChiSqr/((float)NDF));
  histName = TString::Format("%s/hPChi2_lRhoCombinFit", groupName.Data());
  fHistManager.FillTH2(histName, CDF, ChiSqr/((float)NDF));

  histName = TString::Format("%s/hPvalueCDFROOT_lRhoCombinFit", groupName.Data());
  fHistManager.FillTH1(histName, CDFROOT);
  histName = TString::Format("%s/hPvalueCDFROOTCent_lRhoCombinFit", groupName.Data());
  fHistManager.FillTH2(histName, iCentBin, CDFROOT);
  histName = TString::Format("%s/hChi2ROOTCent_lRhoCombinFit", groupName.Data());
  fHistManager.FillTH2(histName, iCentBin, ChiSqr/((float)NDF));
  histName = TString::Format("%s/hPChi2ROOT_lRhoCombinFit", groupName.Data());
  fHistManager.FillTH2(histName, CDFROOT, ChiSqr/((float)NDF));

  // std::cout <<"Comb Fit (ChiSqr, CDF) = ("<< ChiSqr/((float)NDF) << ", "<< CDF <<")"<< std::endl; 

  // == v2 fit ==
  TF1* tempV2Fit = new TF1("tempRhoFitV2", "[0]*(1.+2.*([1]*TMath::Cos(2.*(x-[2]))))", \
    0.0, TMath::TwoPi());
  tempV2Fit->SetParameter(0, fRho->GetVal());
  tempV2Fit->FixParameter(2, psi2V0[0]);
  hBkgTracks->Fit(tempV2Fit, "N0Q");
  numOfFreePara = 1; //v2
  NDF = tempV2Fit->GetXaxis()->GetNbins() - numOfFreePara;
  if(NDF == 0 || (float)NDF <= 0.) return;
  ChiSqr = 999.;
  ChiSqr = ChiSquare(*hBkgTracks, tempV2Fit);
  // CDF = 1. - ChiSquareCDF(NDF, ChiSqr);
  CDF = 1. - ChiSquarePDF(NDF, ChiSqr);
  // CDFROOT = 1. - ChiSquareCDF(NDF, tempV2Fit->GetChisquare());
  CDFROOT = 1. - ChiSquarePDF(NDF, tempV2Fit->GetChisquare());

  histName = TString::Format("%s/hPvalueCDF_lRhoV2Fit", groupName.Data());
  fHistManager.FillTH1(histName, CDF);
  histName = TString::Format("%s/hPvalueCDFCent_lRhoV2Fit", groupName.Data());
  fHistManager.FillTH2(histName, iCentBin, CDF);
  histName = TString::Format("%s/hChi2Cent_lRhoV2Fit", groupName.Data());
  fHistManager.FillTH2(histName, iCentBin, ChiSqr/((float)NDF));
  histName = TString::Format("%s/hPChi2_lRhoV2Fit", groupName.Data());
  fHistManager.FillTH2(histName, CDF, ChiSqr/((float)NDF));

  histName = TString::Format("%s/hPvalueCDFROOT_lRhoV2Fit", groupName.Data());
  fHistManager.FillTH1(histName, CDFROOT);
  histName = TString::Format("%s/hPvalueCDFROOTCent_lRhoV2Fit", groupName.Data());
  fHistManager.FillTH2(histName, iCentBin, CDFROOT);
  histName = TString::Format("%s/hChi2ROOTCent_lRhoV2Fit", groupName.Data());
  fHistManager.FillTH2(histName, iCentBin, ChiSqr/((float)NDF));
  histName = TString::Format("%s/hPChi2ROOT_lRhoV2Fit", groupName.Data());
  fHistManager.FillTH2(histName, CDFROOT, ChiSqr/((float)NDF));
  // std::cout << "v2Fit (ChiSqr, CDF) = ("<< ChiSqr/((float)NDF) << ", "<< CDF <<")"<< std::endl;

  delete tempV2Fit;


  // == global rho fit ==
  TF1* tempGlobalFit = new TF1("tempGlobalRhoFit", "[0]", 0.0, TMath::TwoPi());
  tempGlobalFit->FixParameter(0, fRho->GetVal());
  tempGlobalFit->SetParameter(0, fRho->GetVal());
  tempGlobalFit->FixParameter(2, psi2V0[0]);
  hBkgTracks->Fit(tempGlobalFit, "N0Q");
  numOfFreePara = 0; //v2
  NDF = tempGlobalFit->GetXaxis()->GetNbins() - numOfFreePara;
  if(NDF == 0 || (float)NDF <= 0.) return;
  ChiSqr = 999.;
  ChiSqr = ChiSquare(*hBkgTracks, tempGlobalFit);
  // CDF = 1. - ChiSquareCDF(NDF, ChiSqr);
  // CDFROOT = 1.-ChiSquareCDF(NDF, tempGlobalFit->GetChisquare());
  CDF = 1. - ChiSquarePDF(NDF, ChiSqr);
  CDFROOT = 1.-ChiSquarePDF(NDF, tempGlobalFit->GetChisquare());

  histName = TString::Format("%s/hPvalueCDF_gRhoFit", groupName.Data());
  fHistManager.FillTH1(histName, CDF);
  histName = TString::Format("%s/hPvalueCDFCent_gRhoFit", groupName.Data());
  fHistManager.FillTH2(histName, iCentBin, CDF);
  histName = TString::Format("%s/hChi2Cent_gRhoFit", groupName.Data());
  fHistManager.FillTH2(histName, iCentBin, ChiSqr/((float)NDF));
  histName = TString::Format("%s/hPChi2_gRhoFit", groupName.Data());
  fHistManager.FillTH2(histName, CDF, ChiSqr/((float)NDF));

  histName = TString::Format("%s/hPvalueCDFROOT_gRhoFit", groupName.Data());
  fHistManager.FillTH1(histName, CDFROOT);
  histName = TString::Format("%s/hPvalueCDFROOTCent_gRhoFit", groupName.Data());
  fHistManager.FillTH2(histName, iCentBin, CDFROOT);
  histName = TString::Format("%s/hChi2ROOTCent_gRhoFit", groupName.Data());
  fHistManager.FillTH2(histName, iCentBin, ChiSqr/((float)NDF));
  histName = TString::Format("%s/hPChi2ROOT_gRhoFit", groupName.Data());
  fHistManager.FillTH2(histName, CDFROOT, ChiSqr/((float)NDF));

  // std::cout << "NoFit (ChiSqr, CDF) = (" << ChiSqr/((float)NDF) << ", "<< CDF <<")"<< std::endl;
  delete tempGlobalFit;

}


Bool_t AliAnalysisTaskEPCalibForJet::CheckEventIsPileUp2018(){
  
  /// Todo Rihan: I can check for PileUp and get TPC event Plane in Same Function
  /// Utilizing same track loop. This method would save time..
  if (!fAOD && AODEvent() && IsStandardAOD()) {
    fAOD = dynamic_cast<AliAODEvent*> (AODEvent());
  }
  if (!fAOD) {
    AliWarning("AliAnalysisTaskJetQnVectors::Exec(): bad AOD");
    return kFALSE;
  }
  
  Bool_t BisPileup = kFALSE;

  Double_t centrV0M=-99.0;
  Double_t centrCL1=-99.0;
  Double_t centrCL0=-99.0;

  AliMultSelection* fMultSelection = (AliMultSelection*) InputEvent()->FindListObject("MultSelection");
  if(!fMultSelection) {
    printf("\n\n **WARNING** ::UserExec() AliMultSelection object not found.\n\n");
    exit(111);
  }
  
  centrV0M = fMultSelection->GetMultiplicityPercentile("V0M");
  centrCL1 = fMultSelection->GetMultiplicityPercentile("CL1");
  centrCL0 = fMultSelection->GetMultiplicityPercentile("CL0");

  Int_t nITSClsLy0 = fAOD->GetNumberOfITSClusters(0);
  Int_t nITSClsLy1 = fAOD->GetNumberOfITSClusters(1);
  Int_t nITSCls = nITSClsLy0 + nITSClsLy1;

  AliAODTracklets* aodTrkl = (AliAODTracklets*)fAOD->GetTracklets();
  Int_t nITSTrkls = aodTrkl->GetNumberOfTracklets();
  const Int_t nTracks = fAOD->GetNumberOfTracks();
  Int_t multTrk = 0;
  
  for (Int_t it = 0; it < nTracks; it++) {
    AliAODTrack* aodTrk = (AliAODTrack*)fAOD->GetTrack(it);

    if (!aodTrk){
      delete aodTrk;
      continue;
    }

    if(aodTrk->TestFilterBit(32)){
      if((TMath::Abs(aodTrk->Eta()) < 0.8)&&(aodTrk->GetTPCNcls() >= 70)&&(aodTrk->Pt() >= 0.2))
      multTrk++;
    }
  }
  
  AliAODVZERO* aodV0 = fAOD->GetVZEROData();
  Float_t multV0a = aodV0->GetMTotV0A();
  Float_t multV0c = aodV0->GetMTotV0C();
  Float_t multV0Tot = multV0a + multV0c;
  UShort_t multV0aOn = aodV0->GetTriggerChargeA();
  UShort_t multV0cOn = aodV0->GetTriggerChargeC();
  UShort_t multV0On = multV0aOn + multV0cOn;
  
  Int_t tpcClsTot = fAOD->GetNumberOfTPCClusters();
  Float_t nclsDif = Float_t(tpcClsTot) \
    - (60932.9 + 69.2897*multV0Tot - 0.000217837*multV0Tot*multV0Tot);
  
  
  if(centrCL0 < fCenCutLowPU->Eval(centrV0M)) BisPileup=kTRUE;
  if(centrCL0 > fCenCutHighPU->Eval(centrV0M)) BisPileup=kTRUE;
  if(Float_t(nITSCls) > fSPDCutPU->Eval(nITSTrkls)) BisPileup=kTRUE;
  if(multV0On < fV0CutPU->Eval(multV0Tot)) BisPileup=kTRUE;
  if(Float_t(multTrk) < fMultCutPU->Eval(centrV0M)) BisPileup=kTRUE;
  if(((AliAODHeader*)fAOD->GetHeader())->GetRefMultiplicityComb08() < 0) BisPileup=kTRUE;
  if(fAOD->IsIncompleteDAQ()) BisPileup=kTRUE;
  //if (nclsDif > 200000)//can be increased to 200000
  // BisPileup=kTRUE;

  Int_t multEsd = ((AliAODHeader*)fAOD->GetHeader())->GetNumberOfESDTracks();

  if(fPileupCutQA){
    TString histName;
    TString groupName;
    groupName="PileupCutQA";

    histName = TString::Format("%s/fHistCentCL0VsV0MBefore", groupName.Data());
    fHistManager.FillTH2(histName, centrV0M,centrCL0);
    histName = TString::Format("%s/fHistTPCVsESDTrkBefore", groupName.Data());
    fHistManager.FillTH2(histName, multTrk,multEsd);
    histName = TString::Format("%s/fHistTPConlyVsCL1Before", groupName.Data());
    fHistManager.FillTH2(histName, centrCL1,multTrk);
    histName = TString::Format("%s/fHistTPConlyVsV0MBefore", groupName.Data());
    fHistManager.FillTH2(histName, centrV0M,multTrk);

    if(!BisPileup){
      histName = TString::Format("%s/fHistCentCL0VsV0MAfter", groupName.Data());
      fHistManager.FillTH2(histName, centrV0M,centrCL0);
      histName = TString::Format("%s/fHistTPCVsESDTrkAfter", groupName.Data());
      fHistManager.FillTH2(histName, multTrk,multEsd);
      histName = TString::Format("%s/fHistTPConlyVsCL1After", groupName.Data());
      fHistManager.FillTH2(histName, centrCL1,multTrk);
      histName = TString::Format("%s/fHistTPConlyVsV0MAfter", groupName.Data());
      fHistManager.FillTH2(histName, centrV0M,multTrk);
    }
  }
  
  return BisPileup;
}


//________________________________________________________________
bool AliAnalysisTaskEPCalibForJet::SetAODEvent(AliAODEvent* event)
{
    if(!event) {
        AliWarning("Event not found!");
        return false;
    }
    fRun = event->GetRunNumber();
    const AliAODVertex* trkVtx = dynamic_cast<AliAODVertex*>(event->GetPrimaryVertex());
    if (!trkVtx || trkVtx->GetNContributors()<=0) {
        AliWarning("Primary vertex not found!");
        return false;
    }
    else fZvtx = trkVtx->GetZ();

    AliMultSelection* MultSelection = dynamic_cast<AliMultSelection*>(event->FindListObject("MultSelection"));
    if(!MultSelection) {
        AliWarning("AliMultSelection object not found!");
        return false;
    }    
    else fCentrality = MultSelection->GetMultiplicityPercentile("V0M");

    fV0 = dynamic_cast<AliAODVZERO*>(event->GetVZEROData());
    if(!fV0) {
        AliWarning("V0 info not found!");
        return false;
    }

    //only if everything ok get event
    fAOD = event;

    return true;
}

//________________________________________________________________
void AliAnalysisTaskEPCalibForJet::ResetAODEvent() 
{
    fRun            = -9999;
    fCentrality     = -9999.;
    fZvtx           = -9999.;
    fV0             = nullptr;

    fUsedTrackPosIDs.ResetAllBits();
    fUsedTrackNegIDs.ResetAllBits();
}


//________________________________________________________________
void AliAnalysisTaskEPCalibForJet::GetQnVec(double QnVecFull[2], double QnVecA[2], double QnVecC[2], Double_t QnNorm[3], Double_t Multi[3])
{
    switch(fNormMethod) {
        case 0: //kQoverQlength
            for(int iComp=0; iComp<2; iComp++) {
                QnNorm[0] > 0 ? QnVecFull[iComp] = QnVecFull[iComp] / QnNorm[0] : QnVecFull[iComp] = QnVecFull[iComp];
                QnNorm[2] > 0 ? QnVecA[iComp]  = QnVecA[iComp] / QnNorm[2] : QnVecA[iComp] = QnVecA[iComp];
                QnNorm[1] > 0 ? QnVecC[iComp]  = QnVecC[iComp] / QnNorm[1] : QnVecC[iComp] = QnVecC[iComp];
            }
        break;
        case 1: //kQoverM
            for(int iComp=0; iComp<2; iComp++) {
                Multi[0] > 0 ? QnVecFull[iComp] = QnVecFull[iComp] / Multi[0] : QnVecFull[iComp] = QnVecFull[iComp];
                Multi[2] > 0 ? QnVecA[iComp]  = QnVecA[iComp] / Multi[2] : QnVecA[iComp] = QnVecA[iComp];
                Multi[1] > 0 ? QnVecC[iComp]  = QnVecC[iComp] / Multi[1] : QnVecC[iComp] = QnVecC[iComp];
            }
        break;
        case 2: //kQoverSqrtM
            for(int iComp=0; iComp<2; iComp++) {
                Multi[0] > 0 ? QnVecFull[iComp] = QnVecFull[iComp] / TMath::Sqrt(Multi[0]) : QnVecFull[iComp] = QnVecFull[iComp];
                Multi[2] > 0 ? QnVecA[iComp]  = QnVecA[iComp] / TMath::Sqrt(Multi[2]) : QnVecA[iComp] = QnVecA[iComp];
                Multi[1] > 0 ? QnVecC[iComp]  = QnVecC[iComp] / TMath::Sqrt(Multi[1]) : QnVecC[iComp] = QnVecC[iComp];
            }
        break;
        case 3: //kNone
            for(int iComp=0; iComp<2; iComp++) {
                QnVecFull[iComp] = QnVecFull[iComp];
                QnVecA[iComp]  = QnVecA[iComp];
                QnVecC[iComp]  = QnVecC[iComp];
            }
        break;
    }
}


//________________________________________________________________
void AliAnalysisTaskEPCalibForJet::Getqn(Double_t Qn[3], Double_t QnNorm[3], Double_t Multi[3])
{
    Multi[0] > 0 ? Qn[0] = QnNorm[0] / TMath::Sqrt(Multi[0]) : Qn[0] = 0;
    Multi[1] > 0 ? Qn[1] = QnNorm[1] / TMath::Sqrt(Multi[1]) : Qn[1] = 0;
    Multi[2] > 0 ? Qn[2] = QnNorm[2] / TMath::Sqrt(Multi[2]) : Qn[2] = 0;
}


//________________________________________________________________
void AliAnalysisTaskEPCalibForJet::EnablePhiDistrHistos() 
{
    fEnablePhiDistrHistos=true;
    TString histonames[2] = {"TPCPosEta","TPCNegEta"};
    for(int iHisto=0; iHisto<2; iHisto++) {
        if(fPhiVsCentrTPC[iHisto]) {
            delete fPhiVsCentrTPC[iHisto];
            fPhiVsCentrTPC[iHisto]=nullptr;
        }
        fPhiVsCentrTPC[iHisto] = new TH2F(Form("fPhiVsCentrTPC%s",histonames[iHisto].Data()),";centrality (%);#varphi;Entries",10,0.,100.,180,0.,2*TMath::Pi());
    }
}


//__________________________________________________________
void AliAnalysisTaskEPCalibForJet::ComputeQvecTpc(Double_t QnVecTpcM[2],Double_t QnVecTpcN[2],Double_t QnVecTpcP[2], Double_t QnNorm[3], Double_t Multi[3], unsigned int harmonic) 
{
    short centbin = GetCentBin();

    //initialise Q vectors
    for(int iComp=0; iComp<2; iComp++) {
        QnVecTpcM[iComp] = 0.;
        QnVecTpcP[iComp]  = 0.;
        QnVecTpcN[iComp]  = 0.;
    }
    for(int i=0; i<3; i++) {
        QnNorm[3] = 1.;
        Multi[3] = 0.;
    }

    fUsedTrackPosIDs.ResetAllBits();
    fUsedTrackNegIDs.ResetAllBits();
    
    //reset phi distributions
    if(fEnablePhiDistrHistos) {
        fPhiVsCentrTPC[0]->Reset();
        fPhiVsCentrTPC[1]->Reset();
    }

    int nTracks=fAOD->GetNumberOfTracks();
    for(int iTrack=0; iTrack<nTracks; iTrack++) {
        AliAODTrack* track=dynamic_cast<AliAODTrack*>(fAOD->GetTrack(iTrack));
        double pt=track->Pt();
        double eta=track->Eta();
        if(!track || !IsTrackSelected(track)) continue;
        double phi=track->Phi();
        double pseudoRand = pt*1000.-(long)(pt*1000);
        if(pseudoRand>fFractionOfTracksForQnTPC) continue;
        if(track->GetID()>=0) fUsedTrackPosIDs.SetBitNumber(track->GetID());
        else fUsedTrackNegIDs.SetBitNumber(TMath::Abs(track->GetID()));
        double qx = TMath::Cos(harmonic*phi);
        double qy = TMath::Sin(harmonic*phi);
        double weight = 1.;
        if(eta>0) {
            if(fWeightsTPCPosEta[centbin]) {
                int phibin = fWeightsTPCPosEta[centbin]->GetXaxis()->FindBin(phi);
                weight = 1./fWeightsTPCPosEta[centbin]->GetBinContent(phibin);
            }
            QnVecTpcM[0] += weight*qx;
            QnVecTpcM[1] += weight*qy;
            Multi[0]     += weight;
            QnVecTpcP[0]  += weight*qx;
            QnVecTpcP[1]  += weight*qy;
            Multi[2]      += weight;
            if(fEnablePhiDistrHistos && fPhiVsCentrTPC[0])
                fPhiVsCentrTPC[0]->Fill(fCentrality,phi);
        }
        else {
            if(fWeightsTPCNegEta[centbin]) {
                int phibin = fWeightsTPCNegEta[centbin]->GetXaxis()->FindBin(phi);
                weight = 1./fWeightsTPCNegEta[centbin]->GetBinContent(phibin);
            }
            QnVecTpcM[0] += weight*qx;
            QnVecTpcM[1] += weight*qy;
            Multi[0]     += weight;
            QnVecTpcN[0]  += weight*qx;
            QnVecTpcN[1]  += weight*qy;
            Multi[1]     += weight;
            if(fEnablePhiDistrHistos && fPhiVsCentrTPC[1])
                fPhiVsCentrTPC[1]->Fill(fCentrality,phi);
        }
    }

    QnNorm[0] = TMath::Sqrt(QnVecTpcM[0]*QnVecTpcM[0]+QnVecTpcM[1]*QnVecTpcM[1]);
    QnNorm[2] = TMath::Sqrt(QnVecTpcP[0]*QnVecTpcP[0]+QnVecTpcP[1]*QnVecTpcP[1]);
    QnNorm[1] = TMath::Sqrt(QnVecTpcN[0]*QnVecTpcN[0]+QnVecTpcN[1]*QnVecTpcN[1]);

    Multi[0] > 0 ? QnNorm[0] = QnNorm[0] / TMath::Sqrt(Multi[0]) : QnNorm[0] = 0;
    Multi[1] > 0 ? QnNorm[1] = QnNorm[1] / TMath::Sqrt(Multi[1]) : QnNorm[1] = 0;
    Multi[2] > 0 ? QnNorm[2] = QnNorm[2] / TMath::Sqrt(Multi[2]) : QnNorm[2] = 0;
}

//__________________________________________________________
void AliAnalysisTaskEPCalibForJet::ComputeQvecV0(Double_t QnVecV0M[2],Double_t QnVecV0C[2],Double_t QnVecV0A[2], Double_t QnNorm[3], Double_t Multi[3], unsigned int harmonic)
{
    TString histName;
    TString groupName;
    groupName="EventPlane";
    
    //initialise Q vectors
    for(int iComp=0; iComp<2; iComp++) {
        QnVecV0M[iComp] = 0.;
        QnVecV0A[iComp] = 0.;
        QnVecV0C[iComp] = 0.;
    }

    for(int i=0; i<3; i++) {
        QnNorm[i] = 1.;
        Multi[i] = 0.;
    }
    
    short zvtxbin = GetVertexZbin();
    for (int iCh = 0; iCh < 64; iCh++) {
        
        double phiCh = TMath::PiOver4()*(0.5 + iCh % 8);
        double multv0 = fV0->GetMultiplicity(iCh);
        
        if (iCh < 32) { // V0C side
            double multCorC = -10;
            if (iCh < 8)
                multCorC = multv0/fHistMultV0->GetBinContent(iCh+1)*fHistMultV0->GetBinContent(1);
            else if (iCh >= 8 && iCh < 16)
                multCorC = multv0/fHistMultV0->GetBinContent(iCh+1)*fHistMultV0->GetBinContent(9);
            else if (iCh >= 16 && iCh < 24)
                multCorC = multv0/fHistMultV0->GetBinContent(iCh+1)*fHistMultV0->GetBinContent(17);
            else if (iCh >= 24 && iCh < 32)
                multCorC = multv0/fHistMultV0->GetBinContent(iCh+1)*fHistMultV0->GetBinContent(25);
            if (multCorC < 0) {
                AliWarning("Problem with multiplicity in V0C");
                continue;
            }

            befGainCalibQ2VecV0C[0] += TMath::Cos(harmonic*phiCh) * multv0;
            befGainCalibQ2VecV0C[1] += TMath::Sin(harmonic*phiCh) * multv0;
            befGainCalibQ2VecV0M[0] += TMath::Cos(harmonic*phiCh) * multv0;
            befGainCalibQ2VecV0M[1] += TMath::Sin(harmonic*phiCh) * multv0;
            
            QnVecV0C[0] += TMath::Cos(harmonic*phiCh) * multCorC;
            QnVecV0C[1] += TMath::Sin(harmonic*phiCh) * multCorC;
            QnVecV0M[0] += TMath::Cos(harmonic*phiCh) * multCorC;
            QnVecV0M[1] += TMath::Sin(harmonic*phiCh) * multCorC;

            Multi[1] += multCorC;  
            Multi[0] += multCorC;  
        } 
        else { // V0A side
            double multCorA = -10;
            
            if (iCh >= 32 && iCh < 40)
                multCorA = multv0/fHistMultV0->GetBinContent(iCh+1)*fHistMultV0->GetBinContent(33);
            else if (iCh >= 40 && iCh < 48)
                multCorA = multv0/fHistMultV0->GetBinContent(iCh+1)*fHistMultV0->GetBinContent(41);
            else if (iCh >= 48 && iCh < 56)
                multCorA = multv0/fHistMultV0->GetBinContent(iCh+1)*fHistMultV0->GetBinContent(49);
            else if (iCh >= 56 && iCh < 64)
                multCorA = multv0/fHistMultV0->GetBinContent(iCh+1)*fHistMultV0->GetBinContent(57);
            
            if (multCorA < 0) {
                AliWarning("Problem with multiplicity in V0A");
                continue;
            }

            befGainCalibQ2VecV0A[0] += TMath::Cos(harmonic*phiCh) * multv0;
            befGainCalibQ2VecV0A[1] += TMath::Sin(harmonic*phiCh) * multv0;
            befGainCalibQ2VecV0M[0] += TMath::Cos(harmonic*phiCh) * multv0;
            befGainCalibQ2VecV0M[1] += TMath::Sin(harmonic*phiCh) * multv0;

            QnVecV0A[0] += TMath::Cos(harmonic*phiCh) * multCorA;
            QnVecV0A[1] += TMath::Sin(harmonic*phiCh) * multCorA;
            QnVecV0M[0] += TMath::Cos(harmonic*phiCh) * multCorA;
            QnVecV0M[1] += TMath::Sin(harmonic*phiCh) * multCorA;

            Multi[2] += multCorA;  
            Multi[0] += multCorA;              
        }
    }
    
    
    if(0){
      if(harmonic == 2) std::cout << "(Mult V0M, V0C, V0A) = (" << Multi[0] << ", " << Multi[1] << ", " << Multi[2]<< ")" << std::endl;
      if(harmonic == 2) std::cout << "Aft Gain Calib Qx (V0C, V0A) = ("  << QnVecV0C[0] << ", " << QnVecV0A[0] << ")"\
        << ", Qy (V0C, V0A) = ("  << QnVecV0C[1] << ", " << QnVecV0A[1] << ")"<< std::endl;
    }

    histName;
    groupName;
    groupName="GainCalib";

    Int_t iCentBin = GetCentBin();
    if(Multi[0] != 0.){
      histName = TString::Format("%s/CentGainCalibImpQ2x_V0M", groupName.Data());
      fHistManager.FillTH2(histName, iCentBin, (QnVecV0M[0]-befGainCalibQ2VecV0M[0])/Multi[0]);
      histName = TString::Format("%s/CentGainCalibImpQ2y_V0M", groupName.Data());
      fHistManager.FillTH2(histName, iCentBin, (QnVecV0M[1]-befGainCalibQ2VecV0M[1])/Multi[0]);

      histName = TString::Format("%s/CentGainCalibImpQ3x_V0M", groupName.Data());
      fHistManager.FillTH2(histName, iCentBin, (QnVecV0M[0]-befGainCalibQ3VecV0M[0])/Multi[0]);
      histName = TString::Format("%s/CentGainCalibImpQ3y_V0M", groupName.Data());
      fHistManager.FillTH2(histName, iCentBin, (QnVecV0M[1]-befGainCalibQ3VecV0M[1])/Multi[0]);
    }
    if(Multi[1] != 0.){
      histName = TString::Format("%s/CentGainCalibImpQ2x_V0C", groupName.Data());
      fHistManager.FillTH2(histName, iCentBin, (QnVecV0C[0]-befGainCalibQ2VecV0C[0])/Multi[1]);
      histName = TString::Format("%s/CentGainCalibImpQ2y_V0C", groupName.Data());
      fHistManager.FillTH2(histName, iCentBin, (QnVecV0C[1]-befGainCalibQ2VecV0C[1])/Multi[1]);

      histName = TString::Format("%s/CentGainCalibImpQ3x_V0A", groupName.Data());
      fHistManager.FillTH2(histName, iCentBin, (QnVecV0A[0]-befGainCalibQ3VecV0A[0])/Multi[1]);
      histName = TString::Format("%s/CentGainCalibImpQ3y_V0A", groupName.Data());
      fHistManager.FillTH2(histName, iCentBin, (QnVecV0A[1]-befGainCalibQ3VecV0A[1])/Multi[1]);
    }
    if(Multi[2] != 0.){
      histName = TString::Format("%s/CentGainCalibImpQ2x_V0A", groupName.Data());
      fHistManager.FillTH2(histName, iCentBin, (QnVecV0A[0]-befGainCalibQ2VecV0A[0])/Multi[2]);
      histName = TString::Format("%s/CentGainCalibImpQ2y_V0A", groupName.Data());
      fHistManager.FillTH2(histName, iCentBin, (QnVecV0A[1]-befGainCalibQ2VecV0A[1])/Multi[2]);

      histName = TString::Format("%s/CentGainCalibImpQ3x_V0C", groupName.Data());
      fHistManager.FillTH2(histName, iCentBin, (QnVecV0C[0]-befGainCalibQ3VecV0C[0])/Multi[2]);
      histName = TString::Format("%s/CentGainCalibImpQ3y_V0C", groupName.Data());
      fHistManager.FillTH2(histName, iCentBin, (QnVecV0C[1]-befGainCalibQ3VecV0C[1])/Multi[2]);
    }
      
    
    if(fReCentCalibQA){
      TString histName;
      TString groupName;
      groupName="ReCentCalib";

      histName = TString::Format("%s/hAvgQ%dXvsCentV0MBef_%d", groupName.Data(), harmonic, fRunNumber);
      fHistManager.FillProfile(histName, iCentBin, QnVecV0M[0]);
      histName = TString::Format("%s/hAvgQ%dYvsCentV0MBef_%d", groupName.Data(), harmonic, fRunNumber);
      fHistManager.FillProfile(histName, iCentBin, QnVecV0M[1]);

      histName = TString::Format("%s/hAvgQ%dXvsCentV0CBef_%d", groupName.Data(), harmonic, fRunNumber);
      fHistManager.FillProfile(histName, iCentBin, QnVecV0C[0]);
      histName = TString::Format("%s/hAvgQ%dYvsCentV0CBef_%d", groupName.Data(), harmonic, fRunNumber);
      fHistManager.FillProfile(histName, iCentBin, QnVecV0C[1]);

      histName = TString::Format("%s/hAvgQ%dXvsCentV0ABef_%d", groupName.Data(), harmonic, fRunNumber);
      fHistManager.FillProfile(histName, iCentBin, QnVecV0A[0]);

      histName = TString::Format("%s/hAvgQ%dYvsCentV0ABef_%d", groupName.Data(), harmonic, fRunNumber);
      fHistManager.FillProfile(histName, iCentBin, QnVecV0A[1]);
    }

    // int iCentBin = static_cast<int>(fCentrality)+1;

    //only recentering and not width equalisation to preserve multiplicity dependence (needed for qn)
    if(harmonic == 2){
        Int_t tempCentBin = fQx2mV0A[zvtxbin]->FindBin(fCent);
        QnVecV0A[0] = (QnVecV0A[0] - fQx2mV0A[zvtxbin]->GetBinContent(tempCentBin));///fQx2sV0A[zvtxbin]->GetBinContent(iCentBin);
        QnVecV0A[1] = (QnVecV0A[1] - fQy2mV0A[zvtxbin]->GetBinContent(tempCentBin));///fQy2sV0A[zvtxbin]->GetBinContent(iCentBin);
        QnVecV0C[0] = (QnVecV0C[0] - fQx2mV0C[zvtxbin]->GetBinContent(tempCentBin));///fQx2sV0C[zvtxbin]->GetBinContent(iCentBin);   
        QnVecV0C[1] = (QnVecV0C[1] - fQy2mV0C[zvtxbin]->GetBinContent(tempCentBin));///fQy2sV0C[zvtxbin]->GetBinContent(iCentBin);

        if(fReCentCalibQA){
          histName;
          groupName="ReCentCalib";

          histName = TString::Format("%s/hAvgQ%dXvsCentV0MAft_%d", groupName.Data(), harmonic, fRunNumber);
          fHistManager.FillProfile(histName, iCentBin, QnVecV0M[0]);
          histName = TString::Format("%s/hAvgQ%dYvsCentV0MAft_%d", groupName.Data(),harmonic,  fRunNumber);
          fHistManager.FillProfile(histName, iCentBin, QnVecV0M[1]);

          histName = TString::Format("%s/hAvgQ%dXvsCentV0CAft_%d", groupName.Data(), harmonic, fRunNumber);
          fHistManager.FillProfile(histName, iCentBin, QnVecV0C[0]);
          histName = TString::Format("%s/hAvgQ%dYvsCentV0CAft_%d", groupName.Data(), harmonic, fRunNumber);
          fHistManager.FillProfile(histName, iCentBin, QnVecV0C[1]);
          
          histName = TString::Format("%s/hAvgQ%dXvsCentV0AAft_%d", groupName.Data(), harmonic, fRunNumber);
          fHistManager.FillProfile(histName, iCentBin, QnVecV0A[0]);

          histName = TString::Format("%s/hAvgQ%dYvsCentV0AAft_%d", groupName.Data(), harmonic, fRunNumber);
          fHistManager.FillProfile(histName, iCentBin, QnVecV0A[1]);

          histName = TString::Format("%s/hCentVsQxV0MAftCalib", groupName.Data());
          fHistManager.FillTH2(histName, iCentBin, q2VecV0M[0]);
          histName = TString::Format("%s/hCentVsQxV0MAftCalib", groupName.Data());
          fHistManager.FillTH2(histName, iCentBin, q2VecV0M[1]);
          histName = TString::Format("%s/hCentVsQxV0CAftCalib", groupName.Data());
          fHistManager.FillTH2(histName, iCentBin, q2VecV0C[0]);
          histName = TString::Format("%s/hCentVsQxV0CAftCalib", groupName.Data());
          fHistManager.FillTH2(histName, iCentBin, q2VecV0C[1]);
          histName = TString::Format("%s/hCentVsQxV0AAftCalib", groupName.Data());
          fHistManager.FillTH2(histName, iCentBin, q2VecV0A[0]);
          histName = TString::Format("%s/hCentVsQxV0AAftCalib", groupName.Data());
          fHistManager.FillTH2(histName, iCentBin, q2VecV0A[1]);
        }
        // Double_t avgqxC = fQx2mV0C[zvtxbin]->GetBinContent(iCentBin);
        // Double_t avgqyC = fQy2mV0C[zvtxbin]->GetBinContent(iCentBin);
        // Double_t avgqxA = fQx2mV0A[zvtxbin]->GetBinContent(iCentBin);
        // Double_t avgqyA = fQy2mV0A[zvtxbin]->GetBinContent(iCentBin);
        // std::cout << "C Q2x, Q2y : avgqx, avgqy = " << QnVecV0C[0] << ", " << QnVecV0C[1] << " : " << avgqxC << ", " << avgqyC << std::endl;
        // std::cout << "A Q2x, Q2y : avgqx, avgqy = " << QnVecV0A[0] << ", " << QnVecV0A[1] << " : " << avgqxA << ", " << avgqyA << std::endl;
    }
    else if(harmonic == 3){
        Int_t tempCentBin = fQx3mV0A[zvtxbin]->FindBin(fCent);
        QnVecV0A[0] = (QnVecV0A[0] - fQx3mV0A[zvtxbin]->GetBinContent(tempCentBin));///fQx2sV0A[zvtxbin]->GetBinContent(iCentBin);
        QnVecV0A[1] = (QnVecV0A[1] - fQy3mV0A[zvtxbin]->GetBinContent(tempCentBin));///fQy2sV0A[zvtxbin]->GetBinContent(iCentBin);
        QnVecV0C[0] = (QnVecV0C[0] - fQx3mV0C[zvtxbin]->GetBinContent(tempCentBin));///fQx2sV0C[zvtxbin]->GetBinContent(iCentBin);   
        QnVecV0C[1] = (QnVecV0C[1] - fQy3mV0C[zvtxbin]->GetBinContent(tempCentBin));///fQy2sV0C[zvtxbin]->GetBinContent(iCentBin);

        if(fReCentCalibQA){
          histName = "";
          groupName="ReCentCalib";

          histName = TString::Format("%s/hAvgQ%dXvsCentV0MAft_%d", groupName.Data(), harmonic, fRunNumber);
          fHistManager.FillProfile(histName, iCentBin, QnVecV0M[0]);
          histName = TString::Format("%s/hAvgQ%dYvsCentV0MAft_%d", groupName.Data(), harmonic, fRunNumber);
          fHistManager.FillProfile(histName, iCentBin, QnVecV0M[1]);

          histName = TString::Format("%s/hAvgQ%dXvsCentV0CAft_%d", groupName.Data(), harmonic, fRunNumber);
          fHistManager.FillProfile(histName, iCentBin, QnVecV0C[0]);
          histName = TString::Format("%s/hAvgQ%dYvsCentV0CAft_%d", groupName.Data(), harmonic, fRunNumber);
          fHistManager.FillProfile(histName, iCentBin, QnVecV0C[1]);
          
          histName = TString::Format("%s/hAvgQ%dXvsCentV0AAft_%d", groupName.Data(), harmonic, fRunNumber);
          fHistManager.FillProfile(histName, iCentBin, QnVecV0A[0]);

          histName = TString::Format("%s/hAvgQ%dYvsCentV0AAft_%d", groupName.Data(), harmonic, fRunNumber);
          fHistManager.FillProfile(histName, iCentBin, QnVecV0A[1]);


          // histName = TString::Format("%s/hCentVsQxV0MAftCalib", groupName.Data());
          // fHistManager.FillTH2(histName, fCent, q2VecV0M[0]);
          // histName = TString::Format("%s/hCentVsQyV0MAftCalib", groupName.Data());
          // fHistManager.FillTH2(histName, fCent, q2VecV0M[1]);
          // histName = TString::Format("%s/hCentVsQxV0CAftCalib", groupName.Data());
          // fHistManager.FillTH2(histName, fCent, q2VecV0C[0]);
          // histName = TString::Format("%s/hCentVsQyV0CAftCalib", groupName.Data());
          // fHistManager.FillTH2(histName, fCent, q2VecV0C[1]);
          // histName = TString::Format("%s/hCentVsQxV0AAftCalib", groupName.Data());
          // fHistManager.FillTH2(histName, fCent, q2VecV0A[0]);
          // histName = TString::Format("%s/hCentVsQyV0AAftCalib", groupName.Data());
          // fHistManager.FillTH2(histName, fCent, q2VecV0A[1]);
        }
    }

    QnVecV0M[0] /= Multi[0];
    QnVecV0M[1] /= Multi[0];
    QnVecV0C[0] /= Multi[1];
    QnVecV0C[1] /= Multi[1];
    QnVecV0A[0] /= Multi[2];
    QnVecV0A[1] /= Multi[2];

    QnNorm[0] = TMath::Sqrt(QnVecV0M[0]*QnVecV0M[0]+QnVecV0M[1]*QnVecV0M[1])*TMath::Sqrt(Multi[0]);
    QnNorm[1] = TMath::Sqrt(QnVecV0C[0]*QnVecV0C[0]+QnVecV0C[1]*QnVecV0C[1])*TMath::Sqrt(Multi[1]);
    QnNorm[2] = TMath::Sqrt(QnVecV0A[0]*QnVecV0A[0]+QnVecV0A[1]*QnVecV0A[1])*TMath::Sqrt(Multi[2]);

    // QnNorm[0] = TMath::Sqrt(QnVecV0M[0]*QnVecV0M[0]+QnVecV0M[1]*QnVecV0M[1]);
    // QnNorm[1] = TMath::Sqrt(QnVecV0C[0]*QnVecV0C[0]+QnVecV0C[1]*QnVecV0C[1]);
    // QnNorm[2] = TMath::Sqrt(QnVecV0A[0]*QnVecV0A[0]+QnVecV0A[1]*QnVecV0A[1]);
}


//__________________________________________________________
short AliAnalysisTaskEPCalibForJet::GetVertexZbin() const
{
    if(!fV0CalibZvtxDiff)
        return 0; //if it is not Zvtx differential, always first bin

    short zvtxbin = -10;
    
    if (fZvtx >= -10. && fZvtx < -8.) zvtxbin = 0;
    else if (fZvtx >= -8. && fZvtx < -6.) zvtxbin = 1;
    else if (fZvtx >= -6. && fZvtx < -4.) zvtxbin = 2;
    else if (fZvtx >= -4. && fZvtx < -3.) zvtxbin = 3;
    else if (fZvtx >= -3. && fZvtx < -2.) zvtxbin = 4;
    else if (fZvtx >= -2. && fZvtx < -1.) zvtxbin = 5;
    else if (fZvtx >= -1. && fZvtx < 0)   zvtxbin = 6;
    else if (fZvtx >= 0 && fZvtx < 1.)    zvtxbin = 7;
    else if (fZvtx >= 1. && fZvtx < 2.)   zvtxbin = 8;
    else if (fZvtx >= 2. && fZvtx < 3.)   zvtxbin = 9;
    else if (fZvtx >= 3. && fZvtx < 4.)   zvtxbin = 10;
    else if (fZvtx >= 4. && fZvtx < 6.)   zvtxbin = 11;
    else if (fZvtx >= 6. && fZvtx < 8.)   zvtxbin = 12;
    else if (fZvtx >= 8. && fZvtx <= 10.) zvtxbin = 13;
    
    return zvtxbin; 
}

//__________________________________________________________
Int_t AliAnalysisTaskEPCalibForJet::GetCentBin()
{
    Int_t centbin = 10;
    
    if (fCentrality >= 0. && fCentrality < 5.)       centbin = 0;
    else if (fCentrality >= 5.  && fCentrality < 10.) centbin = 1;
    else if (fCentrality >= 10. && fCentrality < 20.) centbin = 2;
    else if (fCentrality >= 20. && fCentrality < 30.) centbin = 3;
    else if (fCentrality >= 30. && fCentrality < 40.) centbin = 4;
    else if (fCentrality >= 40. && fCentrality < 50.) centbin = 5;
    else if (fCentrality >= 50. && fCentrality < 60.) centbin = 6;
    else if (fCentrality >= 60. && fCentrality < 70.) centbin = 7;
    else if (fCentrality >= 70. && fCentrality < 80.) centbin = 8;
    else if (fCentrality >= 80. && fCentrality < 90.) centbin = 9;

    return centbin;
}

//__________________________________________________________
bool AliAnalysisTaskEPCalibForJet::IsTrackSelected(AliAODTrack* track) {

    if(fRun>=244918 && fRun<=246994) {//PbPb2015
        if(fCalibType==kQnCalib) {
            if(!track->TestFilterBit(BIT(8)) && !track->TestFilterBit(BIT(9)))
                return false;
            if(track->Pt()<fPtMinTPC || track->Pt()>fPtMaxTPC || TMath::Abs(track->Eta())>fEtaMaxTPC || TMath::Abs(track->Eta())<fEtaMinTPC)
                return false;
        }
        else if(fCalibType==kQnFrameworkCalib) {
            if(!track->TestFilterBit(BIT(8)) && !track->TestFilterBit(BIT(9)))
                return false;
            if(track->Pt()<0.2 || track->Pt()>5 || TMath::Abs(track->Eta())>0.8 || TMath::Abs(track->Eta())<0.)
                return false;
        }
    }
    else if(fRun>=295581 && fRun<=297317) { //PbPb2018
        if(fCalibType==kQnCalib) {
            if(!track->TestFilterBit(BIT(8)) && !track->TestFilterBit(BIT(9)))
                return false;
            if(track->Pt()<fPtMinTPC || track->Pt()>fPtMaxTPC || TMath::Abs(track->Eta())>fEtaMaxTPC || TMath::Abs(track->Eta())<fEtaMinTPC)
                return false;
        }
        else if(fCalibType==kQnFrameworkCalib) {
            if(!track->TestFilterBit(BIT(8)) && !track->TestFilterBit(BIT(9)))
                return false;
            if(track->Pt()<0.2 || track->Pt()>5 || TMath::Abs(track->Eta())>0.8 || TMath::Abs(track->Eta())<0.)
                return false;
        }
    }
    else { //default
        if(fCalibType==kQnCalib) {
            if(!track->TestFilterBit(BIT(8)) && !track->TestFilterBit(BIT(9)))
                return false;
            if(track->Pt()<fPtMinTPC || track->Pt()>fPtMaxTPC || TMath::Abs(track->Eta())>fEtaMaxTPC || TMath::Abs(track->Eta())<fEtaMinTPC)
                return false;
        }
        else if(fCalibType==kQnFrameworkCalib) {
            if(!track->TestFilterBit(BIT(8)) && !track->TestFilterBit(BIT(9)))
                return false;
            if(track->Pt()<0.2 || track->Pt()>5 || TMath::Abs(track->Eta())>0.8 || TMath::Abs(track->Eta())<0.)
                return false;
        }
    }

    return true;
}

void AliAnalysisTaskEPCalibForJet::ReSetValuable(){
  
  for(int iXY = 0; iXY < 2; iXY++){
    befGainCalibQ2VecV0M[iXY] = -999.;
    befGainCalibQ2VecV0C[iXY] = -999.;
    befGainCalibQ2VecV0A[iXY] = -999.;
    
    befGainCalibQ3VecV0M[iXY] = -999.;
    befGainCalibQ3VecV0C[iXY] = -999.;
    befGainCalibQ3VecV0A[iXY] = -999.;

    q2VecV0M[iXY] = -999.;
    q2VecV0C[iXY] = -999.;
    q2VecV0A[iXY] = -999.;

    q3VecV0M[iXY] = -999.;
    q3VecV0C[iXY] = -999.;
    q3VecV0A[iXY] = -999.;

    q2VecTpcM[iXY] = -999.;
    q2VecTpcP[iXY] = -999.;
    q2VecTpcN[iXY] = -999.;

    q3VecTpcM[iXY] = -999.;
    q3VecTpcP[iXY] = -999.;
    q3VecTpcN[iXY] = -999.;
  }


  for(Int_t i(0); i < 3; i++){
    V0Mult2[i] = 0.;
    V0Mult3[i] = 0.;
    TpcMult2[i] = 0.;
    TpcMult3[i] = 0.;

    befGainCalibQ2V0[i] = -9999.;
    befGainCalibQ3V0[i] = -9999.;

    q2V0[i]  = -9999.;
    q3V0[i]  = -9999.;
    q2Tpc[i] = -9999.;
    q3Tpc[i] = -9999.;

    psi2V0[i]  = -9999.;
    psi3V0[i]  = -9999.;
    psi2Tpc[i] = -9999.;
    psi3Tpc[i] = -9999.;

    q2NormV0[i] = -9999.;
    q3NormV0[i] = -9999.;
    q2NormTpc[i] = -9999.;
    q3NormTpc[i] = -9999.;
  }

}


/**
 * This function is called once at the end of the analysis.
 */
void AliAnalysisTaskEPCalibForJet::Terminate(Option_t *) 
{
}



/// ==========================================================================================
AliAnalysisTaskEPCalibForJet * AliAnalysisTaskEPCalibForJet::AddTaskEPCalibForJet(
    TString EPCailbType,
    TString EPCalibJEHandRefFileName, TString EPCalibOrigRefFileName,
    const char *ntracks, const char *nclusters, const char* ncells, const char *suffix)
{
    // Get the pointer to the existing analysis manager via the static access method.
    //==============================================================================
    AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
    if (!mgr)
    {
        ::Error("AddTaskEPCalibForJet", "No analysis manager to connect to.");
        return 0;
    }

    // Check the analysis type using the event handlers connected to the analysis manager.
    //==============================================================================
    AliVEventHandler* handler = mgr->GetInputEventHandler();
    if (!handler)
    {
        ::Error("AddTaskEPCalibForJet", "This task requires an input event handler");
        return 0;
    }
    
    enum EDataType_t {kUnknown, kESD, kAOD};
    EDataType_t dataType = kAOD;
    
    //-------------------------------------------------------
    // Init the task and do settings
    //-------------------------------------------------------
    TString trackName(ntracks);
    if (trackName == "usedefault") trackName = "tracks";

    TString name("AliAnalysisTaskEPCalibForJet");
    if (strcmp(suffix,"") != 0) {
        name += "_";
        name += suffix;
    }
    
    AliAnalysisTaskEPCalibForJet* epCalibTask = new AliAnalysisTaskEPCalibForJet(name);
    epCalibTask->SetQnCalibType(EPCailbType); //kJeHand, kOrig
    epCalibTask->SetVzRange(-10,10);
    
    if(EPCailbType == "kOrig"){
        TList *lCalibRefHists = NULL;

        TString tempCalibFileName = AliDataFile::GetFileName(EPCalibOrigRefFileName.Data());
        TString tempCalibLocalFileName = EPCalibOrigRefFileName;
        
        // Check access to CVMFS (will only be displayed locally)
        if(EPCalibOrigRefFileName.BeginsWith("alien://") && !gGrid){
            TGrid::Connect("alien://");
        }
        TFile* EPCalibRefFile = NULL;
        if(!tempCalibFileName.IsNull()) EPCalibRefFile = TFile::Open(tempCalibFileName.Data());
        if(tempCalibFileName.IsNull())  EPCalibRefFile = TFile::Open(tempCalibLocalFileName.Data());
        lCalibRefHists = (TList *)EPCalibRefFile->Get("fWgtsV0ZDC");
        
        epCalibTask->SetCalibOrigRefObjList(lCalibRefHists);
    }
    else if(EPCailbType == "kJeHand"){
        TString pathToFileLocal = EPCalibJEHandRefFileName;

        TString tempCalibFileName = AliDataFile::GetFileName(EPCalibJEHandRefFileName.Data());
        TString tempCalibLocalFileName = EPCalibJEHandRefFileName;
        
        // Check access to CVMFS (will only be displayed locally)
        if(EPCalibJEHandRefFileName.BeginsWith("alien://") && !gGrid){
            // AliInfo("Trying to connect to AliEn ...");
            TGrid::Connect("alien://");
        }
        TFile* EPCalibRefFile = NULL;
        if(!tempCalibFileName.IsNull()) EPCalibRefFile = TFile::Open(tempCalibFileName.Data());
        if(tempCalibFileName.IsNull())  EPCalibRefFile = TFile::Open(tempCalibLocalFileName.Data());

        AliOADBContainer *lRefMultV0BefCorPfpx = new AliOADBContainer();
        TObjArray *lRefQx2am = new TObjArray();
        TObjArray *lRefQy2am = new TObjArray();
        TObjArray *lRefQx2as = new TObjArray();
        TObjArray *lRefQy2as = new TObjArray();
        TObjArray *lRefQx3am = new TObjArray();
        TObjArray *lRefQy3am = new TObjArray();
        TObjArray *lRefQx3as = new TObjArray();
        TObjArray *lRefQy3as = new TObjArray();
        TObjArray *lRefQx2cm = new TObjArray();
        TObjArray *lRefQy2cm = new TObjArray();
        TObjArray *lRefQx2cs = new TObjArray();
        TObjArray *lRefQy2cs = new TObjArray();
        TObjArray *lRefQx3cm = new TObjArray();
        TObjArray *lRefQy3cm = new TObjArray(); 
        TObjArray *lRefQx3cs = new TObjArray(); 
        TObjArray *lRefQy3cs = new TObjArray();
        TObjArray *lRefTPCposEta = new TObjArray();;
        TObjArray *lRefTPCnegEta = new TObjArray();;

        lRefMultV0BefCorPfpx = (AliOADBContainer *) EPCalibRefFile->Get("hMultV0BefCorPfpx");

        bool LoadedCaliRef = epCalibTask->ExtractRecentPara(EPCalibRefFile, lRefQx2am, lRefQy2am, lRefQx2as, lRefQy2as, lRefQx3am, lRefQy3am, lRefQx3as, lRefQy3as, lRefQx2cm, lRefQy2cm, lRefQx2cs, lRefQy2cs, lRefQx3cm,lRefQy3cm, lRefQx3cs, lRefQy3cs, lRefTPCposEta, lRefTPCnegEta);
        if (!LoadedCaliRef) {
            std::cout << "Calibrations failed to load!\n" << std::endl;
        } else {
            std::cout << "Calibrations loaded correctly!\n" << std::endl;
        }
        

        epCalibTask->SetLRefMultV0BefCorPfpx(lRefMultV0BefCorPfpx);
        epCalibTask->SetLRefQx2am(lRefQx2am);
        epCalibTask->SetLRefQy2am(lRefQy2am);
        epCalibTask->SetLRefQx2as(lRefQx2as);
        epCalibTask->SetLRefQy2as(lRefQy2as);
        epCalibTask->SetLRefQx3am(lRefQx3am);
        epCalibTask->SetLRefQy3am(lRefQy3am);
        epCalibTask->SetLRefQx3as(lRefQx3as);
        epCalibTask->SetLRefQy3as(lRefQy3as);
        epCalibTask->SetLRefQx2cm(lRefQx2cm);
        epCalibTask->SetLRefQy2cm(lRefQy2cm);
        epCalibTask->SetLRefQx2cs(lRefQx2cs);
        epCalibTask->SetLRefQy2cs(lRefQy2cs);
        epCalibTask->SetLRefQx3cm(lRefQx3cm);
        epCalibTask->SetLRefQy3cm(lRefQy3cm);
        epCalibTask->SetLRefQx3cs(lRefQx3cs);
        epCalibTask->SetLRefQy3cs(lRefQy3cs);
        epCalibTask->SetLRefTPCposEta(lRefTPCposEta);
        epCalibTask->SetLRefTPCnegEta(lRefTPCnegEta);
        
        if(lRefMultV0BefCorPfpx) delete lRefMultV0BefCorPfpx;
        if(lRefQx2am) delete lRefQx2am;
        if(lRefQy2am) delete lRefQy2am;
        if(lRefQx2as) delete lRefQx2as;
        if(lRefQy2as) delete lRefQy2as;
        if(lRefQx3am) delete lRefQx3am;
        if(lRefQy3am) delete lRefQy3am;
        if(lRefQx3as) delete lRefQx3as;
        if(lRefQy3as) delete lRefQy3as;
        if(lRefQx2cm) delete lRefQx2cm;
        if(lRefQy2cm) delete lRefQy2cm;
        if(lRefQx2cs) delete lRefQx2cs;
        if(lRefQy2cs) delete lRefQy2cs;
        if(lRefQx3cm) delete lRefQx3cm;
        if(lRefQy3cm) delete lRefQy3cm;
        if(lRefQx3cs) delete lRefQx3cs;
        if(lRefQy3cs) delete lRefQy3cs;
        if(lRefTPCposEta) delete lRefTPCposEta;
        if(lRefTPCnegEta) delete lRefTPCnegEta;
        
    }
    

    if (trackName == "mcparticles") epCalibTask->AddMCParticleContainer(trackName);
    else if (trackName == "tracks") epCalibTask->AddTrackContainer(trackName);
    else if (!trackName.IsNull()) epCalibTask->AddParticleContainer(trackName);

    //-------------------------------------------------------
    // Final settings, pass to manager and set the containers
    //-------------------------------------------------------
    mgr->AddTask(epCalibTask);
    
    // Create containers for input/output
    AliAnalysisDataContainer *cinput1  = mgr->GetCommonInputContainer()  ;
    TString contname(name);
    contname += "_histos";
    AliAnalysisDataContainer *coutput1 = mgr->CreateContainer(contname.Data(),
        TList::Class(),AliAnalysisManager::kOutputContainer,
        Form("%s", AliAnalysisManager::GetCommonFileName()));
    mgr->ConnectInput  (epCalibTask, 0,  cinput1 );
    mgr->ConnectOutput (epCalibTask, 1, coutput1 );
    
    std::cout << "Success add AliAnalysisTaskRawJetWithEP!!" << std::endl;

    return epCalibTask;
}


bool AliAnalysisTaskEPCalibForJet::ExtractRecentPara(TFile *EPCalibRefFile, TObjArray *lRefQx2am, TObjArray *lRefQy2am, TObjArray *lRefQx2as, TObjArray *lRefQy2as, TObjArray *lRefQx3am, TObjArray *lRefQy3am, TObjArray *lRefQx3as, TObjArray *lRefQy3as, TObjArray *lRefQx2cm, TObjArray *lRefQy2cm, TObjArray *lRefQx2cs, TObjArray *lRefQy2cs, TObjArray *lRefQx3cm,TObjArray *lRefQy3cm, TObjArray *lRefQx3cs, TObjArray *lRefQy3cs, TObjArray *lRefTPCposEta, TObjArray *lRefTPCnegEta) {
    for(int iZvtx = 0; iZvtx < 14; iZvtx++) {
        // V0 A-side #################################################################
        // Mean Qx correction
        // Includes check if Zvtx is differential
        AliOADBContainer* contQx2am = (AliOADBContainer*) EPCalibRefFile->Get(Form("fqxa2m_%d", iZvtx));
        if(!contQx2am) { //check if it is not Zvtx differential
            contQx2am = (AliOADBContainer*) EPCalibRefFile->Get("fqxa2m");
            if(contQx2am) fV0CalibZvtxDiff = false;
        }
        if(!contQx2am) {
            AliWarning("OADB object fqxa2m is not available in the file\n");
            return false;
        }
        
        lRefQx2am->Add(contQx2am);
        
        AliOADBContainer* contQy2am = nullptr; // Mean Qy correction
        AliOADBContainer* contQx2as = nullptr; // Sigma Qx correction
        AliOADBContainer* contQy2as = nullptr; // Sigma Qy correction
        if(fV0CalibZvtxDiff){
            contQy2am = (AliOADBContainer*) EPCalibRefFile->Get(Form("fqya2m_%d", iZvtx));
            contQx2as = (AliOADBContainer*) EPCalibRefFile->Get(Form("fqxa2s_%d", iZvtx));
            contQy2as = (AliOADBContainer*) EPCalibRefFile->Get(Form("fqya2s_%d", iZvtx));
        }
        else {
            contQy2am = (AliOADBContainer*) EPCalibRefFile->Get("fqya2m");
            contQx2as = (AliOADBContainer*) EPCalibRefFile->Get("fqxa2s");
            contQy2as = (AliOADBContainer*) EPCalibRefFile->Get("fqya2s");
        }
        if(!contQy2am) {
            AliWarning("OADB object fqya2m is not available in the file\n");
            return false;
        }
        lRefQy2am->Add(contQy2am);
        if(!contQx2as) {
            AliWarning("OADB object fqxa2s is not available in the file\n");
            return false;
        }
        lRefQx2as->Add(contQx2as);
        if(!contQy2as) {
            AliWarning("OADB object fqya2s is not available in the file\n");
            return false;
        }
        lRefQy2as->Add(contQy2as);


        AliOADBContainer* contQx3am = (AliOADBContainer*) EPCalibRefFile->Get(Form("fqxa3m_%d", iZvtx));
        if(!contQx3am) { //check if it is not Zvtx differential
            contQx3am = (AliOADBContainer*) EPCalibRefFile->Get("fqxa3m");
            if(contQx3am) fV0CalibZvtxDiff = false;
        }
        if(!contQx3am) {
            AliWarning("OADB object fqxa3m is not available in the file\n");
            return false;
        }
        lRefQx3am->Add(contQx3am);

        
        AliOADBContainer* contQy3am = nullptr; // Mean Qy correction
        AliOADBContainer* contQx3as = nullptr; // Sigma Qx correction
        AliOADBContainer* contQy3as = nullptr; // Sigma Qy correction
        if(fV0CalibZvtxDiff){
            contQy3am = (AliOADBContainer*) EPCalibRefFile->Get(Form("fqya3m_%d", iZvtx));
            contQx3as = (AliOADBContainer*) EPCalibRefFile->Get(Form("fqxa3s_%d", iZvtx));
            contQy3as = (AliOADBContainer*) EPCalibRefFile->Get(Form("fqya3s_%d", iZvtx));
        }
        else {
            contQy3am = (AliOADBContainer*) EPCalibRefFile->Get("fqya3m");
            contQx3as = (AliOADBContainer*) EPCalibRefFile->Get("fqxa3s");
            contQy3as = (AliOADBContainer*) EPCalibRefFile->Get("fqya3s");
        }
        if(!contQy3am) {
            AliWarning("OADB object fqya3m is not available in the file\n");
            return false;
        }
        lRefQy3am->Add(contQy3am);

        if(!contQx3as) {
            AliWarning("OADB object fqxa3s is not available in the file\n");
            return false;
        }
        lRefQx3as->Add(contQx3as);

        if(!contQy3as) {
            AliWarning("OADB object fqya3s is not available in the file\n");
            return false;
        }
        lRefQy3as->Add(contQy3as);


        // V0 C-side  #################################################################
        AliOADBContainer* contQx2cm = nullptr; // Mean Qx correction
        AliOADBContainer* contQy2cm = nullptr; // Mean Qy correction
        AliOADBContainer* contQx2cs = nullptr; // Sigma Qx correction
        AliOADBContainer* contQy2cs = nullptr; // Sigma Qy correction

        AliOADBContainer* contQx3cm = nullptr; // Mean Qx correction
        AliOADBContainer* contQy3cm = nullptr; // Mean Qy correction
        AliOADBContainer* contQx3cs = nullptr; // Sigma Qx correction
        AliOADBContainer* contQy3cs = nullptr; // Sigma Qy correction
        if(fV0CalibZvtxDiff){
            contQx2cm = (AliOADBContainer*) EPCalibRefFile->Get(Form("fqxc2m_%d", iZvtx));
            contQy2cm = (AliOADBContainer*) EPCalibRefFile->Get(Form("fqyc2m_%d", iZvtx));
            contQx2cs = (AliOADBContainer*) EPCalibRefFile->Get(Form("fqxc2s_%d", iZvtx));
            contQy2cs = (AliOADBContainer*) EPCalibRefFile->Get(Form("fqyc2s_%d", iZvtx));
            
            contQx3cm = (AliOADBContainer*) EPCalibRefFile->Get(Form("fqxc3m_%d", iZvtx));
            contQy3cm = (AliOADBContainer*) EPCalibRefFile->Get(Form("fqyc3m_%d", iZvtx));
            contQx3cs = (AliOADBContainer*) EPCalibRefFile->Get(Form("fqxc3s_%d", iZvtx));
            contQy3cs = (AliOADBContainer*) EPCalibRefFile->Get(Form("fqyc3s_%d", iZvtx));
        }
        else{
            contQx2cm = (AliOADBContainer*) EPCalibRefFile->Get("fqxc2m");
            contQy2cm = (AliOADBContainer*) EPCalibRefFile->Get("fqyc2m");
            contQx2cs = (AliOADBContainer*) EPCalibRefFile->Get("fqxc2s");
            contQy2cs = (AliOADBContainer*) EPCalibRefFile->Get("fqyc2s");

            contQx3cm = (AliOADBContainer*) EPCalibRefFile->Get("fqxc3m");
            contQy3cm = (AliOADBContainer*) EPCalibRefFile->Get("fqyc3m");
            contQx3cs = (AliOADBContainer*) EPCalibRefFile->Get("fqxc3s");
            contQy3cs = (AliOADBContainer*) EPCalibRefFile->Get("fqyc3s");
        }

        if(!contQx2cm) {
            AliWarning("OADB object fqxc2m is not available in the file\n");
            return false;
        }
        lRefQx2cm->Add(contQx2cm);

        if(!contQy2cm) {
            AliWarning("OADB object fqyc2m is not available in the file\n");
            return false;
        }
        lRefQy2cm->Add(contQy2cm);

        if(!contQx2cs) {
            AliWarning("OADB object fqxc2s is not available in the file\n");
            return false;
        }
        lRefQx2cs->Add(contQx2cs);

        if(!contQy2cs) {
            AliWarning("OADB object fqyc2s is not available in the file\n");
            return false;
        }
        lRefQy2cs->Add(contQy2cs);

        
        if(!contQx3cm) {
            AliWarning("OADB object fqxc3m is not available in the file\n");
            return false;
        }
        lRefQx3cm->Add(contQx3cm);

        if(!contQy3cm) {
            AliWarning("OADB object fqyc3m is not available in the file\n");
            return false;
        }
        lRefQy3cm->Add(contQy3cm);

        if(!contQx3cs) {
            AliWarning("OADB object fqxc2s is not available in the file\n");
            return false;
        }
        lRefQx3cs->Add(contQx3cs);

        if(!contQy3cs) {
            AliWarning("OADB object fqyc2s is not available in the file\n");
            return false;
        }
        lRefQy3cs->Add(contQy3cs);
    }
    
    
    //load TPC calibrations (not mandatory)
    for(int iCent = 0; iCent < 9; iCent++) {
        AliOADBContainer* contTPCposEta = (AliOADBContainer*) EPCalibRefFile->Get(Form("fphidistr_poseta_%d_%d", iCent*10, (iCent+1)*10));
        if(!contTPCposEta) {
            AliWarning("OADB object fphidistr_poseta (TPC Calibration) is not available in the file\n");
        }
        lRefTPCposEta->Add(contTPCposEta);

        AliOADBContainer* contTPCnegEta = (AliOADBContainer*) EPCalibRefFile->Get(Form("fphidistr_negeta_%d_%d", iCent*10, (iCent+1)*10));
        if(!contTPCnegEta) {
            AliWarning("OADB object fphidistr_negeta (TPC Calibration) is not available in the file\n");
            fWeightsTPCNegEta[iCent] = nullptr;
        }
        lRefTPCnegEta->Add(contTPCnegEta);
    }
    
    return true;
}

bool AliAnalysisTaskEPCalibForJet::OpenInfoCalbration() 
{
    if(!fMultV0BefCorPfpx) {
        AliWarning("OADB object hMultV0BefCorPfpx is not available\n");
        return false;
    }
    
    if(!(fMultV0BefCorPfpx->GetObject(fRun))) {
        AliWarning(Form("OADB object hMultV0BefCorPfpx is not available for run %i\n", fRun));
        return false;
    }
    
    fHistMultV0 = ((TH1D*) fMultV0BefCorPfpx->GetObject(fRun));
    for(int iZvtx = 0; iZvtx < 14; iZvtx++) {
        AliOADBContainer* contQx2am = 0;
        // If we do not have z-vertex differential objects, then only the first index is 
        // in the OADBContainer array
        if(fV0CalibZvtxDiff) contQx2am = (AliOADBContainer* ) fOADBzArray_contQx2am->At(iZvtx);
        else contQx2am = (AliOADBContainer* ) fOADBzArray_contQx2am->At(0);
        if(!contQx2am) {
            AliWarning("OADB object fqxa2m is not available\n");
            return false;
        }
        if(!(contQx2am->GetObject(fRun))) {
            AliWarning(Form("OADB object fqxa2m is not available for run %i\n", fRun));
            return false;
        }
        fQx2mV0A[iZvtx] = ((TH1D*) contQx2am->GetObject(fRun));
        
        AliOADBContainer* contQy2am = 0;
        if(fV0CalibZvtxDiff) contQy2am = (AliOADBContainer* ) fOADBzArray_contQy2am->At(iZvtx);
        else contQy2am = (AliOADBContainer* ) fOADBzArray_contQy2am->At(0);
        if(!contQy2am) {
            AliWarning("OADB object fqya2m is not available\n");
            return false;
        }
        if(!(contQy2am->GetObject(fRun))) {
            AliWarning(Form("OADB object fqya2m is not available for run %i\n", fRun));
            return false;
        }
        fQy2mV0A[iZvtx] = ((TH1D*) contQy2am->GetObject(fRun));
        
        AliOADBContainer* contQx2as = 0;
        if(fV0CalibZvtxDiff) contQx2as = (AliOADBContainer* ) fOADBzArray_contQx2as->At(iZvtx);
        else contQx2as = (AliOADBContainer* ) fOADBzArray_contQx2as->At(0);
        if(!contQx2as) {
            AliWarning("OADB object fqxa2s is not available\n");
            return false;
        }
        if(!(contQx2as->GetObject(fRun))) {
            AliWarning(Form("OADB object fqxa2s is not available for run %i\n", fRun));
            return false;
        }
        fQx2sV0A[iZvtx] = ((TH1D*) contQx2as->GetObject(fRun));
        
        AliOADBContainer* contQy2as = 0;
        if(fV0CalibZvtxDiff) contQy2as = (AliOADBContainer* ) fOADBzArray_contQy2as->At(iZvtx);
        else contQy2as = (AliOADBContainer* ) fOADBzArray_contQy2as->At(0);
        if(!contQy2as) {
            AliWarning("OADB object fqya2s is not available\n");
            return false;
        }
        if(!(contQy2as->GetObject(fRun))) {
            AliWarning(Form("OADB object fqya2s is not available for run %i\n", fRun));
            return false;
        }
        fQy2sV0A[iZvtx] = ((TH1D*) contQy2as->GetObject(fRun));
        
        AliOADBContainer* contQx2cm = 0;
        if(fV0CalibZvtxDiff) contQx2cm = (AliOADBContainer* ) fOADBzArray_contQx2cm->At(iZvtx);
        else contQx2cm = (AliOADBContainer* ) fOADBzArray_contQx2cm->At(0);
        if(!contQx2cm) {
            AliWarning("OADB object fqxc2m is not available\n");
            return false;
        }
        if(!(contQx2cm->GetObject(fRun))) {
            AliWarning(Form("OADB object fqxc2m is not available for run %i\n", fRun));
            return false;
        }
        fQx2mV0C[iZvtx] = ((TH1D*) contQx2cm->GetObject(fRun));
        
        AliOADBContainer* contQy2cm = 0;
        if(fV0CalibZvtxDiff) contQy2cm = (AliOADBContainer* ) fOADBzArray_contQy2cm->At(iZvtx);
        else contQy2cm = (AliOADBContainer* ) fOADBzArray_contQy2cm->At(0);
        if(!contQy2cm) {
            AliWarning("OADB object fqyc2m is not available\n");
            return false;
        }
        if(!(contQy2cm->GetObject(fRun))) {
            AliWarning(Form("OADB object fqyc2m is not available for run %i\n", fRun));
            return false;
        }
        fQy2mV0C[iZvtx] = ((TH1D*) contQy2cm->GetObject(fRun));

        AliOADBContainer* contQx2cs = 0;
        if(fV0CalibZvtxDiff) contQx2cs = (AliOADBContainer* ) fOADBzArray_contQx2cs->At(iZvtx);
        else contQx2cs = (AliOADBContainer* ) fOADBzArray_contQx2cs->At(0);
        if(!contQx2cs) {
            AliWarning("OADB object fqxc%ds is not available\n");
            return false;
        }
        if(!(contQx2cs->GetObject(fRun))) {
            AliWarning(Form("OADB object fqxc2s is not available for run %i\n",  fRun));
            return false;
        }
        fQx2sV0C[iZvtx] = ((TH1D*) contQx2cs->GetObject(fRun));
        
        AliOADBContainer* contQy2cs = nullptr;
        if(fV0CalibZvtxDiff) contQy2cs = (AliOADBContainer* ) fOADBzArray_contQy2cs->At(iZvtx);
        else contQy2cs = (AliOADBContainer* ) fOADBzArray_contQy2cs->At(0);
        if(!contQy2cs) {
            AliWarning("OADB object fqyc%ds is not available\n");
            return false;
        }
        if(!(contQy2cs->GetObject(fRun))) {
            AliWarning(Form("OADB object fqyc2s is not available for run %i\n",  fRun));
            return false;
        }
        fQy2sV0C[iZvtx] = ((TH1D*) contQy2cs->GetObject(fRun));

        // == Q3 Vector
                AliOADBContainer* contQx3am = 0;
        // If we do not have z-vertex differential objects, then only the first index is 
        // in the OADBContainer array
        if(fV0CalibZvtxDiff) contQx3am = (AliOADBContainer* ) fOADBzArray_contQx3am->At(iZvtx);
        else contQx3am = (AliOADBContainer* ) fOADBzArray_contQx3am->At(0);
        if(!contQx3am) {
            AliWarning("OADB object fqxa2m is not available\n");
            return false;
        }
        if(!(contQx3am->GetObject(fRun))) {
            AliWarning(Form("OADB object fqxa2m is not available for run %i\n", fRun));
            return false;
        }
        fQx3mV0A[iZvtx] = ((TH1D*) contQx3am->GetObject(fRun));
        
        AliOADBContainer* contQy3am = 0;
        if(fV0CalibZvtxDiff) contQy3am = (AliOADBContainer* ) fOADBzArray_contQy3am->At(iZvtx);
        else contQy3am = (AliOADBContainer* ) fOADBzArray_contQy3am->At(0);
        if(!contQy3am) {
            AliWarning("OADB object fqya3m is not available\n");
            return false;
        }
        if(!(contQy3am->GetObject(fRun))) {
            AliWarning(Form("OADB object fqya3m is not available for run %i\n", fRun));
            return false;
        }
        fQy3mV0A[iZvtx] = ((TH1D*) contQy3am->GetObject(fRun));
        
        AliOADBContainer* contQx3as = 0;
        if(fV0CalibZvtxDiff) contQx3as = (AliOADBContainer* ) fOADBzArray_contQx3as->At(iZvtx);
        else contQx3as = (AliOADBContainer* ) fOADBzArray_contQx3as->At(0);
        if(!contQx3as) {
            AliWarning("OADB object fqxa2s is not available\n");
            return false;
        }
        if(!(contQx3as->GetObject(fRun))) {
            AliWarning(Form("OADB object fqxa2s is not available for run %i\n", fRun));
            return false;
        }
        fQx3sV0A[iZvtx] = ((TH1D*) contQx3as->GetObject(fRun));
        
        AliOADBContainer* contQy3as = 0;
        if(fV0CalibZvtxDiff) contQy3as = (AliOADBContainer* ) fOADBzArray_contQy3as->At(iZvtx);
        else contQy3as = (AliOADBContainer* ) fOADBzArray_contQy3as->At(0);
        if(!contQy3as) {
            AliWarning("OADB object fqya2s is not available\n");
            return false;
        }
        if(!(contQy3as->GetObject(fRun))) {
            AliWarning(Form("OADB object fqya2s is not available for run %i\n", fRun));
            return false;
        }
        fQy3sV0A[iZvtx] = ((TH1D*) contQy3as->GetObject(fRun));
        
        AliOADBContainer* contQx3cm = 0;
        if(fV0CalibZvtxDiff) contQx3cm = (AliOADBContainer* ) fOADBzArray_contQx3cm->At(iZvtx);
        else contQx3cm = (AliOADBContainer* ) fOADBzArray_contQx3cm->At(0);
        if(!contQx3cm) {
            AliWarning("OADB object fqxc2m is not available\n");
            return false;
        }
        if(!(contQx3cm->GetObject(fRun))) {
            AliWarning(Form("OADB object fqxc2m is not available for run %i\n", fRun));
            return false;
        }
        fQx3mV0C[iZvtx] = ((TH1D*) contQx3cm->GetObject(fRun));
        
        AliOADBContainer* contQy3cm = 0;
        if(fV0CalibZvtxDiff) contQy3cm = (AliOADBContainer* ) fOADBzArray_contQy3cm->At(iZvtx);
        else contQy3cm = (AliOADBContainer* ) fOADBzArray_contQy3cm->At(0);
        if(!contQy3cm) {
            AliWarning("OADB object fqyc2m is not available\n");
            return false;
        }
        if(!(contQy3cm->GetObject(fRun))) {
            AliWarning(Form("OADB object fqyc2m is not available for run %i\n", fRun));
            return false;
        }
        fQy3mV0C[iZvtx] = ((TH1D*) contQy3cm->GetObject(fRun));

        AliOADBContainer* contQx3cs = 0;
        if(fV0CalibZvtxDiff) contQx3cs = (AliOADBContainer* ) fOADBzArray_contQx3cs->At(iZvtx);
        else contQx3cs = (AliOADBContainer* ) fOADBzArray_contQx3cs->At(0);
        if(!contQx3cs) {
            AliWarning("OADB object fqxc%ds is not available\n");
            return false;
        }
        if(!(contQx3cs->GetObject(fRun))) {
            AliWarning(Form("OADB object fqxc2s is not available for run %i\n",  fRun));
            return false;
        }
        fQx3sV0C[iZvtx] = ((TH1D*) contQx3cs->GetObject(fRun));
        
        AliOADBContainer* contQy3cs = nullptr;
        if(fV0CalibZvtxDiff) contQy3cs = (AliOADBContainer* ) fOADBzArray_contQy3cs->At(iZvtx);
        else contQy3cs = (AliOADBContainer* ) fOADBzArray_contQy3cs->At(0);
        if(!contQy3cs) {
            AliWarning("OADB object fqyc%ds is not available\n");
            return false;
        }
        if(!(contQy3cs->GetObject(fRun))) {
            AliWarning(Form("OADB object fqyc2s is not available for run %i\n",  fRun));
            return false;
        }
        fQy3sV0C[iZvtx] = ((TH1D*) contQy3cs->GetObject(fRun));


        if(!fV0CalibZvtxDiff) //assign only first element of array if it is not Zvtx differential
            break;
    }
    
    //load TPC calibrations (not mandatory)
    for(int iCent = 0; iCent < 9; iCent++) {
        AliOADBContainer* contTPCposEta = 0;
        contTPCposEta = (AliOADBContainer* ) fOADBcentArray_contTPCposEta->At(iCent);
        if(!contTPCposEta) {
            AliWarning("OADB object fphidistr_poseta (TPC Calibration) is not available\n");
            fWeightsTPCPosEta[iCent] = nullptr;
        }
        else {
            if(!(contTPCposEta->GetObject(fRun))) {
                AliWarning(Form("OADB object fphidistr_poseta (TPC Calibration) is not available for run %i\n", fRun));
                fWeightsTPCPosEta[iCent] = nullptr;
            }
            else {
                fWeightsTPCPosEta[iCent] = ((TH1D*) contTPCposEta->GetObject(fRun));   
            }
        }

        AliOADBContainer* contTPCnegEta = 0;
        contTPCnegEta = (AliOADBContainer* ) fOADBcentArray_contTPCnegEta->At(iCent);
        if(!contTPCnegEta) {
            AliWarning("OADB object fphidistr_negeta (TPC Calibration) is not available in the file\n");
            fWeightsTPCNegEta[iCent] = nullptr;
            return true;
        }
        else {        
            if(!(contTPCnegEta->GetObject(fRun))) {
                AliWarning(Form("OADB object fphidistr_negeta (TPC Calibration) is not available for run %i\n", fRun));
                fWeightsTPCNegEta[iCent] = nullptr;
            }
            else {
                fWeightsTPCNegEta[iCent] = ((TH1D*) contTPCnegEta->GetObject(fRun));   
            }
        }
    }

    return true;
}










