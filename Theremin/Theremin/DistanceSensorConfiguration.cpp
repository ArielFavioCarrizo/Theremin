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

#include "DistanceSensorConfiguration.h"

DistanceSensor::Configuration::Configuration() {
	
}

DistanceSensor::Configuration DistanceSensor::Configuration::withTriggerId(int id) {
	DistanceSensor::Configuration newConfig = *this;

	newConfig.triggerId_m = id;

	return newConfig;
}

DistanceSensor::Configuration DistanceSensor::Configuration::withEchoId(int id) {
	DistanceSensor::Configuration newConfig = *this;

	newConfig.echoId_m = id;

	return newConfig;
}

DistanceSensor::Configuration DistanceSensor::Configuration::withNumberOfSamples(int numberOfSamples) {
	if (numberOfSamples > 0) {
		DistanceSensor::Configuration newConfig = *this;

		newConfig.numberOfSamples_m = numberOfSamples;

		return newConfig;
	}
	else {
		throw std::runtime_error("Invalid number of samples");
	}
}

DistanceSensor::Configuration DistanceSensor::Configuration::withExpectedTemperature(double temperature) {
	DistanceSensor::Configuration newConfig = *this;

	newConfig.expectedTemperature_m = temperature;

	return newConfig;
}

DistanceSensor::Configuration DistanceSensor::Configuration::withMaxDistance(double maxDistance) {
	DistanceSensor::Configuration newConfig = *this;

	newConfig.maxDistance_m = maxDistance;

	return newConfig;
}

int DistanceSensor::Configuration::getTriggerId() {
	return *this->triggerId_m;
}

int DistanceSensor::Configuration::getEchoId() {
	return *this->echoId_m;
}

int DistanceSensor::Configuration::getNumberOfSamples() {
	return *this->numberOfSamples_m;
}

double DistanceSensor::Configuration::getExpectedTemperature() {
	return *this->expectedTemperature_m;
}

double DistanceSensor::Configuration::getMaxDistance() {
	return *this->maxDistance_m;
}