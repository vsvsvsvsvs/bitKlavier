/*
  ==============================================================================

    BKUtilities.h
    Created: 16 Nov 2016 11:12:15am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKUTILITIES_H_INCLUDED
#define BKUTILITIES_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "BKReferenceCountedBuffer.h"

#include "GraphicsConstants.h"

#include "AudioConstants.h"

String          intArrayToString(Array<int> arr);
String          floatArrayToString(Array<float> arr);
String          offsetArrayToString(Array<float> arr);
Array<int>      stringToIntArray(String s);
Array<int>      keymapStringToIntArray(String s);
Array<float>    stringToFloatArray(String s);
Array<float>    stringOrderedPairsToFloatArray(String s, int size);

double          mtof(double f);
double          ftom(double f);

BKParameterDataType getBKDataType ( SynchronicParameterType param);
BKParameterDataType getBKDataType ( TuningParameterType param);
BKParameterDataType getBKDataType ( DirectParameterType param);
BKParameterDataType getBKDataType ( NostalgicParameterType param);

inline int layerToLayerId(BKNoteType type, int layer) { return (50*type)+layer;}

typedef enum BKTextFieldType
{
    BKParameter = 0,
    BKModification,
    BKTFNil
    
}BKTextFieldType;

#endif  // BKUTILITIES_H_INCLUDED
