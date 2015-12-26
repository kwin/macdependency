#include "demangler.h"
#include "machodemangleexception.h"

using namespace boost::process;
/**
 class for using c++flt to demangle names. Uses Boost.Process from http://www.highscore.de/cpp/process/index.html
 */
Demangler::Demangler() : child(NULL), isRunning(false)
{
	init();
}

Demangler::~Demangler()
{
	if (child)
		child->terminate();
	delete child;
}

string Demangler::demangleName(const string& name) {
	if (isRunning){
		(*stdin) << name << endl;
		string line;
		getline(*stdout, line);
		return line;
	} else {
		throw MachODemangleException("Could not find/start process c++flt.");
	}
}

void Demangler::init() {
	try {
	std::string exec = find_executable_in_path("c++filt"); 
	std::vector<std::string> args;
	args.push_back("--strip-underscore"); 
	context ctx; 
	ctx.environment = self::get_environment(); 
	ctx.stdout_behavior = capture_stream(); 
	ctx.stdin_behavior = capture_stream(); 
	child = new boost::process::child(launch(exec, args, ctx)); 
	stdout = &child->get_stdout(); 
	stdin = &child->get_stdin();
	isRunning = true;
	// TODO: check exceptions
	} catch (boost::filesystem::filesystem_error& e) {
		// errors during finding executable
	} catch (boost::system::system_error& e2) {
		// errors during starting of process
	}
}