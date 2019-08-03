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
#include "DistanceSensorSynchronizedContext.h"

#include <thread>
#include <atomic>
#include <memory>

namespace Theremin {
	class UserInput final
	{
	public:
		/**
		 * @post Crea el lector de sensores, indicando si tiene que arrancar
		         en segundo plano
		 */
		UserInput(bool backgroundThread);

		/**
		 * @post Destruye el lector de sensores
		 */
		~UserInput();

		/**
		 * @post Realiza la lectura de los sensores si no está en segundo plano.
		 */
		void doReading();

		/**
		 * @post Devuelve el volumen deseado (Normalizado)
		 */
		boost::optional<double> getVolume();

		/**
		 * @post Devuelve el pitch deseado (Relativo), entre 0 y 1
		 */
		boost::optional<double> getRelativePitch();

	private:
		/**
		 * @post Realiza la lectura de los sensores en el thread
		         actual
		 */
		void doReading_internal();

		/**
		 * @post Estado inicial
		 */
		static Cont initialState(Theremin::UserInput *userInput);

		/**
		 * @post Lee el sensor
		 */
		static Cont readSensor(DistanceSensor::SynchronizedContext *context);

		/**
		 * @post Revisa si hay petición de cierre
		 */
		static Cont checkStopCondition(Theremin::UserInput *userInput);

		static constexpr double volumeMinDistance_m = 0.06;
		static constexpr double volumeMaxDistance_m = 0.4;

		static constexpr double pitchMinDistance_m = 0.06;
		static constexpr double pitchMaxDistance_m = 0.4;

		std::unique_ptr<std::thread> backgroundThread_m;

		DistanceSensor::SynchronizedContext volumeSensorContext_m;
		DistanceSensor::SynchronizedContext pitchSensorContext_m;

		std::atomic<bool> stop_m;
	};

}

