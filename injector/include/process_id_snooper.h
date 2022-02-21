#ifndef INJECTOR_PROCESS_ID_SNOOPER_H
#define INJECTOR_PROCESS_ID_SNOOPER_H

#include <string>
#include <optional>
#include <Windows.h>

namespace Injector {

class ProcessIdSnooper {
public:
    explicit ProcessIdSnooper(std::string const& name_of_executable);
    std::optional<DWORD> getProcessId() const { return m_process_id; }

private:
    std::optional<DWORD> m_process_id;
};

}

#endif