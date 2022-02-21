#include <vector>
#include <filesystem>
#include "process_id_snooper.h"
#include <TlHelp32.h>

namespace Injector {

namespace {

std::vector<std::filesystem::path> enumerateModules(DWORD process_id)
{
    std::vector<std::filesystem::path> rv{};
    HANDLE process_level_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, process_id);

    if (process_level_snapshot != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 module_entry{sizeof(MODULEENTRY32)};
        BOOL retrieved_module = Module32First(process_level_snapshot, &module_entry);
        while (retrieved_module) {
            rv.push_back(std::filesystem::path{ module_entry.szExePath });
            retrieved_module = Module32Next(process_level_snapshot, &module_entry);
        }
    }
    
    return rv;
}

}

ProcessIdSnooper::ProcessIdSnooper(std::string const& name_of_executable)
{
    HANDLE system_level_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (system_level_snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 process_entry{sizeof(PROCESSENTRY32)};
        BOOL retrieved_process = Process32First(system_level_snapshot, &process_entry);
        while (retrieved_process) {
            DWORD process_id = process_entry.th32ProcessID;
            auto modules_exe_path_list = enumerateModules(process_id);

            for (auto const& p : modules_exe_path_list) {
                if (p.has_filename() && p.filename() == name_of_executable)
                {
                    m_process_id = process_id;
                    break;
                }
            }

            if (m_process_id.has_value()) {
                break;
            }

            retrieved_process = Process32Next(system_level_snapshot, &process_entry);
        }
    }
}

}