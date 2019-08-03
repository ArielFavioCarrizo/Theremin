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

#include "ThereminUserInput.h"
#include "CPSSched.h"

#include <thread>
#include <cmath>

Theremin::UserInput::UserInput(bool backgroundThread) :
   volumeSensorContext_m(
		DistanceSensor::Configuration()
		.withEchoId(26)
		.withTriggerId(19)
		.withNumberOfSamples(10)
		.withExpectedTemperature(20)
		.withMaxDistance(volumeMaxDistance_m)
	),
    pitchSensorContext_m(
	   DistanceSensor::Configuration()
	      .withEchoId(13)
		  .withTriggerId(6)
	      .withNumberOfSamples(10)
	      .withExpectedTemperature(20)
	      .withMaxDistance(pitchMaxDistance_m)
    )
{
	this->stop_m = false;

	if (this->backgroundThread_m) {
		this->backgroundThread_m = std::unique_ptr<std::thread>(
				new std::thread(
					[this]() { this->doReading_internal(); }
		));
	}
}


Theremin::UserInput::~UserInput()
{
	this->stop_m = true;
	
	if (this->backgroundThread_m.get() != nullptr) {
		this->backgroundThread_m->join();
	}
}

void Theremin::UserInput::doReading() {
	if (this->backgroundThread_m.get() == nullptr) {
		this->doReading_internal();
	}
	else {
		throw std::runtime_error("Reading has been started on background");
	}
}

boost::optional<double> Theremin::UserInput::getVolume() {
	auto distance = this->volumeSensorContext_m.getDistance();

	if (distance.is_initialized()) {
		double normalisedDistance = (*distance - volumeMinDistance_m) / (volumeMaxDistance_m - volumeMinDistance_m);

		normalisedDistance = std::max(0.0, normalisedDistance);
		normalisedDistance = std::min(1.0, normalisedDistance);

		return normalisedDistance;
	}
	else {
		return boost::optional<double>();
	}
}


boost::optional<double> Theremin::UserInput::getRelativePitch() {
	auto distance = this->pitchSensorContext_m.getDistance();

	if (distance.is_initialized()) {
		double normalisedDistance = (*distance - pitchMinDistance_m) / (pitchMaxDistance_m - pitchMinDistance_m);

		normalisedDistance = std::max(0.0, normalisedDistance);
		normalisedDistance = std::min(1.0, normalisedDistance);

		return normalisedDistance;
	}
	else {
		return boost::optional<double>();
	}
}

void Theremin::UserInput::doReading_internal() {
	runCPS(Cont(Theremin::UserInput::initialState, this));
}

Cont Theremin::UserInput::initialState(Theremin::UserInput *userInput) {
	return CPSSched::fork(
		Cont(Theremin::UserInput::readSensor, &(userInput->volumeSensorContext_m)),
		Cont(Theremin::UserInput::readSensor, &(userInput->pitchSensorContext_m))
	);
}

Cont Theremin::UserInput::readSensor(DistanceSensor::SynchronizedContext *context) {
	return context->update(
		Cont(Theremin::UserInput::readSensor, context)
	);
}

Cont Theremin::UserInput::checkStopCondition(Theremin::UserInput *userInput) {
	if (userInput->stop_m) {
		return CPS_EXIT;
	}
	else {
		return CPSSched::waitFor(std::chrono::milliseconds(100), Cont(Theremin::UserInput::checkStopCondition, userInput));
	}
}