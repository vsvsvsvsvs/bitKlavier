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
#include "Tempo.h"
#include "General.h"
#include "Keymap.h"

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
    sNumBeats(p->getNumBeats()),
    sClusterMin(p->getClusterMin()),
    sClusterMax(p->getClusterMax()),
    sClusterCap(p->getClusterCap()),
    sMode(p->getMode()),
    sBeatsToSkip(p->getBeatsToSkip()),
    sBeatMultipliers(p->getBeatMultipliers()),
    sAccentMultipliers(p->getAccentMultipliers()),
    sLengthMultipliers(p->getLengthMultipliers()),
    sTransposition(p->getTransposition()),
    sGain(p->getGain()),
    sClusterThresh(p->getClusterThreshMS()),
    sClusterThreshSec(p->getClusterThreshSEC()),
    sReleaseVelocitySetsSynchronic(p->getReleaseVelocitySetsSynchronic())
    {
        
    }
    
                        
    SynchronicPreparation(int numBeats,
                          int clusterMin,
                          int clusterMax,
                          float clusterThresh,
                          SynchronicSyncMode mode,
                          bool velocityMode,
                          int beatsToSkip,
                          Array<float> beatMultipliers,
                          Array<float> accentMultipliers,
                          Array<float> lengthMultipliers,
                          Array<Array<float>> transp,
                          Tuning::Ptr tun,
                          Tempo::Ptr tmp):
    sNumBeats(numBeats),
    sClusterMin(clusterMin),
    sClusterMax(clusterMax),
    sMode(mode),
    sReleaseVelocitySetsSynchronic(velocityMode),
    sBeatsToSkip(beatsToSkip),
    sBeatMultipliers(beatMultipliers),
    sAccentMultipliers(accentMultipliers),
    sLengthMultipliers(lengthMultipliers),
    sTransposition(transp),
    sClusterThresh(clusterThresh),
    sClusterThreshSec(.001 * sClusterThresh)
    {
    }

    
    SynchronicPreparation(Tuning::Ptr tun, Tempo::Ptr tmp):
    sNumBeats(0),
    sClusterMin(1),
    sClusterMax(100),
    sClusterCap(8), //8 in original bK, but behavior is different here?
    sMode(FirstNoteOnSync),
    sBeatsToSkip(0),
    sBeatMultipliers(Array<float>({1.0})),
    sAccentMultipliers(Array<float>({1.0})),
    sLengthMultipliers(Array<float>({1.0})),
    sGain(1.0),
    sClusterThresh(500),
    sClusterThreshSec(.001 * sClusterThresh)
    {
        sTransposition.ensureStorageAllocated(1);
        sTransposition.add(Array<float>({0.0}));
    }
    
    inline void copy(SynchronicPreparation::Ptr s)
    {
        sNumBeats = s->getNumBeats();
        sClusterMin = s->getClusterMin();
        sClusterMax = s->getClusterMax();
        sClusterCap = s->getClusterCap();
        sMode = s->getMode();
        sBeatsToSkip = s->getBeatsToSkip();
        sBeatMultipliers = s->getBeatMultipliers();
        sAccentMultipliers = s->getAccentMultipliers();
        sLengthMultipliers = s->getLengthMultipliers();
        sGain = s->getGain();
        sTransposition = s->getTransposition();
        sClusterThresh = s->getClusterThreshMS();
        sClusterThreshSec = s->getClusterThreshSEC();
        sReleaseVelocitySetsSynchronic = s->getReleaseVelocitySetsSynchronic();
    }
    
    bool compare(SynchronicPreparation::Ptr s)
    {
        bool lens = true;
        bool accents = true;
        bool beats = true;
        bool transp = true;
        
        for (int i = s->getLengthMultipliers().size(); --i>=0;)
        {
            if (s->getLengthMultipliers()[i] != sLengthMultipliers[i])
            {
                lens = false;
                break;
                
            }
        }
        
        for (int i = s->getAccentMultipliers().size(); --i>=0;)
        {
            if (s->getAccentMultipliers()[i] != sAccentMultipliers[i])
            {
                accents = false;
                break;
                
            }
        }
        
        for (int i = s->getBeatMultipliers().size(); --i>=0;)
        {
            if (s->getBeatMultipliers()[i] != sBeatMultipliers[i])
            {
                beats = false;
                break;
                
            }
        }
        
        for (int i  = s->getTransposition().size(); --i >= 0;)
        {
            Array<float> transposition = s->getTransposition()[i];
            for (int j = transposition.size(); --j >= 0;)
            {
                if (transposition[j] != sTransposition[i][j])
                {
                    transp = false;
                    break;
                    
                }
            }
        }
        
        return (sNumBeats == s->getNumBeats() &&
                sClusterMin == s->getClusterMin() &&
                sClusterMax == s->getClusterMax() &&
                sClusterCap == s->getClusterCap() &&
                (sMode == s->getMode()) &&
                transp && lens && accents && beats &&
                sGain == s->getGain() &&
                sClusterThresh == s->getClusterThreshMS() &&
                sClusterThreshSec == s->getClusterThreshSEC() &&
                sReleaseVelocitySetsSynchronic == s->getReleaseVelocitySetsSynchronic());
    }
    
    //inline const float getTempo() const noexcept                       {return sTempo;               }
    inline const int getNumBeats() const noexcept                      {return sNumBeats;              }
    inline const int getClusterMin() const noexcept                    {return sClusterMin;            }
    inline const int getClusterMax() const noexcept                    {return sClusterMax;            }
    inline const int getClusterCap() const noexcept                    {return sClusterCap;            }
    inline const float getClusterThreshSEC() const noexcept            {return sClusterThreshSec;      }
    inline const float getClusterThreshMS() const noexcept             {return sClusterThresh;         }
    inline const SynchronicSyncMode getMode() const noexcept           {return sMode;                  }
    inline const Array<float> getBeatMultipliers() const noexcept      {return sBeatMultipliers;       }
    inline const int getBeatsToSkip() const noexcept                   {return sBeatsToSkip;           }
    inline const int getOffsetParamToggle() const noexcept
    {
        if(getMode() == FirstNoteOnSync || getMode() == AnyNoteOnSync) return getBeatsToSkip() + 1;
        else return getBeatsToSkip();
    }
    
    inline const Array<float> getAccentMultipliers() const noexcept    {return sAccentMultipliers;     }
    inline const Array<float> getLengthMultipliers() const noexcept    {return sLengthMultipliers;     }
    inline const Array<Array<float>> getTransposition() const noexcept {return sTransposition;         }
    inline const bool getReleaseVelocitySetsSynchronic() const noexcept{return sReleaseVelocitySetsSynchronic; }
    inline const float getGain() const noexcept                        {return sGain;                   }
    

    inline void setClusterThresh(float clusterThresh)
    {
        sClusterThresh = clusterThresh;
        sClusterThreshSec = sClusterThresh * .001;
    }
    
    
    inline const String getName() const noexcept {return name;}
    inline void setName(String n){name = n;}
    
    inline void setNumBeats(int numBeats)                              {sNumBeats = numBeats;                              }
    inline void setClusterMin(int clusterMin)                          {sClusterMin = clusterMin;                          }
    inline void setClusterMax(int clusterMax)                          {sClusterMax = clusterMax;                          }
    inline void setClusterCap(int clusterCap)                          {sClusterCap = clusterCap;                          }
    inline void setMode(SynchronicSyncMode mode)                       {sMode = mode;                                      }
    inline void setBeatsToSkip(int beatsToSkip)                        {sBeatsToSkip = beatsToSkip;                        }
    inline void setBeatMultipliers(Array<float> beatMultipliers)       {sBeatMultipliers.swapWith(beatMultipliers);        }
    inline void setAccentMultipliers(Array<float> accentMultipliers)   {sAccentMultipliers.swapWith(accentMultipliers);    }
    inline void setTransposition(Array<Array<float>> transp)           {sTransposition.swapWith(transp);                   }
    inline void setLengthMultipliers(Array<float> lengthMultipliers)   {sLengthMultipliers.swapWith(lengthMultipliers);    }
    
    inline void setBeatMultiplier(int whichSlider, float value)        {sBeatMultipliers.set(whichSlider, value);           }
    inline void setAccentMultiplier(int whichSlider, float value)      {sAccentMultipliers.set(whichSlider, value);         }
    inline void setLengthMultiplier(int whichSlider, float value)      {sLengthMultipliers.set(whichSlider, value);         }
    inline void setSingleTransposition(int whichSlider, Array<float> values) {sTransposition.set(whichSlider, values); }
    inline void setReleaseVelocitySetsSynchronic(bool rvss)            {sReleaseVelocitySetsSynchronic = rvss;          }
    inline void setGain(float gain)                                    {sGain = gain;                          }
    
    void print(void)
    {
        DBG("| - - - Synchronic Preparation - - - |");
        DBG("sTempo: " + String(sTempo));
        DBG("sNumBeats: " + String(sNumBeats));
        DBG("sClusterMin: " + String(sClusterMin));
        DBG("sClusterMax: " + String(sClusterMax));
        DBG("sClusterCap: " + String(sClusterCap));
        DBG("sClusterThresh: " + String(sClusterThresh));
        DBG("sReleaseVelocitySetsSynchronic: " + String(sReleaseVelocitySetsSynchronic));
        DBG("sMode: " + String(sMode));
        DBG("sBeatsToSkip: " + String(sBeatsToSkip));
        DBG("sBeatMultipliers: " + floatArrayToString(sBeatMultipliers));
        DBG("sLengthMultipliers: " + floatArrayToString(sLengthMultipliers));
        DBG("sAccentMultipliers: " + floatArrayToString(sAccentMultipliers));
        
        String s = "";
        for (auto arr : sTransposition) s += "{ " + floatArrayToString(arr) + " },\n";
        DBG("sTransposition: " + s);
        
        DBG("sClusterThreshSec: " + String(sClusterThreshSec));
        //DBG("resetKeymap: " + intArrayToString(getResetMap()->keys()));
        DBG("| - - - - - - - - -- - - - - - - - - |");
    }
    
