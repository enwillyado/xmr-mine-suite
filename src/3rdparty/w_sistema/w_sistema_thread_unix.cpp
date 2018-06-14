/*********************************************************************************************
 *	Name		: w_sistema_thread_unix.cpp
 *	Description	: Trabajo con hilos
 * Copyright	(LSeWa) 2014 PROYECTO EWA (http://www.proyectoewa.com/)
 ********************************************************************************************/
#ifndef _WIN32

#include "3rdparty/w_sistema/w_sistema_thread.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Esta función auxiliar asemeja el funcionamiento de la gestión de los hilos en POSIX
static void* functionAuxiliar(void* ptrData)
{
	// Cast para obtener área de datos compartida
	const WThread* threadHijo = (const WThread*)(ptrData);

	// Ajustar su PID
	pthread_t pid = pthread_self();
#ifdef SI_SE_PUDIERA
	if(threadHijo->threadPadre != NULL)
	{
		threadHijo->threadPadre->pidThread = pid;
	}
	threadHijo->pidThread = pid;
#endif

	// Lanzar la ejecución programada
	threadHijo->executeFunction();

	// Borrar el área de datos compartida
	delete threadHijo;

	// Retornar
	return NULL;
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
	pthread_t idThread;
	int ret = ::pthread_create(&idThread, NULL, functionAuxiliar, (void*)this->threadHijo);
	if(ret != 0)
	{
		// ASK: ¿ GetLastError() ?
		return WThread::ERRONEO;
	}
	this->idThread	= idThread;
	this->handle	= idThread;
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
		// Solicitar el cierre del hilo al S.O.
		const int ret = ::pthread_cancel((pthread_t)this->handle);
		if(ret != 0)
		{
			return WThread::ERRONEO;
		}

		// Indicar que ya no está en ejecución
		this->started = false;

		// Y liberar ya el descriptor
		this->handle = NULL;
	}
	else
	{
		return this->finish();
	}
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
	const int retJoin = ::pthread_join((pthread_t)this->handle, NULL);
	if(retJoin != 0)
	{
		// If the function fails, the return value is zero.
		return WThread::ERRONEO;
	}

	// Liberar el descriptor
	this->handle = NULL;

	// Todo ha ido bien
	return WThread::SIN_ERROR;
}

#endif
