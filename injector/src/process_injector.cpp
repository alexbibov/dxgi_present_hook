#include "process_injector.h"
#include "dll_injection_binary.h"
#include "load_library_r.h"

namespace Injector {

ProcessInjector::ProcessInjector(DWORD process_id)
    : m_process_id{ process_id }
    , m_snitch_code_size{ getInjectionCodeSize() }
    , m_snitch_code_ptr{getInjectionCode()}
    , m_victim_process_handle{NULL}
{
    m_victim_process_handle = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, false, process_id);
}

bool ProcessInjector::injectSnitchCode() const
{
    if (!m_victim_process_handle) return false;

    SIZE_T bytes_written{};
    HANDLE injection_handle = LoadRemoteLibraryR(m_victim_process_handle, const_cast<void*>(m_snitch_code_ptr), m_snitch_code_size, NULL);
    if (!injection_handle) return false;

    WaitForSingleObject(injection_handle, -1);

    return true;
}

}