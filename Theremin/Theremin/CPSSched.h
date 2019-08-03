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
#include <cstdint>

#include <chrono>

#include <queue>

#include "Timestamped.h"

class CPSSched final
{
public:
	/**
	 * @pre No tiene que haber un scheduler creado
	 * @post Crea el scheduler en el thread

			 Si ya existe incrementa la cuenta de referencias
	 */
	static void create();

	/**
	 * @pre El scheduler tiene que existir
	 * @post Destruye el scheduler del thread, si sólo
	         hay una referencia.
			 Caso contrario sólo decrementa la cuenta de referencias
	 */
	static void destroy();

	/**
	 * @post Forkea el hilo de ejecución en dos continuaciones
	 */
	static Cont fork(Cont cont1, Cont cont2);

	/**
	 * @post Espera la cantidad de tiempo especificada,
	         y ejecuta la continuación especificada
	 */
	static Cont waitFor(std::chrono::steady_clock::duration duration, Cont cont);

	/**
	 * @post Deja el CPU a otra continuación
	 */
	static Cont yield(Cont cont);

private:
	/**
	* @post Crea el scheduler
	*/
	CPSSched();

	/**
	* @post Destruye el scheduler
	*/
	~CPSSched();

	/**
	 * @post Devuelve la instancia de thread
	 */
	static CPSSched * getInstance();

	/*
	 * @post Ejecuta una continuación pendiente del scheduler
	 */
	static Cont executePendingContinuation(CPSSched *scheduler);

	std::priority_queue<Timestamped<std::chrono::steady_clock::time_point, Cont>, std::deque<Timestamped<std::chrono::steady_clock::time_point, Cont>>, Timestamped<std::chrono::steady_clock::time_point, Cont>::TimeCompare> waitingContinuationsByTimestamp;
	std::queue<Cont> continuationsToExecute;

	std::chrono::steady_clock::time_point currentTimestamp_m;
};