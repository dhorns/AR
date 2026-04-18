//--Author	D.Krambrich   25th August 2004
//--Rev
//--Update                    22nd November 2004
//                              Added fName and some error handling
//--Description
//                *** Acqu++ <-> Root ***
// Online/Offline Analysis of Sub-Atomic Physics Experimental Data 
//
// TA2Array.h
//
// Small (templated) array replacement which allows:
// *) Access to the elements in the usual way using the pointer to the 
//    interal array (i.e. *MyArray or MyArray[])
// *) Access to the elements using the overloaded operator (). MyArray(7)
//    will give access to the 7th element, like MyArray[7] does, but it will
//    in addition check the range to prevent accessing memory not allocated 
//    for the array.
// *) You can use TA2Array for filling multi-hit specra in the following way:
//    -Define 
//       TA2Array<Double_t> fMMiss5Cl; // Missing mass 5-Cluster-Events
//     as a member of your analysis-class.
//    -Initialise it in ::PostInit
//          fMMiss5Cl.Init(30);
//       or  fMMiss5Cl.Init(30, "Missing Mass for 5-Cluster events");
//    -Connect it to the DataManager in ::LoadVariable
//        TA2DataManager::LoadVariable(
//                          "MMiss5Cl",    (Double_t*)fMMiss5Cl, EDMultiX);
//    -Set the end-marker in ::Reconstruct
//         fMMiss5Cl.Null(); 
//    -fMMiss5Cl.Add(the_missing_mass) will add entries and take care for
//     end-marker. 
//     +-----------------------------------------------+
//     | Add returns -1 if it failed! You should care! |
//     +-----------------------------------------------+
//   
// To do: implement a getter for fName.
//

#include <stdlib.h>
#include <iostream>

#ifndef __TA2Array_h__
#define __TA2Array_h__

using namespace std;

template<typename T>
class TA2Array{
private:
  T* pArray;
  UInt_t fSize;
  char fName[255];
public:
  TA2Array();
  TA2Array(UInt_t size, char name[255] = "" );  
  Int_t Init(UInt_t size, char name[255] = "");
  UInt_t GetSize()  { return fSize; }
  Int_t GetNhits();
  inline T& operator()(UInt_t i);
  operator T*() {return pArray;}
  Int_t Clear();
  Int_t Null();
  inline Int_t Add(T val);
  Int_t Dump();
  ~TA2Array();
};

template<typename T> 
TA2Array<T>::TA2Array(){
  fSize  = 0;
  pArray = (T*)NULL;
  strcpy(fName, "");
}

template<typename T> 
TA2Array<T>::TA2Array(UInt_t size, char name[255]) : fSize(size){
    pArray = new T[fSize+1];
    strcpy(fName, name);
}

template<typename T>
Int_t TA2Array<T>::Init(UInt_t size, char name[255]){
  fSize  = size;
  pArray = new T[fSize+1];
  strcpy(fName, name);
  return 0;
}

/* template<typename T> */
/* UInt_t TA2Array<T>::GetSize(){  */
/*   return fSize; */
/* } */

template<typename T>
inline T& TA2Array<T>::operator()(UInt_t i){
    if (i >= fSize+1){                // i < 0 cannot happen, i = 0 will work.
      cerr << "<E> Fatal error!\n TA2Array " << fName 
	   << ": Iterator out of range: i="
	   << i << endl;
      exit(1);
    }
    else 
      return pArray[i];
}

template<typename T>
TA2Array<T>::~TA2Array(){
  if (pArray) delete [] pArray;
}

template<typename T>
Int_t TA2Array<T>::Clear(){
  for(UInt_t i=0; i <= fSize; i++) pArray[i] = (T)0;
  // one could do this here: Init();
  return 0;
}

template<typename T>
Int_t TA2Array<T>::Null(){
  pArray[0]=(T)EBufferEnd;
  return 0;
}

template<typename T>
Int_t TA2Array<T>::GetNhits(){
  Int_t i=0;
  while( i <= (Int_t)fSize ){
    if (pArray[i] == EBufferEnd) return i;
    i++;
  }
  return -1;
}

template<typename T>
inline Int_t TA2Array<T>::Add(T val){
  UInt_t i=0;
  while( (i < fSize) && pArray[i]!=(T)EBufferEnd ) i++;
  if (i==fSize) {
    cerr << "<E> TA2Array " << fName 
	 << ": Iterator out of range. "
	 << "Value won't be added."    << endl;
    return -1;
  }
  else{
    pArray[i]   = val;
    pArray[++i] = EBufferEnd;
  }
  return 0;
}

template<typename T>
Int_t TA2Array<T>::Dump(){
  for(UInt_t i=0; i < fSize; i++)
    cout << " " << i << "\t" << pArray[i] << endl;
  cout << "(" << fSize << "\t" << pArray[fSize] << ")" << endl;
  return 0;
}

#endif
