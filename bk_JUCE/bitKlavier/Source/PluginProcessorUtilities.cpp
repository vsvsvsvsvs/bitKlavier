/*
  ==============================================================================

    PluginProcessorUtilities.cpp
    Created: 22 Mar 2017 12:28:41pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "PluginProcessor.h"

void BKAudioProcessor::updateUI(void)
{
    updateState->pianoDidChangeForGraph = true;
    updateState->directPreparationDidChange = true;
    updateState->nostalgicPreparationDidChange = true;
    updateState->synchronicPreparationDidChange = true;
    updateState->tuningPreparationDidChange = true;
    updateState->tempoPreparationDidChange = true;
    updateState->generalSettingsDidChange = true;
    
    
    updateState->directDidChange = true;
    updateState->keymapDidChange = true;
}

void BKAudioProcessor::loadSamples(BKSampleLoadType type, String path, int subsound)
{
    didLoadMainPianoSamples = false;
    
    // Check if path isn't valid and load BKLoadLite if it is not
    if (type == BKLoadSoundfont)
    {
        if ((lastSoundfont == path) && (lastInstrument == subsound)) return;
        
        if (!path.startsWith("default.sf"))
        {
            File file(path);
            
            if (!file.exists())
            {
                type = BKLoadLite;
            }
        }
    }
    
    
    if (type == BKLoadSoundfont)
    {
        currentSampleType = BKLoadSoundfont;
        
        currentSoundfont = path;
        currentInstrument = subsound;
        
        loader.startThread();
    }
    else if (type < BKLoadSoundfont)
    {
        if (lastSampleType == type) return;
        
        currentSampleType = type;
        
        int numSamplesPerLayer = 29;
        int numHarmSamples = 69;
        int numResSamples = 88;
        
        progress = 0.0;
        progressInc = 1.0f / ((type == BKLoadHeavy)  ? (numSamplesPerLayer * 8 + (numResSamples + numHarmSamples)) :
                              (type == BKLoadMedium) ? (numSamplesPerLayer * 4) :
                              (type == BKLoadLite)   ? (numSamplesPerLayer * 2) :
                              (type == BKLoadLitest) ? (numSamplesPerLayer * 1) : 1.0);
        
        loader.startThread();
    }
    
    lastSampleType = currentSampleType;
    lastSoundfont = currentSoundfont;
    lastInstrument = currentInstrument;
}


void BKAudioProcessor::collectSoundfontsFromFolder(File folder)
{
    DirectoryIterator iter (File (folder), true, "*.sf2;*.sfz");
    while (iter.next())
    {
        File soundfontFile (iter.getFile());

        soundfontNames.add(soundfontFile.getFullPathName());
    }
}

void BKAudioProcessor::collectGalleriesFromFolder(File folder)
{
    DirectoryIterator xmlIter (File (folder), true, "*.xml");
    while (xmlIter.next())
    {
        File galleryFile (xmlIter.getFile());
        
        galleryNames.add(galleryFile.getFullPathName());
    }
    
    
    DirectoryIterator jsonIter (File (folder), true, "*.json");
    while (jsonIter.next())
    {
        File galleryFile (jsonIter.getFile());
        
        galleryNames.add(galleryFile.getFullPathName());
    }
}

void BKAudioProcessor::collectGalleries(void)
{
    galleryNames.clear();
    
    File bkGalleries;
    
#if JUCE_IOS
    bkGalleries = File::getSpecialLocation (File::userDocumentsDirectory);
#endif
#if JUCE_MAC
    bkGalleries = File::getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier").getChildFile("galleries");
#endif
#if JUCE_WINDOWS || JUCE_LINUX
    bkGalleries = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getChildFile("galleries");
#endif
    
    collectGalleriesFromFolder(bkGalleries);
}

void BKAudioProcessor::collectSoundfonts(void)
{
    soundfontNames.clear();
    
    File bkSoundfonts;
    
#if JUCE_IOS
    bkSoundfonts = File::getSpecialLocation (File::userDocumentsDirectory);
#endif
#if JUCE_MAC
    bkSoundfonts = File::getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier").getChildFile("soundfonts");
#endif
#if JUCE_WINDOWS || JUCE_LINUX
    bkSoundfonts = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier").getChildFile("soundfonts");
#endif
    
    collectSoundfontsFromFolder(bkSoundfonts);
    soundfontNames.sort(true);
}

String BKAudioProcessor::firstGallery(void)
{
    collectGalleries();
    
    return galleryNames[0];
}



void BKAudioProcessor::updateGalleries()
{
    collectGalleries();
    
    updateState->setAllCurrentIdsTo(-1);
    
    gallery->getGeneralSettings();
    mainPianoSynth.updateGeneralSettings(gallery->getGeneralSettings());
    hammerReleaseSynth.updateGeneralSettings(gallery->getGeneralSettings());
    resonanceReleaseSynth.updateGeneralSettings(gallery->getGeneralSettings());
    pedalSynth.updateGeneralSettings(gallery->getGeneralSettings());
    
    clipboard.clear();
    
    updateState->galleriesUpdated = true;
}

