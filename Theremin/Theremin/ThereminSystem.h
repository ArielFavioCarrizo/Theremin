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

#pragma once 
#include "ThereminUserInput.h"
#include "ThereminSynthesizer.h"

#include <stk/RtAudio.h>
#include <stk/Stk.h>

namespace Theremin {
	class System final
	{
	public:
		/**
		 * @post Realiza la ejecución del sistema de Theremin
		 */
		static void run();

	private:
		/**
		* @post Crea el sistema de Theremin
		*/
		System();

		/**
		* @post Destruye el sistema de Theremin
		*/
		~System();

		/**
		 * @post Convierte el pitch en valor de frecuencia
		 */
		static double pitchToFrequency(double pitch);

		/**
		 * @post Realiza la operación de síntesis de audio.
		         Es un callback de STK, que es invocado
				 cuando la biblioteca necesita la síntesis
				 de nuevas muestras.

				 userData: Es un puntero al sistema de Theremin
		 */
		static int rtAudioCallback(void *outputBuffer, void *inputBuffer, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void *userData);

		Theremin::UserInput userInput_m;
		Theremin::Synthesizer synthesizer_m;

		static constexpr float minPitch_m = 0.0f;
		static constexpr float maxPitch_m = 70.0f;

		static constexpr unsigned int sampleRate_m = 44100;
		static constexpr unsigned int waveTableSize_m = 4096;

		unsigned int bufferFrames; // Longitud del buffer de audio que le llega al callback

		RtAudio::StreamOptions streamOptions;
	};
}
