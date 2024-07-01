#include <TSystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

void load(){
  //Load all the System libraries
  gSystem->Load("StarRoot");
  gSystem->Load("St_base");
  gSystem->Load("StarClassLibrary"); //problem with vector<double>
  gSystem->Load("StUtilities");

  gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
  loadSharedLibraries();
  
  gSystem->Load("StTpcDb");
  gSystem->Load("StEvent");
  gSystem->Load("StMcEvent");
  gSystem->Load("StMcEventMaker");
  gSystem->Load("StDaqLib");
  gSystem->Load("libgen_Tables");
  gSystem->Load("libsim_Tables");
  gSystem->Load("libglobal_Tables");
  gSystem->Load("StMagF");
  
  gSystem->Load("St_g2t.so");
  gSystem->Load("St_geant_Maker.so");
  gSystem->Load("StAssociationMaker");
  gSystem->Load("StMcAnalysisMaker");
  gSystem->Load("libgeometry_Tables");   
  gSystem->Load("StTriggerUtilities");



  gSystem->Load("StEmcUtil");
  gSystem->Load("StEmcRawMaker");
  gSystem->Load("StEmcADCtoEMaker");
  gSystem->Load("StPreEclMaker");
  gSystem->Load("StEpcMaker");
  gSystem->Load("StEmcSimulatorMaker");
  
  gSystem->Load("StDbLib");
  gSystem->Load("StDbUtilities");
  gSystem->Load("StDbBroker");
  gSystem->Load("StDetectorDbMaker");
  gSystem->Load("St_db_Maker");
  
  gSystem->Load("StMtdHitMaker");
  gSystem->Load("StMtdUtil");
  gSystem->Load("StMtdMatchMaker");
  gSystem->Load("StMtdCalibMaker");
  gSystem->Load("StBTofUtil");
  gSystem->Load("StVpdCalibMaker");

  gSystem->Load("StPicoEvent");
  gSystem->Load("StPicoDstMaker");
  gSystem->Load("StRefMultCorr");

  gSystem->Load("StDeCorrSPEPDTreeMaker");

  gSystem->Load("StEEmcUtil");
  gSystem->Load("StEEmcDbMaker");
}

void doEvent_test(Int_t nEvents=-1, const Char_t *inputFile="root://xrdstar.rcf.bnl.gov:1095//home/starlib/home/starreco/reco/27GeV_production_2018/ReversedFullField/P19ib/2018/131/19131005/st_physics_19131005_raw_4500003.picoDst.root", const TString outputFile="test.root", const Bool_t debug = kFALSE)
{
	load();

	StChain *chain = new StChain();
	chain->SetDebug(0);
	StMuDebug::setLevel(0); // switch of some debug output

	StMuTimer timer;
	timer.start();

	Bool_t iMuDst = 0;

	ifstream infile;
	infile.open(inputFile);
	string name;
	getline(infile,name);
	infile.close();
	
	std::size_t found = name.find("MuDst.root");
	if(found!=std::string::npos)
		iMuDst = 1;

	if(iMuDst){
		char theFilter[80];
		sprintf(theFilter,".MuDst.root:MuDst.root");
		StMuDstMaker *microMaker = new StMuDstMaker(0,0,"",inputFile2,theFilter,1000);
		microMaker->Init();
		microMaker->SetStatus("*",1);
	}else{
        StPicoDstMaker *picoMaker = new StPicoDstMaker(StPicoDstMaker::IoRead,inputFile,"picoDst"); 
	}

	StDeCorrSPEPDTreeMaker *miniTreeMaker = new StDeCorrSPEPDTreeMaker();
	miniTreeMaker->setOutFileName(outputFile);

	if(chain->Init()==kStERR) return;
	cout<<"chain->Init();"<<endl;

	if(nEvents<0){
		if(iMuDst)
			nEvents = microMaker->chain()->GetEntries();
		else 
			nEvents = picoMaker->chain()->GetEntries();
	}

	cout << "****************************************** " << endl;
	cout << "total number of events  " << nEvents << endl;
	cout << "****************************************** " << endl;

	for(Int_t i=0; i<nEvents; i++) {
		if(debug) {
			cout<<endl;
			cout<<"Working on eventNumber "<< i <<endl;
		} else {
			if(i%1000==0)
				cout << "Working on eventNumber " << i << endl;
		}

		chain->Clear();
		int iret = chain->Make(i);

		if(iret) { cout << "Bad return code!" << iret << endl; break;}
	}

	chain->Finish();
	delete chain;

	timer.stop();
	cout << "Total time = " << timer.elapsedTime() << " s" << endl;

	cout << "****************************************** " << endl;
	cout << "Work done... now its time to close up shop!"<< endl;
	cout << "****************************************** " << endl;
}

