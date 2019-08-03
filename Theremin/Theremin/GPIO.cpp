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

#include "GPIO.h"

#include <iostream>
#include <fstream>

#include <stdexcept>

GPIO::Handler::Handler(int id)
{
	this->gpioName = std::to_string(id);

	// Intentar exportar el GPIO
	std::ofstream exportgpio("/sys/class/gpio/export");

	if (exportgpio.is_open()) {
		exportgpio << this->gpioName;
	}
	else {
		throw std::runtime_error("Cannot export gpio " + this->gpioName);
	}

	this->directionFilename = "/sys/class/gpio/gpio" + this->gpioName + "/direction";
	this->valueFilename = "/sys/class/gpio/gpio" + this->gpioName + "/value";
}


GPIO::Handler::~Handler()
{
	// Intentar desexportar el GPIO
	std::ofstream unexportgpio("/sys/class/gpio/unexport");

	if (unexportgpio.is_open()) {
		unexportgpio << this->gpioName;
	}
	else {
		std::cerr  << "Cannot unexport gpio " + this->gpioName;
	}
}

void GPIO::Handler::setDirection(GPIO::Direction direction) {
	std::string fileName = this->directionFilename;

	std::ofstream setdirGPIO;

	while (!setdirGPIO.is_open()) {
		setdirGPIO.open(fileName);
	}

	std::string value;

	switch (direction)
	{
	case GPIO::Direction::in:
		value = "in";
		break;
	case GPIO::Direction::out:
		value = "out";
		break;
	default:
		throw std::runtime_error("Invalid gpio's direction");
		break;
	}

	setdirGPIO << value;
}

void GPIO::Handler::write(bool value) {
	std::ofstream setvalGPIO;

	while (!setvalGPIO.is_open()) {
		setvalGPIO.open(this->valueFilename);
	}

	setvalGPIO << (value ? 1 : 0);
}

bool GPIO::Handler::read() {
	std::ifstream getvalGPIO;

	while (!getvalGPIO.is_open()) {
		getvalGPIO.open(this->valueFilename);
	}

	std::string val_str;

	getvalGPIO >> val_str;

	return (val_str != "0");
}