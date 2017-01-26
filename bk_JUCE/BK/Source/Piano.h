/*
  ==============================================================================

    Piano.h
    Created: 7 Dec 2016 10:25:40am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef Piano_H_INCLUDED
#define Piano_H_INCLUDED

#include "BKUtilities.h"

#include "PreparationMap.h"

#include "Keymap.h"
#include "Synchronic.h"
#include "Nostalgic.h"
#include "Direct.h"
#include "Tuning.h"
#include "General.h"

class Piano : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<Piano>   Ptr;
    typedef Array<Piano::Ptr>                  PtrArr;
    typedef Array<Piano::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Piano>                  Arr;
    typedef OwnedArray<Piano, CriticalSection> CSArr;
    
    Piano(BKSynthesiser *s,
          BKSynthesiser *res,
          BKSynthesiser *ham,
          Keymap::Ptr keymap,
          int Id);
    ~Piano();
    
    inline GeneralSettings::Ptr                getGeneralSettings(void)            {   return general;         }
    
    inline void setGeneralSettings(GeneralSettings::Ptr g)                          {   general = g;            }
    
    ValueTree*  getPianoValueTree(void);
    
    inline void setId(int Id) { Id = Id; }
    
    inline int getId(void) { return Id; }
    
    inline PreparationMap::CSPtrArr getPreparationMaps(void) { return prepMaps; }
    
    PreparationMap::Ptr currentPMap;
    PreparationMap::CSPtrArr activePMaps;
    
    PreparationMap::CSPtrArr           prepMaps;
    
    void prepareToPlay(double sampleRate);
    
private:
    int Id;

    GeneralSettings::Ptr general;
    
    
    
    
    
    
    ValueTree vt;
    
    
    JUCE_LEAK_DETECTOR(Piano)
};


#endif  // Piano_H_INCLUDED
