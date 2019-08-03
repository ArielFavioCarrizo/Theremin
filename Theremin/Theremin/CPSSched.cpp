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

#include "CPSSched.h"
#include <stdexcept>
#include <thread>

#include <limits>

thread_local CPSSched *threadSched = nullptr;

thread_local uint64_t threadSched_refcount = 0;

void CPSSched::create() {
	if (threadSched_refcount == std::numeric_limits<uint64_t>::max()) {
		throw std::runtime_error("Count overflow");
	}

	if (threadSched_refcount++ == 0) {
		threadSched = new CPSSched();
	}
}

void CPSSched::destroy() {
	if (threadSched != nullptr ) {
		if ( threadSched_refcount-- == 1 ) {
			delete threadSched;
			threadSched = nullptr;
		}
	}
	else {
		throw std::runtime_error("No CPSSched exists on current thread");
	}
}

Cont CPSSched::fork(Cont cont1, Cont cont2) {
	CPSSched *scheduler = CPSSched::getInstance();

	scheduler->continuationsToExecute.push(cont1);
	scheduler->continuationsToExecute.push(cont2);

	return Cont(CPSSched::executePendingContinuation, scheduler);
}

Cont CPSSched::waitFor(std::chrono::steady_clock::duration duration, Cont cont) {
	CPSSched *scheduler = CPSSched::getInstance();

	std::chrono::steady_clock::time_point& currentTimestamp = scheduler->currentTimestamp_m;

	currentTimestamp = std::chrono::steady_clock::now();

	scheduler->waitingContinuationsByTimestamp.push(Timestamped<std::chrono::steady_clock::time_point, Cont>(currentTimestamp + duration, cont));

	return Cont(CPSSched::executePendingContinuation, scheduler);
}

Cont CPSSched::yield(Cont cont) {
	CPSSched *scheduler = CPSSched::getInstance();

	scheduler->continuationsToExecute.push(cont);

	return Cont(CPSSched::executePendingContinuation, scheduler);
}

CPSSched::CPSSched()
{

}

CPSSched::~CPSSched()
{
}

/**
* @post Devuelve la instancia de thread
*/
CPSSched * CPSSched::getInstance() {
	if (threadSched != nullptr) {
		return threadSched;
	}
	else {
		throw std::runtime_error("Missing thread CPSSched");
	}
}

Cont CPSSched::executePendingContinuation(CPSSched *scheduler) {
	// Tratar las continuaciones que deben empezar después en un momento determinado (En sleep)
	if (!scheduler->waitingContinuationsByTimestamp.empty()) {
		auto nextTimestampedCont = scheduler->waitingContinuationsByTimestamp.top();
		auto& currentTimestamp = scheduler->currentTimestamp_m;

		bool queueContinuation;

		/*
		 * Si no hay más continuaciones para ejecutar immediatamente esperar a que
		 * se cumpla el tiempo de la continuación más próxima a ejecutarse
		 */
		if (scheduler->continuationsToExecute.empty()) {
			if (currentTimestamp < nextTimestampedCont.timestamp()) {
				auto waitTime = nextTimestampedCont.timestamp() - currentTimestamp;

				std::this_thread::sleep_for(waitTime);

				currentTimestamp = nextTimestampedCont.timestamp();
			}

			queueContinuation = true;
		}
		else {
			// Caso contrario encolarla para ejecutarse sólo si se cumplió el tiempo que le corresponde
			queueContinuation = (currentTimestamp >= nextTimestampedCont.timestamp());
		}

		// Encolar la continuación para ser ejecutada si corresponde
		if ( queueContinuation ) {
			Cont nextCont = nextTimestampedCont.value();

			scheduler->waitingContinuationsByTimestamp.pop();

			scheduler->continuationsToExecute.push(nextCont);
		}
	}

    // Desencolar y ejecutar una continuación pendiente de ser ejecutada
	if (!scheduler->continuationsToExecute.empty()) {
		Cont nextCont = scheduler->continuationsToExecute.front();
		scheduler->continuationsToExecute.pop();

		return nextCont;
	}
	else {
		throw std::runtime_error("Assertion error: Missing tasks!");
	}
}