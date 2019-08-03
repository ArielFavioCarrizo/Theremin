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

#include "Cont.h"
#include "DistanceSensorConfiguration.h"
#include "GPIO.h"

#include <chrono>
#include <vector>

#include <boost/optional.hpp>

namespace DistanceSensor {
	class Reader final
	{
	public:
		/**
		 * @post Crea un lector de distancia con la
		        configuración especificada
		 */
		Reader(DistanceSensor::Configuration configuration);

		/**
		 * @post Destruye el lector de distancia
		 */
		~Reader();

		/**
		 * @post Lee el sensor con la continuación
		         parametrizada, que indica la distancia
		         detectada
		 */
		Cont read(PCont<boost::optional<double>> pcont);

	private:
		GPIO::Handler echoGPIO_m;
		GPIO::Handler triggerGPIO_m;

		bool isInitialized_m; // Indica si fue inicializado

		const double speedOfSound_m; // Velocidad del sonido
		const std::chrono::steady_clock::duration maxWaveTravelTime_m; // Máximo tiempo que tarda en volver el impulso emitido por el sensor en cada lectura
		const int numberOfSamples_m; // Número de muestras a usar por cada lectura del sensor

		std::vector<std::chrono::nanoseconds> accumulatedSamples_m; // Muestras acumuladas en el proceso de lectura de un nuevo valor del sensor

		int pendingNumberOfSamples_m;

		std::chrono::steady_clock::time_point triggerHighTimestamp_m; // Timestamp del flanco ascendente del pin 'trigger'
		boost::optional<std::chrono::steady_clock::time_point> echoLowTimestamp_m; // Timestamp de la última vez en que estuvo el pin 'echo' en bajo
		boost::optional<std::chrono::steady_clock::time_point> echoHighTimestamp_m; // Timestamp de la última vez en que estuvo el pin 'echo' en alto

		PCont<boost::optional<double>> distancePCont_m; // Continuación para notificar la distancia después de terminar la detección
	};
}


