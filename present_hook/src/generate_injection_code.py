import argparse
import pathlib
import struct
import os
from string import Template

argument_parser = argparse.ArgumentParser(description="Creates C++ code containing binary representation of injected DLL")
argument_parser.add_argument('-s', "--source", type=pathlib.Path, 
							 help="Path to DLL, for which to create the injection code", 
							 dest="source", required=True)
argument_parser.add_argument('-o', "--output-directory", type=pathlib.Path, 
							 help="Directory where the injection code source will be written", 
							 dest="output_directory", required=True)
argument_parser.add_argument('-n', "--name", type=str, 
							 help="User-friendly name of the injection code sources. This argument defines the name of the generated *.cpp and *.h files", 
							 dest="name", required=True)

arguments = argument_parser.parse_args()

if arguments.source.is_file() is False:
	raise RuntimeError(f'provided source "{arguments.source.as_posix()}" is not a file')
if arguments.output_directory.is_dir() is False:
	raise RuntimeError(f'provided destination path "arguments.output_directory.as_posix()" must be a directory')

dll_size = os.stat(arguments.source.as_posix()).st_size
with open(arguments.source.as_posix(), 'rb') as f:
	dll_data = f.read()

injection_code_cpp = """// This source code contains binary data to be injected into the host process. The data below has been created by an automated system, DO NOT EDIT
#include "${injection_code_source_name}.h"

namespace{

uint64_t injection_code_bin[] = {$injection_code_bin_data};
size_t injection_code_bin_size = sizeof(injection_code_bin);

}

namespace Injector{
size_t getInjectionCodeSize(){ return injection_code_bin_size; }
uint64_t const* getInjectionCode() { return injection_code_bin; }
}

"""

injection_code_header = """// This source code contains binary data to be injected into the host process. The data below has been created by an automated system, DO NOT EDIT

#include <cstdint>

namespace Injector{

size_t getInjectionCodeSize();
uint64_t const* getInjectionCode();

}

"""

bytes_read = 0
uint64_t_elements_read = 0
parsed_data = ""
while bytes_read < dll_size:
	value = struct.unpack('<Q', dll_data[bytes_read:bytes_read + 8])[0]
	bytes_read += 8
	uint64_t_elements_read += 1
	parsed_data += f"{hex(value)}, "

	if uint64_t_elements_read % 16 == 0:
		parsed_data += "\n\t"

parsed_data = parsed_data.rstrip("\n\t, ")

(arguments.output_directory / "src").mkdir(parents=True, exist_ok=True)
(arguments.output_directory / "include").mkdir(parents=True, exist_ok=True)

with open((arguments.output_directory / f"src/{arguments.name}.cpp"), 'w') as f:
	f.write(Template(injection_code_cpp).substitute(injection_code_source_name=arguments.name, injection_code_bin_data=parsed_data))

with open((arguments.output_directory / f"include/{arguments.name}.h"), 'w') as f:
	f.write(injection_code_header)
	