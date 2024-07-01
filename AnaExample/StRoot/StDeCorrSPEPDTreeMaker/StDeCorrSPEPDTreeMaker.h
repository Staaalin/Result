#ifndef StDeCorrSPEPDTreeMaker_HH
#define StDeCorrSPEPDTreeMaker_HH

/***************************************************************************
 *
 * $Id: StDeCorrSPEPDTreeMaker.h 2015/04/09  Exp $ 
 * StDeCorrSPEPDTreeMaker - class to produce miniTree for mtd related analysis
 * Author: Shuai Yang
 *--------------------------------------------------------------------------
 *
 ***************************************************************************/

#include "StMaker.h"
#include "StTreeStructure.h"

#include "StThreeVectorF.hh"
#include "TLorentzVector.h"

#include "StMtdUtil/StMtdConstants.h"

#include <vector>
#include <map>
#ifndef ST_NO_NAMESPACES
using std::vector;
#endif

//define all classes used in analysis, some of these are not needed but no harm to always keep them
class TH1F;
class TH2F;
class TH2D;
class TH2F;
class TF1;
class TString;
class TTree;
class TFile;

class StMuDstMaker;
class StMuDst;
class StEmcCollection;
class StEmcPosition;
class StEmcGeom;
class StEmcRawHit;
class StMuTrack;
class StMuMtdHit;

class StFmsDbMaker;

class StPicoDstMaker;
class StPicoDst;
class StPicoTrack;
class StPicoMtdHit;

class StRefMultCorr;
class CentralityMaker;

#if !defined(ST_NO_TEMPLATE_DEF_ARGS) || defined(__CINT__)
typedef vector<Int_t> IntVec;
typedef vector<Double_t> DoubleVec;
typedef vector<TLorentzVector> LorentzVec;
#else
typedef vector<Int_t, allocator<Int_t>> IntVec;
typedef vector<Double_t, allocator<Double_t>> DoubleVec;
typedef vector<TLorentzVector, allocator<TLorentzVector>> LorentzVec;
#endif

//define structure for the analyzer
class StDeCorrSPEPDTreeMaker : public StMaker {
	public:
		StDeCorrSPEPDTreeMaker(const Char_t *name = "StDeCorrSPEPDTreeMaker");
		~StDeCorrSPEPDTreeMaker();

		Int_t    Init();
		Int_t    InitRun(const Int_t runNumber);
		Int_t    Make();
		Int_t    Finish();

		void     setMaxVtxR(const Double_t max);
		void     setMaxVtxZ(const Double_t max);
		void     setMaxVzDiff(const Double_t max);
		void     setMinTrackPt(const Double_t min);
        void     setMaxTrackPt(const Double_t max);
		void     setMaxTrackEta(const Double_t max);
		void     setMinNHitsFit(const Int_t min);
		void     setMinNHitsFitRatio(const Double_t min);
		void     setMaxDca(const Double_t max);
		void     setOutFileName(const TString name);
        void     setMipThreshold(const Double_t min);
        void     setMaxTileWeight(const Double_t max);

	protected:
		void     bookHistos();
		Bool_t   processPicoEvent();

	private:
		StPicoDstMaker *mPicoDstMaker;
		StPicoDst      *mPicoDst;
        StFmsDbMaker *fmsDbMaker;

		TFile          *fOutFile;            // Output file
		TString        mOutFileName;         // Name of the output file

        Double_t       mMaxVtxR;             // Maximum vertex r
        Double_t       mMaxVtxZ;             // Maximum vertex z
        Double_t       mMaxVzDiff;           // Maximum VpdVz-TpcVz
        Double_t       mMaxTrkEta;           // Maximum track eta
        Int_t          mMinNHitsFit;         // Minimum number of hits used for track fit
        Double_t       mMinNHitsFitRatio;    // Minimum ratio of hits used for track fit
        Double_t       mMaxDca;              // Maximum track dca
    Double_t       mMaxTrkPt;
    Double_t       mMinTrkPt;
    Double_t       mEPDThresh;
    Double_t       mEPDMax;

		IntVec         mStPhysics_TriggerIDs;
    
		//define histograms
    TH1F* hRefMult;
    TH1F* hPt;
    TH1F* hEta;
    TH1F* hEta_EPD;
    
		ClassDef(StDeCorrSPEPDTreeMaker, 1)
};

//define private functions
inline void StDeCorrSPEPDTreeMaker::setMaxVtxR(const Double_t max) { mMaxVtxR = max; }
inline void StDeCorrSPEPDTreeMaker::setMaxVtxZ(const Double_t max) { mMaxVtxZ = max; }
inline void StDeCorrSPEPDTreeMaker::setMaxVzDiff(const Double_t max) { mMaxVzDiff = max; }
inline void StDeCorrSPEPDTreeMaker::setMinTrackPt(const Double_t min){ mMinTrkPt = min;}
inline void StDeCorrSPEPDTreeMaker::setMaxTrackPt(const Double_t max){ mMaxTrkPt = max;}
inline void StDeCorrSPEPDTreeMaker::setMaxTrackEta(const Double_t max){ mMaxTrkEta = max; }
inline void StDeCorrSPEPDTreeMaker::setMinNHitsFit(const Int_t min) { mMinNHitsFit = min; }
inline void StDeCorrSPEPDTreeMaker::setMinNHitsFitRatio(const Double_t min) { mMinNHitsFitRatio = min; }
inline void StDeCorrSPEPDTreeMaker::setMaxDca(const Double_t max) { mMaxDca = max; }
inline void StDeCorrSPEPDTreeMaker::setOutFileName(const TString name) { mOutFileName = name; }
inline void StDeCorrSPEPDTreeMaker::setMipThreshold(const Double_t min) { mEPDThresh = min; }
inline void StDeCorrSPEPDTreeMaker::setMaxTileWeight(const Double_t max) { mEPDMax = max; }

#endif


