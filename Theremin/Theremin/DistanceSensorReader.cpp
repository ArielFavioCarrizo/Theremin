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

#include "DistanceSensorReader.h"
#include "CPSSched.h"

#include <cmath>

DistanceSensor::Reader::Reader(DistanceSensor::Configuration configuration) :
	echoGPIO_m(configuration.getEchoId()),
	triggerGPIO_m(configuration.getTriggerId()),
	speedOfSound_m(331.3 + sqrt(1 + (configuration.getExpectedTemperature() / 273.15))),
	maxWaveTravelTime_m(
		std::chrono::duration_cast<std::chrono::steady_clock::duration>(
			std::chrono::nanoseconds(
				(int64_t)(configuration.getMaxDistance() * 2.0 * 1000000000.0 / this->speedOfSound_m )
			)
		)
	),
	numberOfSamples_m(configuration.getNumberOfSamples())
{
	this->isInitialized_m = false;
}

DistanceSensor::Reader::~Reader() {

}

Cont DistanceSensor::Reader::read(PCont<boost::optional<double>> pcont) {
	// Máquina de estados
	struct States {
		// Hace las preparaciones previas
		static Cont prepare(DistanceSensor::Reader *reader) {
			// Preparar el estado de las muestras
			reader->accumulatedSamples_m.clear();
			reader->pendingNumberOfSamples_m = reader->numberOfSamples_m;

			// Si está inicializado pasa directamente al estado de poner en alto el pin de trigger
			if (reader->isInitialized_m) {
				return Cont(setTriggerHigh, reader);
			}
			else {
				// Caso contrario primero pasa al estado de inicialización
				return Cont(startInitialization, reader);
			}
		}

		// Realiza la inicialización de los pines, para que queden en un estado definido
		static Cont startInitialization(DistanceSensor::Reader *reader) {
			reader->echoGPIO_m.setDirection(GPIO::Direction::in);
			reader->triggerGPIO_m.setDirection(GPIO::Direction::out);
			reader->triggerGPIO_m.write(false);

			// Esperar 500 milisegundos y pasar al siguiente estado
			return CPSSched::waitFor(
				std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::milliseconds(500)),
				Cont(endInitialization, reader)
			);
		}

		// Finaliza la inicialización
		static Cont endInitialization(DistanceSensor::Reader *reader) {
			reader->isInitialized_m = true;

			return Cont(setTriggerHigh, reader);
		}

		// Setea en alto el pin de trigger
		static Cont setTriggerHigh(DistanceSensor::Reader *reader) {
			reader->triggerGPIO_m.write(true);

			reader->triggerHighTimestamp_m = std::chrono::steady_clock::now();

			// Esperar 10 microsegundos y pasar al siguiente estado
			return CPSSched::waitFor(
				std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::microseconds(10)),
				Cont(setTriggerLow, reader)
			);
		}

		// Setear en bajo el pin de trigger
		static Cont setTriggerLow(DistanceSensor::Reader *reader) {
			reader->triggerGPIO_m.write(false);
			return Cont(waitForHighEcho, reader);
		}

		// Esperar a que el pin 'echo' se ponga en alto
		static Cont waitForHighEcho(DistanceSensor::Reader *reader) {
			std::chrono::steady_clock::time_point currentTimestamp = std::chrono::steady_clock::now();

			// Si no superó el tiempo máximo
			if (currentTimestamp - reader->triggerHighTimestamp_m <= reader->maxWaveTravelTime_m) {
				// Si llegó el flanco ascendente del 'echo'
				if (reader->echoGPIO_m.read()) {
					// Almacenar el timestamp del flanco ascendente
					reader->echoHighTimestamp_m = currentTimestamp;

					// Pasar a estado siguiente
					return Cont(waitForLowEcho, reader);
				}
				else { // Si todavía no llegó
					// Almacenar el timestamp de la última vez que se detectó el pin 'echo' en bajo
					reader->echoLowTimestamp_m = currentTimestamp;

					// Y volver al mismo estado, cediendo CPU antes a otros 'green threads'
					return CPSSched::yield(
						Cont(waitForHighEcho, reader)
					);
				}
			}
			else {
				// Caso contrario dejar en vacío el timestamp del flanco ascendente de 'echo' , porque no se detectó
				reader->echoHighTimestamp_m = boost::optional<std::chrono::steady_clock::time_point>();

				// Pasar a estado siguiente
				return Cont(waitForLowEcho, reader);
			}
		}

		// Esperar a que el pin 'echo' se ponga en bajo
		static Cont waitForLowEcho(DistanceSensor::Reader *reader) {
			if (reader->echoGPIO_m.read()) {
				reader->echoHighTimestamp_m = std::chrono::steady_clock::now();

				// Volver al mismo estado, cediendo CPU antes a otros 'green threads' 
				return CPSSched::yield(
					Cont(waitForLowEcho, reader)
				);
			}
			else {
				// Pasar a estado siguiente
				return Cont(readSample, reader);
			}
		}

		// Leer muestra
		static Cont readSample(DistanceSensor::Reader *reader) {
			if (reader->echoHighTimestamp_m.is_initialized() && reader->echoLowTimestamp_m.is_initialized()) {
				auto timeDelta = *reader->echoHighTimestamp_m - *reader->echoLowTimestamp_m; // Calcular delta de tiempo

				if (timeDelta <= reader->maxWaveTravelTime_m) {
					// Guardar la muestra
					reader->accumulatedSamples_m.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(timeDelta));
				}
			}

			reader->pendingNumberOfSamples_m--;

			// Si hay muestras pendientes volver a repetir la secuencia de detección
			if (reader->pendingNumberOfSamples_m > 0) {
				return Cont(setTriggerHigh, reader);
			}
			else {
				// Caso contrario pasar al estado siguiente
				return Cont(calculateDistance, reader);
			}

		}

		// Calcular distancia
		static Cont calculateDistance(DistanceSensor::Reader *reader) {
			boost::optional<double> distance;

			/*
			 * Si hay muestras calcula la mediana entre las muestras de distancia
			 */
			if (reader->accumulatedSamples_m.size() > 0) {
				double timePassed;

				std::sort(reader->accumulatedSamples_m.begin(), reader->accumulatedSamples_m.end());

				if (reader->accumulatedSamples_m.size() == 1) {
					timePassed = (double)reader->accumulatedSamples_m[0].count();
				}
				else if (reader->accumulatedSamples_m.size() % 2 == 1) {
					timePassed = (double)reader->accumulatedSamples_m[reader->accumulatedSamples_m.size() / 2].count();
				}
				else {
					size_t index = reader->accumulatedSamples_m.size() / 2;

					timePassed = (double)(reader->accumulatedSamples_m[index - 1].count() + reader->accumulatedSamples_m[index].count()) / 2;
				}

				distance = timePassed * reader->speedOfSound_m / 2.0 / 1000000000.0;
			}

			// Pasar el control al invocador, indicando la distancia
			return reader->distancePCont_m.invoke(distance);
		}
	};

	// Setea la continuación de notificación de distancia
	this->distancePCont_m = pcont;

	// Salta al estado de preparación
	return States::prepare(this);
}