private:
    String name;
    float sTempo;
    int sNumBeats,sClusterMin,sClusterMax;
    int sClusterCap = 8; //max in cluster; 8 in original bK. pulseDepth?
    
    SynchronicSyncMode sMode;
    int sBeatsToSkip;
    
    Array<float> sBeatMultipliers;      //multiply pulse lengths by these
    Array<float> sAccentMultipliers;    //multiply velocities by these
    Array<float> sLengthMultipliers;    //multiply note duration by these
    Array<Array<float>> sTransposition;        //transpose by these

    float sGain;                //gain multiplier
    float sClusterThresh;      //max time between played notes before new cluster is started, in MS
    float sClusterThreshSec;
    
    bool sReleaseVelocitySetsSynchronic;

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
    
    
    SynchronicProcessor(BKSynthesiser* main,
                        SynchronicPreparation::Ptr active,
                        Tuning::Ptr tuning,
                        Tempo::Ptr tempo,
                        GeneralSettings::Ptr general,
                        int Id);
    
    ~SynchronicProcessor();
    
    void         setCurrentPlaybackSampleRate(double sr);
    inline const uint64 getCurrentNumSamplesBeat(void) const noexcept   { return numSamplesBeat;    }
    inline const uint64 getCurrentPhasor(void) const noexcept           { return phasor;            }
    
    void processBlock(int numSamples, int channel);
    void keyPressed(int noteNumber, float velocity);
    void keyReleased(int noteNumber, float velocity, int channel);
    float getTimeToBeatMS(float beatsToSkip);
    
    inline const int getBeatMultiplierCounter() const noexcept { return beatMultiplierCounter; }
    inline const int getAccentMultiplierCounter() const noexcept { return accentMultiplierCounter; }
    inline const int getLengthMultiplierCounter() const noexcept { return lengthMultiplierCounter; }
    inline const int getTranspCounter() const noexcept { return transpCounter; }
    inline const SynchronicSyncMode getMode() const noexcept {return active->getMode(); }
    
    inline void setTuning(Tuning::Ptr tuning)
    {
        tuner = tuning;
    }
    
    inline void setTempo(Tempo::Ptr newTempo)
    {
        tempo = newTempo;
    }
    
    inline Tuning::Ptr getTuning(void) const noexcept
    {
        return tuner;
    }
    
    inline Tempo::Ptr getTempo(void) const noexcept
    {
        return tempo;
    }
    
    inline int getTuningId(void) const noexcept
    {
        return tuner->getId();
    }
    
    inline int getTempoId(void) const noexcept
    {
        return tempo->getId();
    }
    
    inline void attachToSynthesiser(BKSynthesiser* main)
    {
        synth = main;
    }
    //void  atReset();
    
