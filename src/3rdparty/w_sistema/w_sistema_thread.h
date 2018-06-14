/*********************************************************************************************
 *	Name		: w_sistema_thread.h
 *	Description	: Declaraciones para el trabajo con hilos
 *	Copyright	(LSeWa) 2014 PROYECTO EWA (http://www.proyectoewa.com/)
 ********************************************************************************************/
#ifndef _W_SISTEMA_THREAD_H_
#define _W_SISTEMA_THREAD_H_

#ifdef _WIN32
#else
// TODO: evitar cargarlo aquí
#include <pthread.h>
#endif

// Trabajo con hilos

// Declaración de la clase agrupadora de datos
class WThreadData
{
public:
	virtual WThreadData* clone() const = 0;	// clona la instancia
	virtual ~WThreadData() = 0;				// destructor virtual puro
};

// Declaración de la clase agrupadora de datos templatizada
template<class T>
class TWThreadData : public WThreadData
{
public:
	TWThreadData()
		: data()
	{
	}
	TWThreadData(const T & t)
		: data(t)
	{
	}
	virtual WThreadData* clone() const
	{
		return (WThreadData*)new TWThreadData(*this);
	}
	virtual const T & operator*() const
	{
		return data;
	}
	virtual ~TWThreadData()
	{
	}
	T data;
};

// Declaración de la clase agrupadora de hilos
class WThread : public WThreadData
{
public:
	enum Estado
	{
		SIN_ERROR,
		ERRONEO,
		YA_EXISTE,
		NO_EXISTE,
	};

public:
	typedef void (*Function)(const WThreadData* const);
	typedef void (*FunctionAll)(const WThread &);
	static void functionNull(const WThreadData* const);

public:
	// Constructor
	WThread(Function = functionNull);
	WThread(Function, const WThreadData &);
	WThread(FunctionAll);
	WThread(FunctionAll, const WThreadData &);

	// Métodos de copia (de la configuración, no del estado)
	//	NOTA: la copia de esta estructura (o asignación a otra instancia) no supone la copia del estado si
	//			no que la copia solo es de la configuración. Es decir, si el WThreadDataThread estaba en ejecución
	//			y se hace una copia del WThreadDataThread, esa copia estará lista para realizar la misma ejecución
	//			a través de la llamada "WThreadDataCreateThread(..)" que ejecutará (DE FORMA COMPLEMENTARIA) la
	//			configuración asignada.
	WThread(const WThread &);
	WThread & operator= (const WThread &);

	// Destructor
	virtual ~WThread();

	// Clonador
	virtual WThreadData* clone() const;

	// Ejecutores
	bool executeFunction() const;	//< ejecuta la función periódica en el hilo actual

	// Control de la ejecución
	Estado start();					//< ejecuta la función en un nuevo hilo
	Estado restart();				//< solicita el reiniciado de la cuenta
	Estado pause();					//< solicita la detención programada al hilo
	Estado resume();				//< solicita la continuación al hilo
	Estado stop();					//< solicita la finalización programada al hilo
	Estado end();					//< solicita la finalización abrupta el hilo
	Estado finish();				//< espera a que finalice el hilo

	// Observadores
	bool isStarted()
	const;					//< determina si se ha indicado que este hilo (el principal o su hijo) se ha iniciado
	bool isStoped() const;					//< determina si se ha solicitado que este hilo (el principal o su hijo) se detenga
	int getId() const;
	const WThreadData & operator*() const;	//< obtener la referencia constante a los datos locales del hijo
	const WThreadData & getData() const;	//< esta es análoga a la anterior

	template<class T>
	const T & getData() const
	{
		return * (static_cast<const T*>(&this->getData()));
	}

	bool hasData() const;					//< determina si este grupo tiene asociado datos
	void setData(const WThreadData &);		//< ajusta los datos (elimina los anteriores si hubiera)
	void clearData();						//< elimina los datos (si hubiera)

	void setFunction(Function fun);
	void setFunction(FunctionAll fun);
private:
	Function					function;
	FunctionAll					functionAll;
	const WThreadData*			ptrData;			//< esta es la memoria "de entrada" (exclusiva del padre)
	int 						idThread;			//< con signo por si hay errores
	bool						borrarEnHijo;		//< a TRUE indica que se gestiona la memoria en el hijo
	// TODO: darle la funcionalidad si es necesario
	bool /* [only write] */
	started;			//< a FALSE por defecto; a TRUE indica al proceso observador que el proceso actual está en marcha
	bool /* [only read] */
	pleaseStop;			//< a FALSE por defecto; a TRUE indica al proceso actual que por favor se detenga

public:
#ifdef _WIN32
	void*						handle;
#else
	pthread_t					handle;
#endif

private:
	WThread*		threadHijo;		//< esta es la memoria compartida (mientras no muera el hijo <o se la cargue con un delete>)
	// TODO: pasar al hijo una referencia, para evitar que se lo cargue
	WThread*		threadPadre;	//< esta es la memoria compartida (mientras no muera el padre) que lee el hijo

private:
	void setDefaults();
	void destructor();
};

#endif // _W_SISTEMA_THREAD_H_
