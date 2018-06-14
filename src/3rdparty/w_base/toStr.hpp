/*********************************************************************************************
*	Name		: toStr.hpp
*	Description	: Método de conversión T, basado en el operador de flujo, para pasar a string
*  Copyright	(LSeWa) 2014 PROYECTO EWA (http://www.proyectoewa.com/)
********************************************************************************************/
#ifndef _TO_STR_HPP_
#define _TO_STR_HPP_

#include <string>       // std::string
#include <sstream>      // std::stringstream, std::stringbuf
#include <iomanip>      // std::setprecision

template <class T>
static std::string toStr (const T & t)
{
	// Push
	std::stringstream ss;
	ss << t;

	// Pop
	const std::string & ret = ss.str();
	return ret;
}

template <class T>
static std::string toStr (const T & t, const int & n)
{
	// Push
	std::stringstream ss;
	ss << std::setprecision(n)  << t;

	// Pop
	const std::string & ret = ss.str();
	return ret;
}

template <class T>
static std::string toStr (const T & t, const int & n, const bool & fixed)
{
	// Push
	std::stringstream ss;
	ss << std::fixed << std::setprecision(n) << t;

	// Pop
	const std::string & ret = ss.str();
	return ret;
}

#endif	//_TO_STR_HPP_