#if 0

Cont DistanceSensor::Reader::setTriggerHigh(DistanceSensor::Reader *reader) {
	reader->triggerGPIO_m.write(true);

	reader->triggerHighTimestamp_m = std::chrono::steady_clock::now();

	return CPSSched::waitFor(
		std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::microseconds(10)),
		Cont( DistanceSensor::Reader::setTriggerLow, reader)
	);
}

Cont DistanceSensor::Reader::setTriggerLow(DistanceSensor::Reader *reader) {
	reader->triggerGPIO_m.write(false);
	return Cont(DistanceSensor::Reader::waitForHighEcho, reader);
}

Cont DistanceSensor::Reader::waitForHighEcho(DistanceSensor::Reader *reader) {
	std::chrono::steady_clock::time_point currentTimestamp = std::chrono::steady_clock::now();

	if (currentTimestamp - reader->triggerHighTimestamp_m <= reader->maxWaveTravelTime_m) {
		if ( reader->echoGPIO_m.read()) {
			reader->echoHighTimestamp_m = std::chrono::steady_clock::now();
		}
		else {
			reader->echoLowTimestamp_m = std::chrono::steady_clock::now();

			return CPSSched::yield(
				Cont(DistanceSensor::Reader::waitForHighEcho, reader)
			);
		}
	}
	else {
		reader->echoHighTimestamp_m = boost::optional<std::chrono::steady_clock::time_point>();
	}

	return Cont(DistanceSensor::Reader::waitForLowEcho, reader);
}

Cont DistanceSensor::Reader::waitForLowEcho(DistanceSensor::Reader *reader) {
	if (reader->echoGPIO_m.read()) {
		reader->echoHighTimestamp_m = std::chrono::steady_clock::now();

		return CPSSched::yield(
			Cont(DistanceSensor::Reader::waitForLowEcho, reader)
		);
	}
	else {
		return Cont(DistanceSensor::Reader::readSample, reader);
	}
}

Cont DistanceSensor::Reader::readSample(DistanceSensor::Reader *reader) {
	if (reader->echoHighTimestamp_m.is_initialized() && reader->echoLowTimestamp_m.is_initialized() ) {
		auto timeDelta = *reader->echoHighTimestamp_m - *reader->echoLowTimestamp_m;

		if (timeDelta <= reader->maxWaveTravelTime_m) {
			reader->accumulatedSamples_m.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(timeDelta));
		}
	}

	reader->pendingNumberOfSamples_m--;

	if (reader->pendingNumberOfSamples_m > 0) {
		return Cont(DistanceSensor::Reader::setTriggerHigh, reader);
	}
	else {
		return Cont(DistanceSensor::Reader::calculateDistance, reader);
	}
	
}

Cont DistanceSensor::Reader::calculateDistance(DistanceSensor::Reader *reader) {
	boost::optional<double> distance;

	if (reader->accumulatedSamples_m.size() > 0) {
		double timePassed;

		std::sort(reader->accumulatedSamples_m.begin(), reader->accumulatedSamples_m.end());

		if (reader->accumulatedSamples_m.size() == 1) {
			timePassed = (double)reader->accumulatedSamples_m[0].count();
		}
		else if (reader->accumulatedSamples_m.size() % 2 == 1) {
			timePassed = (double) reader->accumulatedSamples_m[reader->accumulatedSamples_m.size() / 2].count();
		}
		else {
			size_t index = reader->accumulatedSamples_m.size() / 2;
			
			timePassed = (double)(reader->accumulatedSamples_m[index-1].count() + reader->accumulatedSamples_m[index].count()) / 2;
		}

		distance = timePassed * reader->speedOfSound_m / 2.0 / 1000000000.0;
	}
	
	return reader->distancePCont_m.invoke(distance);
}
#endif