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

#include <iostream>

#include "ThereminSystem.h"

int main() {
	Theremin::System::run();
}

/*
// Para pruebas

#include "Cont.h"
#include "CPSSched.h"

#include "DistanceSensorReader.h"

DistanceSensor::Reader sensor1Reader(
	DistanceSensor::Configuration()
	.withEchoId(26)
	.withTriggerId(19)
	.withNumberOfSamples(10)
	.withExpectedTemperature(20)
	.withMaxDistance(2.0)
);

DistanceSensor::Reader sensor2Reader(
	DistanceSensor::Configuration()
		.withEchoId(13)
		.withTriggerId(6)
		.withNumberOfSamples(10)
		.withExpectedTemperature(20)
		.withMaxDistance(2.0)
);

Cont readSensor(DistanceSensor::Reader *reader);
Cont processSensorDistance(DistanceSensor::Reader *reader, boost::optional<double> distance);

Cont initialState(void *data);

int main()
{
	CPSSched::create();

	runCPS( Cont( initialState, (void *) nullptr ) );
}

Cont initialState(void *data) {
	return Cont(readSensor, &sensor2Reader);
}

Cont readSensor(DistanceSensor::Reader *reader) {
	return reader->read(PCont<boost::optional<double>>(processSensorDistance, reader));
}

Cont processSensorDistance(DistanceSensor::Reader *reader, boost::optional<double> distance) {
	if (distance.is_initialized()) {
		std::cout << "Reader " << (reader == &sensor1Reader ? 1 : 2) << " = " << *distance << std::endl;
	}

	return CPSSched::waitFor( std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::milliseconds(500)), Cont(readSensor, reader) );
}
*/