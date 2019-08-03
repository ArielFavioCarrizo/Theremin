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
#include <memory>

#include <boost/optional.hpp>
#include "SignalLinearFilter.h"

namespace Theremin {
	class Synthesizer final
	{
	public:
		/**
		 * @post Crea un sintetizador de Theremin
		         con el sampleRate y el tamaño de wavetable
				 especificado
		 */
		Synthesizer(int sampleRate, size_t waveTableSize);

		/**
		 * @post Destruye el sintetizador de Theremin
		 */
		~Synthesizer();

		/**
		 * @post Especifica el volumen deseado
		 */
		void setVolume(boost::optional<double> volume);

		/**
		 * @post Especifica la frecuencia deseada
		 */
		void setFrequency(boost::optional<double> frequency);

		/**
		 * @post Realiza un tick con el buffer de datos y el número de frames especificados
		 */
		void tick(int16_t *data, size_t nFrames);

	private:
		const size_t waveTableSize_m;
		const int sampleRate_m;

		std::unique_ptr<int16_t[]> wavetable_m;

		boost::optional<double> volume_m;
		boost::optional<double> frequency_m;

		const uint32_t relativePeriod_m;
		uint32_t relativeScaledPhase_m;
		static constexpr uint32_t relativePhaseScale_m = 1 << 6;

		Signal::LinearFilter relativeVolumeFilter_m;
		Signal::LinearFilter relativePhaseSpeedFilter_m;
	};
}