private:
    int Id;
    BKSynthesiser* synth;
    GeneralSettings::Ptr general;
    SynchronicPreparation::Ptr active;
    
    double sampleRate;
    
    Tempo::Ptr tempo;
    Tuning::Ptr tuner;
    Array<float> tuningOffsets;
    PitchClass tuningBasePitch;

    int beatCounter;  //beat (or pulse) counter; max set by users -- sNumBeats
    
    //parameter field counters
    int beatMultiplierCounter;   //beat length (time between beats) multipliers
    int accentMultiplierCounter; //accent multipliers
    int lengthMultiplierCounter; //note length (sounding length) multipliers (multiples of 50ms, at least for now)
    int transpCounter;     //transposition offsets
    
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

    //adaptive tempo stuff
    /*
    uint64 atTimer, atLastTime; //in samples
    int atDelta;                //in ms
    Array<int> atDeltaHistory;  //in ms
    void atNewNote();
    void atNewNoteOff();
    void atCalculatePeriodMultiplier();
    float adaptiveTempoPeriodMultiplier;
     */
    
    bool shouldPlay;
    
    JUCE_LEAK_DETECTOR(SynchronicProcessor);
};

/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ SYNCHRONIC ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */

class Synchronic : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<Synchronic>   Ptr;
    typedef Array<Synchronic::Ptr>                  PtrArr;
    typedef Array<Synchronic::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Synchronic>                  Arr;
    typedef OwnedArray<Synchronic, CriticalSection> CSArr;
    
    Synchronic(BKSynthesiser *s,
               SynchronicPreparation::Ptr prep,
               Tuning::Ptr tuning,
               Tempo::Ptr tempo,
               GeneralSettings::Ptr general,
               int Id):
    sPrep(new SynchronicPreparation(prep)),
    aPrep(new SynchronicPreparation(sPrep)),
    processor(new SynchronicProcessor(s, aPrep, tuning, tempo, general, Id)),
    Id(Id),
    name(String(Id)),
    X(-1),Y(-1),
    editted(false)
    {
        
    }
    
    Synchronic(BKSynthesiser *s,
               Tuning::Ptr tuning,
               Tempo::Ptr tempo,
               GeneralSettings::Ptr general,
               BKUpdateState::Ptr us,
               int Id):
    Id(Id),
    name(String(Id)),
    updateState(us),
    X(-1),Y(-1),
    editted(false)
    {
        sPrep       = new SynchronicPreparation(tuning, tempo);
        aPrep       = new SynchronicPreparation(sPrep);
        processor   = new SynchronicProcessor(s, aPrep, tuning, tempo, general, Id);
    };
    
    void prepareToPlay(double sampleRate, BKSynthesiser* main)
    {
        processor->attachToSynthesiser(main);
        processor->setCurrentPlaybackSampleRate(sampleRate);
    }
    
    void prepareToPlay(double sampleRate)
    {
        processor->setCurrentPlaybackSampleRate(sampleRate);
    }
    
    inline void setTuning(Tuning::Ptr tuning)
    {
        processor->setTuning(tuning);
    }
    
    inline void setTempo(Tempo::Ptr t)
    {
        processor->setTempo(t);
    }
    
    inline void copy(Synchronic::Ptr from)
    {
        sPrep->copy(from->sPrep);
        aPrep->copy(sPrep);
    }
    
    inline ValueTree getState(void)
    {
        ValueTree prep( vtagSynchronic);
        
        prep.setProperty( "Id",Id, 0);
        prep.setProperty( "name", name, 0);
        prep.setProperty( "tuning", processor->getTuning()->getId(), 0);
        prep.setProperty( "tempo", processor->getTempo()->getId(), 0);
        prep.setProperty( ptagSynchronic_numBeats,            sPrep->getNumBeats(), 0);
        prep.setProperty( ptagSynchronic_clusterMin,          sPrep->getClusterMin(), 0);
        prep.setProperty( ptagSynchronic_clusterMax,          sPrep->getClusterMax(), 0);
        prep.setProperty( ptagSynchronic_clusterThresh,       sPrep->getClusterThreshMS(), 0);
        prep.setProperty( ptagSynchronic_mode,                sPrep->getMode(), 0);
        prep.setProperty( ptagSynchronic_beatsToSkip,         sPrep->getBeatsToSkip(), 0);
        
        prep.setProperty( posX, X, 0);
        prep.setProperty( posY, Y, 0);
        
        
        ValueTree beatMults( vtagSynchronic_beatMults);
        int count = 0;
        for (auto f : sPrep->getBeatMultipliers())
        {
            beatMults.      setProperty( ptagFloat + String(count++), f, 0);
        }
        prep.addChild(beatMults, -1, 0);
        
        
        ValueTree lenMults( vtagSynchronic_lengthMults);
        count = 0;
        for (auto f : sPrep->getLengthMultipliers())
        {
            lenMults.       setProperty( ptagFloat + String(count++), f, 0);
        }
        prep.addChild(lenMults, -1, 0);
        
        
        ValueTree accentMults( vtagSynchronic_accentMults);
        count = 0;
        for (auto f : sPrep->getAccentMultipliers())
        {
            accentMults.    setProperty( ptagFloat + String(count++), f, 0);
        }
        prep.addChild(accentMults, -1, 0);
        
        
        ValueTree transposition( vtagSynchronic_transpOffsets);
        
        int tcount = 0;
        for (auto arr : sPrep->getTransposition())
        {
            ValueTree t("t"+String(tcount++));
            count = 0;
            for (auto f : arr)  t.setProperty( ptagFloat + String(count++), f, 0);
            transposition.addChild(t,-1,0);
        }
        prep.addChild(transposition, -1, 0);
        
        return prep;
        
    }
    
    inline void setState(XmlElement* e, Tuning::PtrArr tuning, Tempo::PtrArr tempo)
    {
        editted = true;
        
        int i; float f;
        
        Id = e->getStringAttribute("Id").getIntValue();
        
        String n = e->getStringAttribute("name");
        
        if (n != String::empty)     name = n;
        else                        name = String(Id);
        
        i = e->getStringAttribute("tuning").getIntValue();
        
        bool found = false;
        for (auto p : tuning)
        {
            if (p->getId() == i)
            {
                setTuning(p);
                found = true;
            }
        }
        if (!found) setTuning(tuning[0]);
        
        i = e->getStringAttribute("tempo").getIntValue();
        
        found = false;
        for (auto p : tempo)
        {
            if (p->getId() == i)
            {
                setTempo(p);
                found = true;
            }
        }
        if (!found) setTempo(tempo[0]);
        
        i = e->getStringAttribute(ptagSynchronic_numBeats).getIntValue();
        sPrep->setNumBeats(i);
        
        i = e->getStringAttribute(ptagSynchronic_clusterMin).getIntValue();
        sPrep->setClusterMin(i);
        
        i = e->getStringAttribute(ptagSynchronic_clusterMax).getIntValue();
        sPrep->setClusterMax(i);
        
        i = e->getStringAttribute(ptagSynchronic_clusterThresh).getIntValue();
        sPrep->setClusterThresh(i);
        
        i = e->getStringAttribute(ptagSynchronic_mode).getIntValue();
        sPrep->setMode((SynchronicSyncMode) i);
        
        i = e->getStringAttribute(ptagSynchronic_beatsToSkip).getIntValue();
        sPrep->setBeatsToSkip(i);
        
        n = e->getStringAttribute(posX);
        if (n != String::empty) X = n.getIntValue();
        else                    X = -1;
        
        n = e->getStringAttribute(posY);
        if (n != String::empty) Y = n.getIntValue();
        else                    Y = -1;
        
        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName(vtagSynchronic_beatMults))
            {
                Array<float> beats;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        beats.add(f);
                    }
                }
                
                sPrep->setBeatMultipliers(beats);
                
            }
            else  if (sub->hasTagName(vtagSynchronic_accentMults))
            {
                Array<float> accents;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        accents.add(f);
                    }
                }
                
                sPrep->setAccentMultipliers(accents);
                
            }
            else  if (sub->hasTagName(vtagSynchronic_lengthMults))
            {
                Array<float> lens;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        lens.add(f);
                    }
                }
                
                sPrep->setLengthMultipliers(lens);
                
            }
            else  if (sub->hasTagName(vtagSynchronic_transpOffsets))
            {
                Array<Array<float>> atransp;
                int tcount = 0;
                forEachXmlChildElement (*sub, asub)
                {
                    if (asub->hasTagName("t"+String(tcount++)))
                    {
                        Array<float> transp;
                        for (int k = 0; k < 128; k++)
                        {
                            String attr = asub->getStringAttribute(ptagFloat + String(k));
                            
                            if (attr == String::empty) break;
                            else
                            {
                                f = attr.getFloatValue();
                                transp.add(f);
                            }
                        }
                        atransp.set(tcount-1, transp);
                    }
                }
                
                sPrep->setTransposition(atransp);
            }
        }
        
        aPrep->copy(sPrep);
    }
    
    ~Synchronic() {};
    
    inline int getId() {return Id;}
    
    SynchronicPreparation::Ptr      sPrep;
    SynchronicPreparation::Ptr      aPrep;
    SynchronicProcessor::Ptr        processor;
    
    void reset()
    {
        aPrep->copy(sPrep);
        //processor->atReset();
        updateState->synchronicPreparationDidChange = true;
        DBG("synchronic reset");
    }
    
    //void didChange(bool which) { updateState->synchronicPreparationDidChange = which; }
    
    inline String getName(void) const noexcept {return name;}
    
    inline void setName(String newName)
    {
        name = newName;
        updateState->synchronicPreparationDidChange = true;
    }
    
    inline void setPosition(int x, int y) { X=x;Y=y;}
    inline Point<int> getPosition(void) { return Point<int>(X,Y);}
    inline void setPosition(Point<int> point) { X = point.getX(); Y= point.getY();}
    inline void setX(int x) { X = x; }
    inline void setY(int y) { Y = y; }
    inline int getX(void) const noexcept { return X; }
    inline int getY(void) const noexcept { return Y; }
    
    bool editted;
    
