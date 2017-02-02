/*
  ==============================================================================

    Synchronic.h
    Created: 22 Nov 2016 3:46:35pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef SYNCHRONIC_H_INCLUDED
#define SYNCHRONIC_H_INCLUDED

#include "BKUtilities.h"

#include "BKSynthesiser.h"

#include "Tuning.h"

class SynchronicPreparation : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<SynchronicPreparation>   Ptr;
    typedef Array<SynchronicPreparation::Ptr>                  PtrArr;
    typedef Array<SynchronicPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<SynchronicPreparation>                  Arr;
    typedef OwnedArray<SynchronicPreparation, CriticalSection> CSArr;
    
    // Copy Constructor
    SynchronicPreparation(SynchronicPreparation::Ptr p):
    Id(p->getId()),
    sTempo(p->getTempo()),
    sNumBeats(p->getNumBeats()),
    sClusterMin(p->getClusterMin()),
    sClusterMax(p->getClusterMax()),
    sClusterCap(p->getClusterCap()),
    sMode(p->getMode()),
    sBeatsToSkip(p->getBeatsToSkip()),
    sBeatMultipliers(p->getBeatMultipliers()),
    sAccentMultipliers(p->getAccentMultipliers()),
    sLengthMultipliers(p->getLengthMultipliers()),
    sTranspOffsets(p->getTranspOffsets()),
    sBeatThreshSec(p->getBeatThresh()),
    sClusterThresh(p->getClusterThreshMS()),
    sClusterThreshSec(p->getClusterThreshSEC()),
    at1History(p->getAdaptiveTempo1History()),
    at1Min(p->getAdaptiveTempo1Min()),
    at1Max(p->getAdaptiveTempo1Max()),
    at1Subdivisions(p->getAdaptiveTempo1Subdivisions()),
    at1Mode(p->getAdaptiveTempo1Mode()),
    tuning(p->getTuning())
    {
    }
    
    SynchronicPreparation(int Id,
                          float tempo,
                          int numBeats,
                          int clusterMin,
                          int clusterMax,
                          float clusterThresh,
                          SynchronicSyncMode mode,
                          int beatsToSkip,
                          Array<float> beatMultipliers,
                          Array<float> accentMultipliers,
                          Array<float> lengthMultipliers,
                          Array<float> transpOffsets,
                          TuningPreparation::Ptr t):
    Id(Id),
    sTempo(tempo),
    sNumBeats(numBeats),
    sClusterMin(clusterMin),
    sClusterMax(clusterMax),
    sMode(mode),
    sBeatsToSkip(beatsToSkip),
    sBeatMultipliers(beatMultipliers),
    sAccentMultipliers(accentMultipliers),
    sLengthMultipliers(lengthMultipliers),
    sTranspOffsets(transpOffsets),
    sBeatThreshSec(60.0/sTempo),
    sClusterThresh(clusterThresh),
    sClusterThreshSec(.001 * sClusterThresh),
    tuning(t)
    {
    }

    
    SynchronicPreparation(int Id, TuningPreparation::Ptr t):
    Id(Id),
    sTempo(120),
    sNumBeats(0),
    sClusterMin(1),
    sClusterMax(100),
    sClusterCap(8),
    sMode(FirstNoteOnSync),
    sBeatsToSkip(0),
    sBeatMultipliers(Array<float>({1.0})),
    sAccentMultipliers(Array<float>({1.0})),
    sLengthMultipliers(Array<float>({1.0})),
    sTranspOffsets(Array<float>({0.0})),
    sBeatThreshSec(60.0/sTempo),
    sClusterThresh(500),
    sClusterThreshSec(.001 * sClusterThresh),
    at1History(1),
    at1Min(100),
    at1Max(2000),
    at1Subdivisions(4.0f),
    at1Mode(TimeBetweenNotes),
    tuning(t)
    {
    }
    
    inline const float getTempo() const noexcept                       {return sTempo;                 }
    inline const int getNumBeats() const noexcept                      {return sNumBeats;             }
    inline const int getClusterMin() const noexcept                    {return sClusterMin;            }
    inline const int getClusterMax() const noexcept                    {return sClusterMax;            }
    inline const int getClusterCap() const noexcept                    {return sClusterCap;            }
    inline const float getClusterThreshSEC() const noexcept            {return sClusterThreshSec;      }
    inline const float getBeatThresh() const noexcept                  {return sBeatThreshSec;        }
    inline const float getClusterThreshMS() const noexcept             {return sClusterThresh;         }
    inline const SynchronicSyncMode getMode() const noexcept           {return sMode;                  }
    inline const Array<float> getBeatMultipliers() const noexcept      {return sBeatMultipliers;       }
    inline const int getBeatsToSkip()                                  {return sBeatsToSkip;           }
    inline const Array<float> getAccentMultipliers() const noexcept    {return sAccentMultipliers;     }
    inline const Array<float> getLengthMultipliers() const noexcept    {return sLengthMultipliers;     }
    inline const Array<float> getTranspOffsets() const noexcept        {return sTranspOffsets;         }
    
    //Adaptive Tempo 1
    inline AdaptiveTempo1Mode getAdaptiveTempo1Mode(void)   {return at1Mode;   }
    inline int getAdaptiveTempo1History(void)               {return at1History;}
    inline float getAdaptiveTempo1Subdivisions(void)        {return at1Subdivisions;}
    inline float getAdaptiveTempo1Min(void)                 {return at1Min;}
    inline float getAdaptiveTempo1Max(void)                 {return at1Max;}
    
    inline int getId(void) {   return Id; }
    
    inline void setTempo(float tempo)
    {
        sTempo = tempo;
        float tempoPeriodS = (60.0/sTempo);
        sBeatThreshSec = tempoPeriodS;
    }
    
    inline void setClusterThresh(float clusterThresh)
    {
        sClusterThresh = clusterThresh;
        sClusterThreshSec = sClusterThresh * .001;
    }
    
    inline void setNumBeats(int numBeats)                              {sNumBeats = numBeats;                            }
    inline void setClusterMin(int clusterMin)                          {sClusterMin = clusterMin;                          }
    inline void setClusterMax(int clusterMax)                          {sClusterMax = clusterMax;                          }
    inline void setClusterCap(int clusterCap)                          {sClusterCap = clusterCap;                          }
    inline void setMode(SynchronicSyncMode mode)                       {sMode = mode;                                      }
    inline void setBeatsToSkip(int beatsToSkip)                        {sBeatsToSkip = beatsToSkip;                        }
    inline void setBeatMultipliers(Array<float> beatMultipliers)       {sBeatMultipliers.swapWith(beatMultipliers);        }
    inline void setAccentMultipliers(Array<float> accentMultipliers)   {sAccentMultipliers.swapWith(accentMultipliers);    }
    inline void setTranspOffsets(Array<float> transpOffsets)           {sTranspOffsets.swapWith(transpOffsets);            }
    inline void setLengthMultipliers(Array<float> lengthMultipliers)   {sLengthMultipliers.swapWith(lengthMultipliers);    }
    
    //Adaptive Tempo 1
    inline void setAdaptiveTempo1Mode(AdaptiveTempo1Mode mode)          {at1Mode = mode;    }
    inline void setAdaptiveTempo1History(int hist)      {at1History = hist;}
    inline void setAdaptiveTempo1Subdivisions(float sub)      {at1Subdivisions = sub;}
    inline void setAdaptiveTempo1Min(float min)      {at1Min = min;}
    inline void setAdaptiveTempo1Max(float max)      {at1Max = max;}
    
    inline const TuningPreparation::Ptr getTuning() const noexcept      {return tuning; }
    inline void setTuning(TuningPreparation::Ptr t)                       {tuning = t;  }
    
    void print(void)
    {
        DBG("| - - - Synchronic Preparation - - - |");
        DBG("sTempo: " + String(sTempo));
        DBG("sNumBeats: " + String(sNumBeats));
        DBG("sClusterMin: " + String(sClusterMin));
        DBG("sClusterMax: " + String(sClusterMax));
        DBG("sClusterCap: " + String(sClusterCap));
        DBG("sClusterThresh: " + String(sClusterThresh));
        DBG("sMode: " + String(sMode));
        DBG("sBeatsToSkip: " + String(sBeatsToSkip));
        DBG("sBeatMultipliers: " + floatArrayToString(sBeatMultipliers));
        DBG("sLengthMultipliers: " + floatArrayToString(sLengthMultipliers));
        DBG("sAccentMultipliers: " + floatArrayToString(sAccentMultipliers));
        DBG("sTranspOffsets: " + floatArrayToString(sTranspOffsets));
        DBG("sBeatThreshSec: " + String(sBeatThreshSec));
        DBG("sClusterThreshSec: " + String(sClusterThreshSec));
        DBG("| - - - - - - - - -- - - - - - - - - |");
    }
    
private:
    int Id;
    float sTempo;
    int sNumBeats,sClusterMin,sClusterMax;
    int sClusterCap = 8; //max in cluster; 8 in original bK
    
    SynchronicSyncMode sMode;
    int sBeatsToSkip;
    
    Array<float> sBeatMultipliers;      //multiply pulse lengths by these
    Array<float> sAccentMultipliers;    //multiply velocities by these
    Array<float> sLengthMultipliers;    //multiply note duration by these
    Array<float> sTranspOffsets;        //transpose by these

    float sBeatThreshSec;      //length of time between pulses, as set by temp
    float sClusterThresh;       //max time between played notes before new cluster is started, in MS
    float sClusterThreshSec;
    
    // Adaptive Tempo 1
    int at1History;
    float at1Min, at1Max;
    float at1Subdivisions;
    AdaptiveTempo1Mode at1Mode;
    
    TuningPreparation::Ptr tuning;
    
    JUCE_LEAK_DETECTOR(SynchronicPreparation);
};

class SynchronicProcessor  : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<SynchronicProcessor>   Ptr;
    typedef Array<SynchronicProcessor::Ptr>                  PtrArr;
    typedef Array<SynchronicProcessor::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<SynchronicProcessor>                  Arr;
    typedef OwnedArray<SynchronicProcessor,CriticalSection>  CSArr;
    
    
    SynchronicProcessor(BKSynthesiser *synth,
                        SynchronicPreparation::Ptr prep,
                        SynchronicPreparation::Ptr active,
                        int Id);
    
    ~SynchronicProcessor();
    
    void         setCurrentPlaybackSampleRate(double sr);
    inline const uint64 getCurrentNumSamplesBeat(void) const noexcept   { return numSamplesBeat;    }
    inline const uint64 getCurrentPhasor(void) const noexcept           { return phasor;            }
    
    void processBlock(int numSamples, int channel);
    void keyPressed(int noteNumber, float velocity);
    void keyReleased(int noteNumber, int channel);
    float getTimeToBeatMS(float beatsToSkip);
    
    inline void                             setPreparation(SynchronicPreparation::Ptr prep) { preparation = prep;           }
    inline SynchronicPreparation::Ptr       getPreparation(void)                            { return preparation;           }
    inline int                              getPreparationId(void)                          { return preparation->getId();  }
    
    inline int getId(void){return Id;}
    
private:
    int Id;
    BKSynthesiser* synth;
    SynchronicPreparation::Ptr preparation, active;
    
    double sampleRate;
    
    TuningProcessor tuner;
    Array<float> tuningOffsets;
    PitchClass tuningBasePitch;

    int beatCounter;  //beat (or pulse) counter; max set by users -- sNumBeats
    
    //parameter field counters
    int beatMultiplierCounter;   //beat length (time between beats) multipliers
    int accentMultiplierCounter; //accent multipliers
    int lengthMultiplierCounter; //note length (sounding length) multipliers (multiples of 50ms, at least for now)
    int transpOffsetCounter;    //transposition offsets
    
    //reset the phase, including of all the parameter fields
    void resetPhase(int skipBeats);
    
    void playNote(int channel, int note, float velocity);
    Array<float> velocities;    //record of velocities
    Array<int> keysDepressed;   //current keys that are depressed
    
    bool inCluster;
    uint64 clusterThresholdSamples;
    uint64 clusterThresholdTimer;
    uint64 clusterTimer;
    Array<int> cluster;         //cluster of notes played, with repetitions, limited to totalClusters (8?)
    Array<int> slimCluster;     //cluster without repetitions
    
    uint64 phasor;
    uint64 numSamplesBeat;          // = beatThresholdSamples * beatMultiplier
    uint64 beatThresholdSamples;    // # samples in a beat, as set by tempo

    bool shouldPlay;

    
    JUCE_LEAK_DETECTOR(SynchronicProcessor);
};




#endif  // SYNCHRONIC_H_INCLUDED
