//
// smart_pointer.h
//

// From Chapter 25 of Professional C++
// by Nicholas A. Solter and Scott J. Kleper
// (Wrox, 2005)
//

#ifndef __SSPointer__
#define __SSPointer__


#include <map>
#include <iostream>

enum Owned {
  NOT_OWNED = 0,
  OWNED = 1
};

template <typename T>

class smart_pointer
{

 public:

  smart_pointer(T* inPtr, bool owned = OWNED);
  smart_pointer();

  ~smart_pointer();
  

  smart_pointer(const smart_pointer<T>& src);

  smart_pointer<T>& operator=(const smart_pointer<T>& rhs);

  template <typename Z>
    operator smart_pointer<Z>();

  const T& operator*() const;

  const T* operator->() const;

  T& operator*();

  T* operator->();

  const T* ptr() const;
  T* ptr();


  operator void*() const { return mPtr; }



 protected:

  T* mPtr;

  static std::map<T*, int> sRefCountMap;
  static std::map<T*, bool> sOwnedMap;



  void finalizePointer();

  void initPointer(T* inPtr);

  void setOwned(T* inPtr, bool owned);

};



template <typename T>
std::map<T*, int> smart_pointer<T>::sRefCountMap;

template <typename T>
std::map<T*, bool> smart_pointer<T>::sOwnedMap;

template <typename T>
smart_pointer<T>::smart_pointer(T* inPtr, bool owned)
{
  initPointer(inPtr);
  setOwned(inPtr, owned);
}

template <typename T>
smart_pointer<T>::smart_pointer()
{
  initPointer(0);
  setOwned(0, NOT_OWNED);
}

template <typename T>
void smart_pointer<T>::setOwned(T* inPtr, bool owned)
{
  sOwnedMap[mPtr] = owned;
}

template <typename T>
smart_pointer<T>::smart_pointer(const smart_pointer<T>& src)
{
  initPointer(src.mPtr);
}

template <typename T>
smart_pointer<T>& 
smart_pointer<T>::operator=(const smart_pointer<T>& rhs)
{
  if (this == &rhs) {
    return (*this);
  }
  finalizePointer();
  initPointer(rhs.mPtr);

  return (*this);
}

template <typename T>
  template <typename Z>
  smart_pointer<T>::operator smart_pointer<Z>()
{
  return smart_pointer<Z>();
}

template <typename T>
smart_pointer<T>::~smart_pointer()
{
  finalizePointer();
}

template<typename T>
void smart_pointer<T>::initPointer(T* inPtr)
{
  mPtr = inPtr;
  if (sRefCountMap.find(mPtr) == sRefCountMap.end()) {  
    sRefCountMap[mPtr] = 1;
  } else {
    sRefCountMap[mPtr]++;
  }
}

template<typename T>
void smart_pointer<T>::finalizePointer()
{
  if (sRefCountMap.find(mPtr) == sRefCountMap.end()) {
    std::cerr << "ERROR: Missing entry in map!" << std::endl;
    return;
  }
  sRefCountMap[mPtr]--;
  if (sRefCountMap[mPtr] == 0) {
    // no No more references to this object -- delete it and remove from map
    sRefCountMap.erase(mPtr);
    if (sOwnedMap[mPtr])
        delete mPtr;
    sOwnedMap.erase(mPtr);
  }
}

template <typename T>
const T* smart_pointer<T>::operator->() const
{
  return (mPtr);
}

template <typename T>
const T* smart_pointer<T>::ptr() const
{
  return (mPtr);
}

template <typename T>
T* smart_pointer<T>::ptr()
{
  return (mPtr);
}

template <typename T>
const T& smart_pointer<T>::operator*() const
{
  return (*mPtr);
}

template <typename T>
T* smart_pointer<T>::operator->() 
{
  return (mPtr);
}

template <typename T>
T& smart_pointer<T>::operator*() 
{
  return (*mPtr);
}


#endif