private:
    int Id;
    String name;
    BKUpdateState::Ptr updateState;
    
    int X,Y;
    
    
    JUCE_LEAK_DETECTOR(Synchronic)
};

class SynchronicModPreparation : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<SynchronicModPreparation>   Ptr;
    typedef Array<SynchronicModPreparation::Ptr>                  PtrArr;
    typedef Array<SynchronicModPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<SynchronicModPreparation>                  Arr;
    typedef OwnedArray<SynchronicModPreparation, CriticalSection> CSArr;
    
    /*
     SynchronicId = 0,
     SynchronicTuning,
     SynchronicTempo,
     SynchronicNumPulses,
     SynchronicClusterMin,
     SynchronicClusterMax,
     SynchronicClusterThresh,
     SynchronicMode,
     SynchronicBeatsToSkip,
     SynchronicBeatMultipliers,
     SynchronicLengthMultipliers,
     SynchronicAccentMultipliers,
     SynchronicTranspOffsets,
     AT1Mode,
     AT1History,
     AT1Subdivisions,
     AT1Min,
     AT1Max,
     */
    
    SynchronicModPreparation(SynchronicPreparation::Ptr p, int Id):
    Id(Id),
    X(-1),Y(-1),
    editted(false)
    {
        param.ensureStorageAllocated(cSynchronicParameterTypes.size());
        
        param.set(SynchronicNumPulses, String(p->getNumBeats()));
        param.set(SynchronicClusterMin, String(p->getClusterMin()));
        param.set(SynchronicClusterMax, String(p->getClusterMax()));
        param.set(SynchronicClusterThresh, String(p->getClusterThreshMS()));
        param.set(SynchronicMode, String(p->getMode()));
        param.set(SynchronicBeatsToSkip, String(p->getBeatsToSkip()));
        param.set(SynchronicBeatMultipliers, floatArrayToString(p->getBeatMultipliers()));
        param.set(SynchronicLengthMultipliers, floatArrayToString(p->getLengthMultipliers()));
        param.set(SynchronicAccentMultipliers, floatArrayToString(p->getAccentMultipliers()));
        param.set(SynchronicTranspOffsets, arrayFloatArrayToString(p->getTransposition()));
        
    }
    
    
    SynchronicModPreparation(int Id):
    Id(Id),
    X(-1),Y(-1),
    editted(false)
    {
        param.set(SynchronicNumPulses, "");
        param.set(SynchronicClusterMin, "");
        param.set(SynchronicClusterMax, "");
        param.set(SynchronicClusterThresh, "");
        param.set(SynchronicMode, "");
        param.set(SynchronicBeatsToSkip, "");
        param.set(SynchronicBeatMultipliers, "");
        param.set(SynchronicLengthMultipliers, "");
        param.set(SynchronicAccentMultipliers, "");
        param.set(SynchronicTranspOffsets, "");
    }
    
    inline void setId(int newId) { Id = newId; }
    inline int getId(void) const noexcept { return Id; }
    
    inline ValueTree getState(int Id)
    {
        ValueTree prep( vtagModSynchronic );
        
        prep.setProperty( "Id",Id, 0);
        
        String p = "";
        
        p = getParam(SynchronicNumPulses);
        if (p != String::empty) prep.setProperty( ptagSynchronic_numBeats,            p.getIntValue(), 0);
        
        p = getParam(SynchronicClusterMin);
        if (p != String::empty) prep.setProperty( ptagSynchronic_clusterMin,          p.getIntValue(), 0);
        
        p = getParam(SynchronicClusterMax);
        if (p != String::empty) prep.setProperty( ptagSynchronic_clusterMax,          p.getIntValue(), 0);
        
        p = getParam(SynchronicClusterThresh);
        if (p != String::empty) prep.setProperty( ptagSynchronic_clusterThresh,       p.getIntValue(), 0);
        
        p = getParam(SynchronicMode);
        if (p != String::empty) prep.setProperty( ptagSynchronic_mode,                p.getIntValue(), 0);
        
        p = getParam(SynchronicBeatsToSkip);
        if (p != String::empty) prep.setProperty( ptagSynchronic_beatsToSkip,         p.getIntValue(), 0);
        
        ValueTree beatMults( vtagSynchronic_beatMults);
        int count = 0;
        p = getParam(SynchronicBeatMultipliers);
        if (p != String::empty)
        {
            Array<float> m = stringToFloatArray(p);
            for (auto f : m)
            {
                beatMults.      setProperty( ptagFloat + String(count++), f, 0);
            }
        }
        prep.addChild(beatMults, -1, 0);
        
        
        ValueTree lengthMults( vtagSynchronic_lengthMults);
        count = 0;
        p = getParam(SynchronicLengthMultipliers);
        if (p != String::empty)
        {
            Array<float> m = stringToFloatArray(p);
            for (auto f : m)
            {
                lengthMults.      setProperty( ptagFloat + String(count++), f, 0);
            }
        }
        prep.addChild(lengthMults, -1, 0);
        
        
        ValueTree accentMults( vtagSynchronic_accentMults);
        count = 0;
        p = getParam(SynchronicAccentMultipliers);
        if (p != String::empty)
        {
            Array<float> m = stringToFloatArray(p);
            for (auto f : m)
            {
                accentMults.      setProperty( ptagFloat + String(count++), f, 0);
            }
        }
        prep.addChild(accentMults, -1, 0);
        
        
        ValueTree transpOffsets( vtagSynchronic_transpOffsets);
        count = 0;
        p = getParam(SynchronicTranspOffsets);
        if (p != String::empty)
        {
            Array<float> m = stringToFloatArray(p);
            for (auto f : m)
            {
                transpOffsets.      setProperty( ptagFloat + String(count++), f, 0);
            }
        }
        prep.addChild(transpOffsets, -1, 0);
        
        
        return prep;
        
    }
    
    inline void setState(XmlElement* e)
    {
        editted = true;
        
        float f;
        
        Id = e->getStringAttribute("Id").getIntValue();
        
        String p = e->getStringAttribute(ptagSynchronic_numBeats);
        setParam(SynchronicNumPulses, p);
        
        p = e->getStringAttribute(ptagSynchronic_clusterMin);
        setParam(SynchronicClusterMin, p);
        
        p = e->getStringAttribute(ptagSynchronic_clusterMax);
        setParam(SynchronicClusterMax, p);
        
        p = e->getStringAttribute(ptagSynchronic_clusterThresh);
        setParam(SynchronicClusterThresh, p);
        
        p = e->getStringAttribute(ptagSynchronic_mode);
        setParam(SynchronicMode, p);
        
        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName(vtagSynchronic_beatMults))
            {
                Array<float> beats;
                
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        beats.add(f);
                    }
                }
                
                setParam(SynchronicBeatMultipliers, floatArrayToString(beats));
                
            }
            else  if (sub->hasTagName(vtagSynchronic_accentMults))
            {
                Array<float> accents;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        accents.add(f);
                    }
                }
                
                setParam(SynchronicAccentMultipliers, floatArrayToString(accents));
                
            }
            else  if (sub->hasTagName(vtagSynchronic_lengthMults))
            {
                Array<float> lens;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        lens.add(f);
                    }
                }
                
                setParam(SynchronicLengthMultipliers, floatArrayToString(lens));
                
            }
            else  if (sub->hasTagName(vtagSynchronic_transpOffsets))
            {
                Array<float> transp;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        transp.add(f);
                    }
                }
                
                setParam(SynchronicTranspOffsets, floatArrayToString(transp));
            }
        }
        
    }
    
    
    ~SynchronicModPreparation(void)
    {
        
    }
    
    inline void copy(SynchronicPreparation::Ptr p)
    {
        param.set(SynchronicNumPulses, String(p->getNumBeats()));
        param.set(SynchronicClusterMin, String(p->getClusterMin()));
        param.set(SynchronicClusterMax, String(p->getClusterMax()));
        param.set(SynchronicClusterThresh, String(p->getClusterThreshMS()));
        param.set(SynchronicMode, String(p->getMode()));
        param.set(SynchronicBeatsToSkip, String(p->getBeatsToSkip()));
        param.set(SynchronicBeatMultipliers, floatArrayToString(p->getBeatMultipliers()));
        param.set(SynchronicLengthMultipliers, floatArrayToString(p->getLengthMultipliers()));
        param.set(SynchronicAccentMultipliers, floatArrayToString(p->getAccentMultipliers()));
        param.set(SynchronicTranspOffsets, arrayFloatArrayToString(p->getTransposition()));
    }
    
    inline void copy(SynchronicModPreparation::Ptr p)
    {
        for (int i = SynchronicId+1; i < SynchronicParameterTypeNil; i++)
        {
            param.set(i, p->getParam((SynchronicParameterType)i));
        }
    }
    
    void clearAll()
    {
        for (int i = SynchronicId+1; i < SynchronicParameterTypeNil; i++)
        {
            param.set(i, "");
        }
    }
    
    
    inline const StringArray getStringArray(void) { return param; }
    
    inline const String getParam(SynchronicParameterType type)
    {
        if (type != SynchronicId)
            return param[type];
        else
            return "";
    }
    
    inline void setParam(SynchronicParameterType type, String val)
    {
        param.set(type, val);
    }
    
    void print(void)
    {
        
    }
    
    inline String getName(void) const noexcept {return name;}
    inline void setName(String newName) {name = newName;}
    
    inline void setPosition(int x, int y) { X=x;Y=y;}
    inline Point<int> getPosition(void) { return Point<int>(X,Y);}
    inline void setPosition(Point<int> point) { X = point.getX(); Y= point.getY();}
    inline void setX(int x) { X = x; }
    inline void setY(int y) { Y = y; }
    inline int getX(void) const noexcept { return X; }
    inline int getY(void) const noexcept { return Y; }
    
    bool editted;
    
private:
    int Id;
    String name;
    StringArray          param;
    
    int X,Y;
    
    
    JUCE_LEAK_DETECTOR(SynchronicModPreparation);
};

#endif  // SYNCHRONIC_H_INCLUDED
