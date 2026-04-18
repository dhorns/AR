//--Author	JRM Annand   24th May 2005	
//--Rev         
//--Update      JRM Annand   18th Oct 2005  Debug hbook ntuple output    
//--Description
//                *** Acqu++ <-> Root ***
// Online/Offline Analysis of Sub-Atomic Physics Experimental Data 
//
// TMCUserGenerator
//
// Skeleton User Monte Carlo Kinematics Generator
// Users can redefine behaviour of TMCGenerator class here

#ifndef __TMCUserGenerator_h__
#define __TMCUserGenerator_h__

#include "TMCFoamGenerator.h"

class TMCUserGenerator : public TMCFoamGenerator {
 protected:
 public:
  TMCUserGenerator( const Char_t*, Int_t = 999999 );
  virtual ~TMCUserGenerator();
  virtual void PostInit( );               // init after parameter input
  virtual void SetConfig(Char_t*, Int_t); // parameter input from config file

  ClassDef(TMCUserGenerator,1)
};


#endif
