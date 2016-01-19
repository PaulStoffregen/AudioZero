/*
 * Copyright (c) 2015 by
 Arturo Guadalupi <a.guadalupi@arduino.cc>
 Angelo Scialabba <a.scialabba@arduino.cc>
 Claudio Indellicati <c.indellicati@arduino.cc> <bitron.it@gmail.com>

 * Audio library for Arduino Zero.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "AudioZero.h"
#include <SD.h>
#include <SPI.h>
#include "IntervalTimer.h"

#define DAC_PIN  A12  // A12 for Teensy LC, A14 for Teensy 3.2

/*Global variables*/
bool __StartFlag;
volatile uint32_t __SampleIndex;
uint32_t __HeadIndex;
uint32_t __NumberOfSamples; // Number of samples to read in block
uint8_t *__WavSamples;

int __Volume;

IntervalTimer myTimer;
float __sampleRate;
void Audio_Handler(void);

void AudioZeroClass::begin(uint32_t sampleRate)
{
	__StartFlag = false;
	__SampleIndex = 0;		//in order to start from the beginning
	__NumberOfSamples = 1024;	//samples to read to have a buffer

	/*Allocate the buffer where the samples are stored*/
	__WavSamples = (uint8_t *) malloc(__NumberOfSamples * sizeof(uint8_t));

	/*Modules configuration */
  	dacConfigure();
	tcConfigure(sampleRate);
}

void AudioZeroClass::end() {
	tcDisable();
	analogWrite(DAC_PIN, 0);
}

/*void AudioZeroClass::prepare(int volume){
//Not Implemented yet
}*/

void AudioZeroClass::play(File myFile)
{
	while (myFile.available()) {
		if (!__StartFlag) {
			myFile.read(__WavSamples, __NumberOfSamples);
			__HeadIndex = 0;
			/*once the buffer is filled for the first time the counter can be started*/
			tcStartCounter();
			__StartFlag = true;
		} else {
			uint32_t current__SampleIndex = __SampleIndex;
			if (current__SampleIndex > __HeadIndex) {
				myFile.read(&__WavSamples[__HeadIndex], current__SampleIndex - __HeadIndex);
				__HeadIndex = current__SampleIndex;
			} else if (current__SampleIndex < __HeadIndex) {
				myFile.read(&__WavSamples[__HeadIndex], __NumberOfSamples-1 - __HeadIndex);
				myFile.read(__WavSamples, current__SampleIndex);
				__HeadIndex = current__SampleIndex;
			}
		}
	}
	myFile.close();
}


/**
 * Configures the DAC in event triggered mode.
 *
 * Configures the DAC to use the module's default configuration, with output
 * channel mode configured for event triggered conversions.
 */
void AudioZeroClass::dacConfigure(void){
	analogWriteResolution(8);
	analogWrite(DAC_PIN, 0);
}

/**
 * Configures the TC to generate output events at the sample frequency.
 *
 * Configures the TC in Frequency Generation mode, with an event output once
 * each time the audio sample frequency period expires.
 */
void AudioZeroClass::tcConfigure(uint32_t sampleRate)
{
	__sampleRate = (float)sampleRate;
}

void AudioZeroClass::tcStartCounter()
{
	myTimer.begin(Audio_Handler, 1000000.0 / __sampleRate);
}

void AudioZeroClass::tcDisable()
{
	myTimer.end();
}

AudioZeroClass AudioZero;

void Audio_Handler(void)
{
	if (__SampleIndex < __NumberOfSamples - 1) {
		analogWrite(DAC_PIN, __WavSamples[__SampleIndex++]);
	} else {
		__SampleIndex = 0;
	}
}

