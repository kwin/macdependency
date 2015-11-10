#ifndef DEMANGLER_H
#define DEMANGLER_H

#include "macho_global.h"
#include <boost/process.hpp>

class Demangler
{
public:
    Demangler();
    virtual ~Demangler();

    string demangleName(const string& name);
private:
	boost::process::child* child;
	boost::process::pistream* stdout; 
	boost::process::postream* stdin;
	bool isRunning;
	
	void init();
private:
};

#endif // DEMANGLER_H
