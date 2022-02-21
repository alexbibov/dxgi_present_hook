#include <cstdint>
#include <string>
#include <iostream>
#include <windows.h>
#include <tclap/CmdLine.h>
#include "process_id_snooper.h"
#include "process_injector.h"


int main(int argc, char* argv[])
{
	try {
		TCLAP::CmdLine command_line_parser("This is a helper executable that assists injection of hijacking code into the process", ' ', "0.1a");
		TCLAP::ValueArg victim_process_executable_name_arg{ "n", "target-process-executable-name", "Name of the executable of the victim process", true, std::string{}, "string value" };
		command_line_parser.add(victim_process_executable_name_arg);

		command_line_parser.parse(argc, argv);
		std::string victim_process_executable = victim_process_executable_name_arg.getValue();

		std::optional<DWORD> snoopedProcessId = Injector::ProcessIdSnooper{ victim_process_executable }.getProcessId();
		if (!snoopedProcessId.has_value())
		{
			std::cout << "Unable to retrieve process ID for given executable name '" << victim_process_executable << "'. The program will terminate." << std::endl;
			return EXIT_FAILURE;
		}

		Injector::ProcessInjector process_injector{ snoopedProcessId.value() };
		if (!process_injector.injectSnitchCode())
		{
			std::cout << "Code injection into the process failed. The program will terminate." << std::endl;
		}

	}
	catch (TCLAP::ArgException& e)
	{
		std::cout << e.error() << " for argument " << e.argId() << std::endl;
	}

	return EXIT_SUCCESS;
}