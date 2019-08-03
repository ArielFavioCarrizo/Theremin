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
#include "DistanceSensorReader.h"
#include "DistanceSensorConfiguration.h"
#include "SynchronizedVariable.h"

namespace DistanceSensor {
	/*
	 * Contexto sincronizado de sensor de distancia.
	   
	 * Útil cuando el sensor se lee en un thread,
	 * y se lee el resultado de la lectura en otro thread.
	 */
	class SynchronizedContext final
	{
	public:
		/**
		 * @post Crea un contexto sincronizado de sensor de distancia
		         con la configuración especificada
		 */
		SynchronizedContext(DistanceSensor::Configuration configuration);

		/**
		* @post Actualiza la lectura del sensor
		*/
		Cont update(Cont cont);

		/**
		* @post Lee la distancia
		*/
		boost::optional<double> getDistance();

	private:
		/**
		 * @post Actualiza la distancia con el valor especificado
		 */
		static Cont updateDistance(DistanceSensor::SynchronizedContext *context, boost::optional<double> distance);

		DistanceSensor::Reader sensorReader_m;
		SynchronizedVariable<boost::optional<double>> distance_m;

		Cont updateNextCont_m;
	};
}

