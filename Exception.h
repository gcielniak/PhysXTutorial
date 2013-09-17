#pragma once

#include <string>

///Generic exception.

///
///Inherit from this class to implement your own exceptions.
///
class Exception
{
	std::string message;
public:
	virtual const std::string& what() const throw() { return message; }
	Exception(const std::string s) : message(s) {};
};

