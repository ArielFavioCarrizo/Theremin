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

#include <boost/optional.hpp>

namespace DistanceSensor {
	class Configuration final
	{
	public:
		/**
		 * @post Crea una configuración de sensor
		 */
		Configuration();

		/**
		 * @post Especifica el id de trigger
		 */
		Configuration withTriggerId(int id);

		/**
		 * @post Especifica el trigger de echo
		 */
		Configuration withEchoId(int id);

		/**
		 * @post Especifica el número de muestras
		 */
		Configuration withNumberOfSamples(int numberOfSamples);

		/**
		 * @post Especifica la temperatura esperada en celsius
		 */
		Configuration withExpectedTemperature(double temperature);

		/**
		 * @post Especifica la máxima distancia en metros
		 */
		Configuration withMaxDistance(double maxDistance);

		/**
		 * @post Lee el id de trigger
		 */
		int getTriggerId();

		/**
		 * @post Devuelve el id de echo
		 */
		int getEchoId();

		/**
		 * @post Devuelve el número de muestras
		 */
		int getNumberOfSamples();

		/**
		 * @post Devuelve la temperatura esperada
		 */
		double getExpectedTemperature();

		/**
		 * @post Devuelve la máxima distancia
		 */
		double getMaxDistance();

	private:
		boost::optional<int> triggerId_m;
		boost::optional<int> echoId_m;

		boost::optional<int> numberOfSamples_m;
		boost::optional<double> expectedTemperature_m;
		boost::optional<double> maxDistance_m;
	};


}
