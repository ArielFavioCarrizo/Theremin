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

#include "ThereminSystem.h"

#include <cmath>

Theremin::System::System() :
	userInput_m(false),
	synthesizer_m(sampleRate_m, waveTableSize_m)
{

}

Theremin::System::~System() {

}

void Theremin::System::run() {
	stk::Stk::setSampleRate(sampleRate_m);

	Theremin::System system;
	RtAudio rtAudio;

	// Abre el stream
	RtAudio::StreamParameters outputParameters;
	outputParameters.deviceId = rtAudio.getDefaultOutputDevice(); // Dispositivo de salida predeterminado
	outputParameters.nChannels = 1; // Un canal (Mono)
		
	RtAudioFormat format = RTAUDIO_SINT16; // Formato de muestra de 16 bits

	system.bufferFrames = stk::RT_BUFFER_SIZE; // Longitud del buffer de frames deseado

	void *userData = static_cast<void *>(&system); // Para que el callback pueda acceder al objeto de sistema de Theremin

	RtAudio::StreamOptions *options = &(system.streamOptions);

	options->flags = RTAUDIO_ALSA_USE_DEFAULT;
	options->numberOfBuffers = 2;
	options->streamName = "Theremin";
	options->priority = 0; // No se usa

	RtAudioErrorCallback errorCallback = NULL; // No se usa un callback de error

	/*
	 * Abre el stream de audio.
	 */
	rtAudio.openStream(&outputParameters, nullptr, format, sampleRate_m, &system.bufferFrames, &Theremin::System::rtAudioCallback, userData, nullptr, errorCallback);

	/* 
	 * Comienza el stream de audio
     * Después de ésta operación la biblioteca invocará el callback en un thread creado por ella.
	 */
	rtAudio.startStream();

	// Realiza la lectura de los sensores de entrada (Bloqueante)
	system.userInput_m.doReading();
}

double Theremin::System::pitchToFrequency(double pitch) {
	return std::pow(2.0, 1.0 / 12.0 * (pitch - 49.0)) * 440.0;
}

int Theremin::System::rtAudioCallback(void *outputBuffer, void *inputBuffer, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void *userData) {
	Theremin::System *self = static_cast<Theremin::System *>(userData);

	// Setear volumen
	self->synthesizer_m.setVolume(self->userInput_m.getVolume());

	// Setear frecuencia
	boost::optional<double> relativePitch = self->userInput_m.getRelativePitch();
	boost::optional<double> absoluteFrequency;
	if (relativePitch.is_initialized()) {
		/*
		const double absolutePitch = minPitch_m + (maxPitch_m - minPitch_m) * *relativePitch;

		absoluteFrequency = Theremin::System::pitchToFrequency(absolutePitch);
		*/

		const double minFrequency = Theremin::System::pitchToFrequency(minPitch_m);
		const double maxFrequency = Theremin::System::pitchToFrequency(maxPitch_m);

		absoluteFrequency = minFrequency + (maxFrequency - minFrequency) * *relativePitch;
	}

	self->synthesizer_m.setFrequency(absoluteFrequency);

	// Sintetizar
	self->synthesizer_m.tick( static_cast<int16_t *>(outputBuffer), (size_t)nFrames);

	// Para continuar la operación del stream
	return 0;
}