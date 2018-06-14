/*********************************************************************************************
 *	Name		: w_sistema_thread_comun.cpp
 *	Description	: Trabajo con hilos
 * Copyright	(LSeWa) 2014 PROYECTO EWA (http://www.proyectoewa.com/)
 ********************************************************************************************/
#include "3rdparty/w_sistema/w_sistema_thread.h"

#include <cstring>	//< NULL

// ----------------------------------------------------------------------------
// WThreadData:
WThreadData::~WThreadData()
{
	// los destructores virtuales puros se han de implementar
}

// ----------------------------------------------------------------------------
// WThread:
// Función que no hace nada
void WThread::functionNull(const WThreadData* const ptrData)
{
}

// Constructor
WThread::WThread(WThread::Function fun)
{
	this->setDefaults();
	this->setFunction(fun);
}
WThread::WThread(WThread::Function fun, const WThreadData & ptrData)
{
	this->setDefaults();
	this->setFunction(fun);
	this->ptrData = ptrData.clone();	//< NOTA: ¡el configurador del hilo guarda una copia!
}
WThread::WThread(WThread::FunctionAll fun)
{
	this->setDefaults();
	this->setFunction(fun);
}
WThread::WThread(WThread::FunctionAll fun, const WThreadData & ptrData)
{
	this->setDefaults();
	this->setFunction(fun);
	this->ptrData = ptrData.clone();	//< NOTA: ¡el configurador del hilo guarda una copia!
}

WThread::WThread(const WThread & org)
{
	this->setDefaults();
	this->operator= (org);
}

// Destructor
WThread::~WThread()
{
	this->destructor();
}

// Clonador
WThreadData* WThread::clone() const
{
	WThread* ret = new WThread(*this);	// el operador de copia realiza la copia
	return (WThreadData*)ret;
}

// Destructor
void WThread::destructor()
{
	if(this->ptrData != NULL)
	{
		delete this->ptrData;
		this->ptrData = NULL;
	}

	// TODO: a partir de aquí, hay que tener en cuenta la sincronización si queremos hilar muy fino

	// Avisar al padre, si hay
	if(this->threadPadre != NULL)
	{
		if(this->threadPadre->threadHijo == this)
		{
			this->threadPadre->started = false;
			this->threadPadre->threadHijo = NULL;
			this->threadPadre = NULL;
		}
	}
	// Avisar al hijo, si hay
	if(this->threadHijo != NULL)
	{
		if(this->threadHijo->threadPadre == this)
		{
			// this->threadHijo->pleaseStop = false;	//< TODO: ¿pedimos al hijo que se muera o qué hacemos?
			this->threadHijo->threadPadre = NULL;
			this->threadHijo = NULL;
		}
	}
}

// Operadores
WThread & WThread::operator= (const WThread & org)
{
	this->destructor();
	this->setDefaults();

	// Copia por clonación
	if(org.ptrData != NULL)
	{
		this->ptrData	= org.ptrData->clone();
	}
	// Copia campo-a-campo
	this->function		= org.function;
	this->functionAll	= org.functionAll;
	this->idThread		= org.idThread;
	this->borrarEnHijo	= org.borrarEnHijo;

	// Retornar la propia estructura
	return *this;
}

// Observadores
bool WThread::isStarted() const
{
	return this->started;
}
bool WThread::isStoped() const
{
	return this->pleaseStop;
}
int WThread::getId() const
{
	return this->idThread;
}

//< obtener la referencia constante a los datos locales del hijo
const WThreadData & WThread::operator*() const
{
	return this->getData();
}
const WThreadData & WThread::getData() const
{
	return (this->hasData() == true) ? (*this->ptrData) : (*this);
}

bool WThread::hasData() const
{
	return this->ptrData != NULL;
}
void WThread::setData(const WThreadData & ptrData)
{
	this->clearData();
	this->ptrData = ptrData.clone();	//< NOTA: ¡el configurador del hilo guarda una copia!
}
void WThread::clearData()
{
	if(this->ptrData != NULL)
	{
		delete this->ptrData;
		this->ptrData = NULL;
	}
}

void WThread::setFunction(WThread::Function fun)
{
	this->function = fun;
}
void WThread::setFunction(WThread::FunctionAll fun)
{
	this->functionAll = fun;
}

// Ejecutores
bool WThread::executeFunction() const
{
	if(this->functionAll != NULL)
	{
		this->functionAll(*this);
		return true;
	}

	if(this->function != NULL)
	{
		this->function(this->ptrData);
		return true;
	}
	return false;
}

// Inicializadores
void WThread::setDefaults()
{
	// Funciones que se ejecutarán
	this->function = NULL;
	this->functionAll = NULL;

	// Área de datos
	this->ptrData = NULL;
	this->handle = NULL;
	this->idThread = 0;

	//
	this->borrarEnHijo = false;

	// Flags
	this->started = false;
	this->pleaseStop = false;

	// Estos punteros se vacían
	this->threadPadre = NULL;
	this->threadHijo = NULL;
}

// Detener (común entre plataformas)
/**
* Solicita la detención de forma controlada el hilo
* \return	WThread::SIN_ERROR	Solicitud correcta
*			WThread::NO_EXISTE	No estaba en ejecución
*			WThread::ERRONEO	Error durante la detención del hilo
**/
WThread::Estado WThread::stop()
{
	// Verificar que esté en ejecución
	if(this->isStarted() == false)
	{
		return WThread::NO_EXISTE;
	}

	// Verificar que exista la forma de comunicarse con el hijo
	if(this->threadHijo == NULL)
	{
		return WThread::ERRONEO;
	}

	// Ver si se ha solicitado la detención anteriormente
	if(this->threadHijo->pleaseStop == true)
	{
		// ASK: en caso de haberse solicitado ¿fallar? Por ahora no
	}

	// Solicitar la detención al hijo
	// NOTA: este es el único sitio (y forma) que se debería alterar la estructura del cliente
	this->threadHijo->pleaseStop = true;
	this->pleaseStop = true;

	// Todo ha ido bien
	return WThread::SIN_ERROR;
}
