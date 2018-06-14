/*********************************************************************************************
 *	Name		: w_sistema_thread_win.cpp
 *	Description	: Trabajo con hilos
 * Copyright	(LSeWa) 2014 PROYECTO EWA (http://www.proyectoewa.com/)
********************************************************************************************/
#ifdef _WIN32
#include "3rdparty/w_sistema/w_sistema_thread.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

// Esta función auxiliar asemeja el funcionamiento de la gestión de los hilos en POSIX
static void functionAuxiliar(const WThreadData & ptrData)
{
	// Cast para obtener área de datos compartida
	const WThread* threadHijo = (const WThread*)(&ptrData);

	// Lanzar la ejecución programada
	threadHijo->executeFunction();

	// Borrar el área de datos compartida
	delete threadHijo;
}

/**
* Crea un nuevo hilo de ejecucion
* \return	WThread::SIN_ERROR	Creación correcta del hilo (en "thread.idThread" el id de hilo creado y "thread.started" a 'true')
*			WThread::ERRONEO	Error durante la creación del hilo
**/
WThread::Estado WThread::start()
{
	if(this->isStarted() == true)
	{
		return WThread::YA_EXISTE;
	}

	// Crear un ára de datos en memoria compartida (dinámica)
	if(this->threadHijo == NULL)
	{
		this->threadHijo = (WThread*)this->clone();
		this->threadHijo->threadPadre = this;
	}

	// Crear el proceso usando el área de datos compartida
	DWORD idThread;
	HANDLE handle = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)functionAuxiliar, (LPVOID)this->threadHijo, 0,
	                               &idThread);
	if(handle == NULL)
	{
#ifdef _DEBUG
		DWORD error = GetLastError();
#endif
		return WThread::ERRONEO;
	}
	this->idThread	= idThread;
	this->handle	= handle;
	this->started	= true;
	this->pleaseStop = false;
	return WThread::SIN_ERROR;
}

/**
 * Solicita la finalización abrupta el hilo y lo finaliza
 * \return	WThread::SIN_ERROR	Solicitud correcta
 *			WThread::NO_EXISTE	No estaba en ejecución
 *			WThread::ERRONEO	Error durante la detención del hilo
**/
WThread::Estado WThread::end()
{
	if(this->isStarted() == true)
	{
		// Solicitar la finalización del hilo al S.O.
		const int retTerminate = ::TerminateThread((HANDLE)this->handle, 0);
		if(retTerminate == 0)
		{
			// If the function fails, the return value is zero.
#ifdef _DEBUG
			DWORD error = GetLastError();
#endif
			return WThread::ERRONEO;
		}

		// Indicar que ya no está en ejecución
		this->started = false;
	}

	return this->finish();
}

/**
 * Espera a que finalice el hilo
 * \return	WThread::SIN_ERROR	Solicitud correcta
 *			WThread::NO_EXISTE	No estaba en ejecución
 *			WThread::ERRONEO	Error durante la detención del hilo
**/
WThread::Estado WThread::finish()
{
	if(this->handle == NULL)
	{
		// Ya no existe
		return WThread::NO_EXISTE;
	}

	// Esperar a que finalice correctamente
	const int retWait = ::WaitForSingleObject(this->handle, INFINITE);
	if(retWait != WAIT_OBJECT_0)
	{
		// If the function fails, the return value is zero.
#ifdef _DEBUG
		DWORD error = GetLastError();
#endif
		return WThread::ERRONEO;
	}

	// Solicitar el cierre del hilo al S.O.
	const int retClose = ::CloseHandle((HANDLE)this->handle);
	if(retClose == 0)
	{
		// If the function fails, the return value is zero.
#ifdef _DEBUG
		DWORD error = GetLastError();
#endif
		return WThread::ERRONEO;
	}

	// Liberar el descriptor
	this->handle  = NULL;

	// Todo ha ido bien
	return WThread::SIN_ERROR;
}

#endif
