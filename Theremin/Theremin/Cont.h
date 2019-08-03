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

#include <stdexcept>

class Cont final
{
public:
	/**
	 * @post Crea una continuación inválida
	 */
	Cont();

	/**
	 * @post Crea una continuación con el puntero
			 a la función y los datos especificados
	 */
	template<typename T>
	Cont(Cont(*funptr) (T *), T *data) {
		this->funptr_m = reinterpret_cast<Cont (*) (void *)>(funptr);
		this->data_m = reinterpret_cast<void *>(data);
	}

	template<typename T>
	static Cont create(Cont(*funptr) (T *), T *data) {
		return Cont(funptr, data);
	}

	friend void runCPS(Cont firstContinuation);

private:
	static Cont invalidFun(void *data) {
		throw std::runtime_error("Invalid continuation");
	}

	Cont(*funptr_m) (void *);
	void *data_m;
};

// Continuación 'dummy' para salir del loop de CPS
static Cont CPS_EXIT = Cont::create<void *>(nullptr, nullptr);

/**
 * @post Ejecuta la secuencia de continuaciones.
 *       
 *       Internamente, manda la orden para crear el scheduler, y
 *       destruirlo cuando sale del método.
 *
 *       ATENCIÓN: Para evitar su destrucción, tiene que haberse creado
 *                 una referencia fuera.
 */
void runCPS(Cont firstContinuation);

template<typename T>
class PCont final {
public:
	PCont() {
		this->funptr_m = PCont::invalidFun;
	}

	template<typename D>
	PCont(Cont(*funptr) (D *, T), D *data) {
		this->funptr_m = reinterpret_cast<Cont(*) (void *, T)>(funptr);
		this->data_m = reinterpret_cast<void *>(data);
	}

	Cont invoke(T input) {
		return this->funptr_m(this->data_m, input);
	}

private:
	static Cont invalidFun(void *data, T input) {
		return Cont();
	}

	Cont(*funptr_m) (void *data, T input);
	void *data_m;
};