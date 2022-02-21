#ifndef INJECTOR_PROCESS_INJECTOR_H
#define INJECTOR_PROCESS_INJECTOR_H

#include <Windows.h>

namespace Injector {

class ProcessInjector {
public:
    explicit ProcessInjector(DWORD process_id);
    bool injectSnitchCode() const;

private:
    DWORD m_process_id;
    HANDLE m_victim_process_handle;
    size_t m_snitch_code_size;
    void const* m_snitch_code_ptr;
};

}

#endif