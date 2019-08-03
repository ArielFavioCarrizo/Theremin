/**
 * Copyright (c) 2019 Ariel Favio Carrizo
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the
 *	 names of its contributors may be used to endorse or promote products
 *	 derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ThereminSynthesizer.h"

#include <cmath>

Theremin::Synthesizer::Synthesizer(int sampleRate, size_t waveTableSize) :
	waveTableSize_m(waveTableSize),
	sampleRate_m(sampleRate),
	relativePeriod_m(this->waveTableSize_m * relativePhaseScale_m),
	relativeVolumeFilter_m(4096),
	relativePhaseSpeedFilter_m(4096)
{
	// Genera el wavetable
	this->wavetable_m = std::unique_ptr<int16_t[]>(new int16_t[waveTableSize]);

	for (size_t i = 0; i < this->waveTableSize_m; i++) {
		double value = (double)( sin((double)i * 2.0 * M_PI / (double)waveTableSize_m) + 1.0 ) / 2.0 * 65535.0 - 32768.0;

		double minValue = -32768.0;
		double maxValue = 32767.0;

		if (value > maxValue) {
			value = maxValue;
		}
		else if (value < minValue ) {
			value = minValue;
		}

		this->wavetable_m[i] = (int16_t)value;
	}

	this->relativeScaledPhase_m = 0;

}

Theremin::Synthesizer::~Synthesizer() {

}

void Theremin::Synthesizer::setVolume(boost::optional<double> volume) {
	this->volume_m = volume;
}

void Theremin::Synthesizer::setFrequency(boost::optional<double> pitch) {
	this->frequency_m = pitch;
}

void Theremin::Synthesizer::tick(int16_t *data, size_t nFrames) {
	int32_t relativePhaseSpeed;

	// Calcular nuevo valor de velocidad de fase y ponerlo en el filtro correspondiente
	if (this->frequency_m.is_initialized()) {
		relativePhaseSpeed = (double)this->relativePeriod_m * *this->frequency_m /(double)this->sampleRate_m;
	}
	else {
		relativePhaseSpeed = 0;
	}

	this->relativePhaseSpeedFilter_m.put(relativePhaseSpeed);

	// Calcular nuevo valor de volumen y ponerlo en el filtro correspondiente
	const int32_t maxRelativeVolume = 65536;

	int32_t relativeVolume;
	if (this->volume_m.is_initialized()) {
		relativeVolume = (int32_t)(*this->volume_m * (double)maxRelativeVolume);
	}
	else {
		relativeVolume = 0;
	}

	if (relativeVolume > maxRelativeVolume) {
		relativeVolume = maxRelativeVolume;
	}

	this->relativeVolumeFilter_m.put(relativeVolume);

	// Realizar copia local de la fase
	uint32_t relativeScaledPhase = this->relativeScaledPhase_m;

	// Sintetizar
	for (size_t i = 0; i < nFrames; i++) {
		int32_t value = (int32_t)this->wavetable_m[(relativeScaledPhase / relativePhaseScale_m) % this->waveTableSize_m];

		value = value * this->relativeVolumeFilter_m.get() / maxRelativeVolume;

		data[i] = (int16_t)value;

		relativeScaledPhase += this->relativePhaseSpeedFilter_m.get();
	}

	// Guardar la nueva fase
	this->relativeScaledPhase_m = relativeScaledPhase;
}