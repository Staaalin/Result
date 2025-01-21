#include "headers.h"
#include "StDeCorrSPEPDTreeMaker.h"
#include "StEpdGeom.h"
#include "StRoot/StRefMultCorr/StRefMultCorr.h"
#include "StRoot/StRefMultCorr/CentralityMaker.h"

ClassImp(StDeCorrSPEPDTreeMaker)

//_____________________________________________________________________________
StDeCorrSPEPDTreeMaker::StDeCorrSPEPDTreeMaker(const Char_t *name) : StMaker(name), fOutFile(0), mOutFileName(""), mMaxVtxR(2.0), mMaxVtxZ(60.0), mMaxVzDiff(3.0), mMaxTrkEta(1.), mMinNHitsFit(15), mMinNHitsFitRatio(0.52), mMaxDca(3.), mMaxTrkPt(4.0), mMinTrkPt(1.0), mEPDThresh(0.3), mEPDMax(2.0){
	//This block defines variables used in the analyzer	
}
//_____________________________________________________________________________
StDeCorrSPEPDTreeMaker::~StDeCorrSPEPDTreeMaker()
{
	// default destructor
}
//_____________________________________________________________________________
Int_t StDeCorrSPEPDTreeMaker::Init()
{
  //create output file  
  if(!mOutFileName.Length()){
    LOG_ERROR << "StDeCorrSPEPDTreeMaker:: no output file specified for tree and histograms." << endm;
    return kStERR;
  }
  fOutFile = new TFile(mOutFileName.Data(),"recreate");
  LOG_INFO << "StDeCorrSPEPDTreeMaker:: create the output file to store the tree and histograms: " << mOutFileName.Data() << endm;
  
  //create historams and trees, see StDeCorrSPEPDTreeMaker::bookHistos()
  bookHistos();

  //Define trigger informations for the analysis, each trigger has its own ID which is pushed into a vector
  mStPhysics_TriggerIDs.clear();
  mStPhysics_TriggerIDs.push_back(610001); //AuAu 27 MB
  mStPhysics_TriggerIDs.push_back(610011); //AuAu 27 MB
  mStPhysics_TriggerIDs.push_back(610021); //AuAu 27 MB
  mStPhysics_TriggerIDs.push_back(610031); //AuAu 27 MB
    mStPhysics_TriggerIDs.push_back(610041); //AuAu 27 MB
    mStPhysics_TriggerIDs.push_back(610051); //AuAu 27 MB
    mStPhysics_TriggerIDs.push_back(610016); //AuAu 27 MB hlt good
    mStPhysics_TriggerIDs.push_back(610026); //AuAu 27 MB hlt good

  return kStOK;
}
//_____________________________________________________________________________
Int_t StDeCorrSPEPDTreeMaker::InitRun(const Int_t runnumber)
{
	LOG_INFO<<"Grab runnumber: "<<runnumber<<endm;

	return kStOK;
}
//_____________________________________________________________________________
Int_t StDeCorrSPEPDTreeMaker::Finish()
{

  //Write output and close up
	if(fOutFile){
		fOutFile->cd();
		fOutFile->Write();
		fOutFile->Close();
		LOG_INFO << "StDeCorrSPEPDTreeMaker::Finish() -> write out tree in " << mOutFileName.Data() << endm;
	}

	return kStOK;
}
//_____________________________________________________________________________
Int_t StDeCorrSPEPDTreeMaker::Make()
{
  //Init all the makers needed for the analysis, each maker is a pre-defined class
    mPicoDstMaker = (StPicoDstMaker *)GetMaker("picoDst");

    mPicoDst = mPicoDstMaker->picoDst();
    if(!mPicoDst){
        LOG_WARN<<"No PicoDst !"<<endm;
        return kStOK;
    }

	if(mPicoDstMaker){
        if(!processPicoEvent())  return kStOK;
	}

	return kStOK;
}
//_____________________________________________________________________________
Bool_t StDeCorrSPEPDTreeMaker::processPicoEvent()
{
   //Main part for analysis, code in this block runs over every event

  //Get picoDst event    
  StPicoEvent *picoEvent = mPicoDst->event();
  if(!picoEvent){
    LOG_WARN<<"No event level information !"<<endm;
    return kFALSE;
  }
  
  //Select trigger based on ID defined above
  Bool_t validTrigger = kFALSE;
    for(Int_t i=0;i<mStPhysics_TriggerIDs.size();i++){
        if(picoEvent->isTrigger(mStPhysics_TriggerIDs[i])){
            validTrigger = kTRUE;
        }
    }

  if(!validTrigger){
    return kFALSE;
  }
  
  //Fill a histogram for refMult
  hRefMult->Fill(picoEvent->refMult());
  
  //cuts on Vtx
  TVector3 vtxPos   = picoEvent->primaryVertex();
  if(TMath::Abs(vtxPos.x())<1.e-5 && TMath::Abs(vtxPos.y())<1.e-5 && TMath::Abs(vtxPos.z())<1.e-5) return kFALSE;
  if(sqrt(vtxPos.x()*vtxPos.x()+vtxPos.y()*vtxPos.y())>=mMaxVtxR) return kFALSE;
  if(TMath::Abs(vtxPos.z())>=mMaxVtxZ) return kFALSE;
  if(TMath::Abs(vtxPos.z() - picoEvent->vzVpd())>=mMaxVzDiff) return kFALSE;
  
  //now find centrality using StRefMultCorr package  
  TVector3 primVertex(vtxPos.x(),vtxPos.y(),vtxPos.z());
    
    Int_t runid = (int) picoEvent->runId();
    Int_t Refmult   = picoEvent->refMult();
    Int_t Ntofmatch = picoEvent->nBTOFMatch();
    Int_t   myCentrality=-1;

    StRefMultCorr* refmultCorrUtil = CentralityMaker::instance()->getRefMultCorr();
    refmultCorrUtil = CentralityMaker::instance()->getRefMultCorr();
    refmultCorrUtil -> init(runid);
    refmultCorrUtil -> initEvent( (UShort_t)(mPicoDst->event()->refMult()), (Double_t)(mPicoDst->event()->primaryVertex().z()), (Double_t)(mPicoDst->event()->ZDCx()) );

        Bool_t   isBadRun    = refmultCorrUtil->isBadRun(runid);
        if(isBadRun) return kFALSE;
        Bool_t   isPileUpEvt = refmultCorrUtil->passnTofMatchRefmultCut(1.*Refmult, 1.*Ntofmatch);
        if(!isPileUpEvt) return kFALSE;
        Double_t refmultcor  = refmultCorrUtil->getRefMultCorr();
        Double_t reweight    = refmultCorrUtil->getWeight();
        Int_t    cent16      = refmultCorrUtil->getCentralityBin16();
        Int_t    cent9       = refmultCorrUtil->getCentralityBin9();
        myCentrality = 8 - cent9;
    
    if(myCentrality<0) return kFALSE;
    if(myCentrality>8) return kFALSE;
    
    //loop over TPC tracks
    Int_t nNodes = mPicoDst->numberOfTracks();
    for(Int_t i=0;i<nNodes;i++)
    {
        StPicoTrack *pTrack = mPicoDst->track(i);
        if(!pTrack) continue;
    
        //track info
      double pt  = pTrack->pMom().Pt();
      double eta = pTrack->pMom().Eta();
      double phi = pTrack->pMom().Phi();
      double dca = pTrack->gDCA(vtxPos).Mag();
     int charge = pTrack->charge();

        //track cuts
      if(TMath::Abs(eta)>mMaxTrkEta) continue;
        if(pt<mMinTrkPt || pt>mMaxTrkPt) continue;
      if(pTrack->nHitsFit()<mMinNHitsFit) continue;
      if(pTrack->nHitsFit()*1./pTrack->nHitsMax()<mMinNHitsFitRatio) continue;
      if(dca>mMaxDca) continue;
        
        //Fill basic histograms
        hPt->Fill(pt);
        hEta->Fill(eta);
        
    }//end loop track
    
    //loop over EPD
    TClonesArray* EpdHits = mPicoDst->picoArray(StPicoArrays::EpdHit);
    StEpdGeom* mEpdGeom = new StEpdGeom();
    
    for (int ihit=0; ihit<EpdHits->GetEntries(); ihit++)
    {
        //for EPD info and coding, please refer to https://drupal.star.bnl.gov/STAR/blog/lisa/using-epd-event-plane-finder
        int tileId,ring,TT,PP,ADC;
        float nMip;
        
        StPicoEpdHit* epdHit = (StPicoEpdHit*)((*EpdHits)[ihit]);
        tileId = epdHit->id();
        ring = epdHit->row();
        TT = epdHit->tile();
        PP = epdHit->position();
        ADC = epdHit->adc();
        nMip = (TT<10)?(double)ADC/160.0:(double)ADC/115.0;
        
        if (nMip<mEPDThresh) continue;
        double TileWeight = (nMip<mEPDMax)?nMip:mEPDMax;
        TVector3 StraightLine = mEpdGeom->TileCenter(tileId) - primVertex;
        double phi = StraightLine.Phi();
        double eta = StraightLine.Eta();
        
        if(fabs(eta)<2.1 || fabs(eta)>5.1) continue;

        //Fill basic EPD histogram
        hEta_EPD->Fill(eta);
        
    }//end loop EPD
    
  return kTRUE;
}
//_____________________________________________________________________________
void StDeCorrSPEPDTreeMaker::bookHistos()
{
    //Define histograms, you can also define trees etc. here
    hRefMult = new TH1F("hRefMult","",2000,0,2000);
    hPt = new TH1F("hPt","",100,0,10);
    hEta = new TH1F("hEta","",300,-1.5,1.5);
    hEta_EPD = new TH1F("hEta_EPD","",1200,-6,6);

}


