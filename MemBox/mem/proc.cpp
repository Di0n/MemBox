#include "pch.h"
#include "proc.hpp"
#include "exceptions/process_exceptions.hpp"
#include "utils/stringutils.hpp"
#include <stdexcept>
#ifdef _WIN32
#include <TlHelp32.h>
#elif __linux__
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <cctype>
#endif

namespace MemBox
{
	using std::wstring;

    Process Process::Attach(ulong desiredAccess, ulong procId)
	{
		return Process(desiredAccess, procId);
	}

	Process Process::Attach(ulong desiredAccess, const std::wstring& procName)
	{
		return Process(desiredAccess, procName);
	}

	Process::Process(ulong desiredAccess, ulong procId)
	{
		this->procId = procId;

		AttachProcess(desiredAccess);
	}

	Process::Process(ulong desiredAccess, const wstring& procName)
	{
		this->procId = GetProcId(procName);

		if (procId == 0)
		{
			throw ProcessNotFoundException(StringUtils::WideToUTF8(procName));
		}

		AttachProcess(desiredAccess);
	}

	Process::~Process()
	{
#ifdef _WIN32
		if (handle != NULL)
			CloseHandle(handle);
#endif
	}

	void Process::AttachProcess(ulong desiredAccess)
	{
#ifdef _WIN32
		handle = OpenProcess(desiredAccess, FALSE, procId);
		if (handle == NULL)
		{
			ulong err = GetLastError();
			throw ProcessAttachException(std::to_string(procId), std::to_string(err));
		}
#elif __linux__
		// No need to attach on linux for reading/writing memory with process_vm_readv/writev
#endif
	}

	uintptr_t Process::GetModuleBaseAddress(const wstring& modName) const
	{
		uintptr_t modBaseAddr = 0;

#ifdef _WIN32
		
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
		if (hSnap != INVALID_HANDLE_VALUE)
		{
			MODULEENTRY32 modEntry;
			modEntry.dwSize = sizeof(modEntry);
			if (Module32First(hSnap, &modEntry))
			{
				do
				{
					if (!_wcsicmp(modEntry.szModule, modName.c_str()))
					{
						modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
						break;
					}
				} while (Module32Next(hSnap, &modEntry));
			}
		}
		CloseHandle(hSnap);
#elif __linux__
		std::string mapsPath = "/proc/" + std::to_string(procId) + "/maps";
		std::ifstream mapsFile(mapsPath);

		if (!mapsFile.is_open()) {
			return 0;
		}

		// Convert wide string to narrow for comparison
		std::string narrowModName(modName.begin(), modName.end());

		std::string line;
		while (std::getline(mapsFile, line)) {
			// Maps file format: address perms offset dev inode pathname
			std::istringstream iss(line);
			std::string addrRange;
			iss >> addrRange; // Get address range

			// Skip to the pathname
			std::string temp;
			for (int i = 0; i < 5; ++i) iss >> temp;

			std::string pathname;
			std::getline(iss >> std::ws, pathname); // Get the rest of the line as pathname

			// Extract module name from pathname
			size_t pos = pathname.find_last_of('/');
			std::string moduleName = (pos != std::string::npos) ?
				pathname.substr(pos + 1) : pathname;

			if (moduleName == narrowModName) {
				// Parse the base address from address range (format: start-end)
				pos = addrRange.find('-');
				if (pos != std::string::npos) {
					std::string baseAddr = addrRange.substr(0, pos);
					modBaseAddr = std::stoull(baseAddr, nullptr, 16);
					break;
				}
			}
		}
#endif
		return modBaseAddr;
	}

	ulong Process::GetProcId(const wstring& procName)
	{
		ulong procID = 0;

#ifdef _WIN32
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnap != INVALID_HANDLE_VALUE)
		{
			PROCESSENTRY32 procEntry;
			procEntry.dwSize = sizeof(PROCESSENTRY32);

			if (Process32First(hSnap, &procEntry))
			{
				do
				{
					if (!_wcsicmp(procEntry.szExeFile, procName.c_str()))
					{
						procID = procEntry.th32ProcessID;
						break;
					}
				} while (Process32Next(hSnap, &procEntry));
			}
		}
		CloseHandle(hSnap);
#elif __linux__
		DIR* dir = opendir("/proc");
		if (!dir) return 0;

		struct dirent* entry;
		while ((entry = readdir(dir)) != nullptr) {
			// Check if the entry is a directory and contains only digits (process ID)
			if (entry->d_type == DT_DIR && std::isdigit(entry->d_name[0])) {
				std::string cmdlinePath = "/proc/" + std::string(entry->d_name) + "/cmdline";
				std::ifstream cmdlineFile(cmdlinePath);

				if (cmdlineFile.is_open()) {
					std::string cmdline;
					std::getline(cmdlineFile, cmdline, '\0');

					// Extract executable name from path
					size_t pos = cmdline.find_last_of('/');
					std::string exeName = (pos != std::string::npos) ?
						cmdline.substr(pos + 1) : cmdline;

					// Convert wide string to narrow for comparison
					std::string narrowProcName(procName.begin(), procName.end());

					if (exeName == narrowProcName) {
						procID = std::stoul(entry->d_name);
						break;
					}
				}
			}
		}
		closedir(dir);
#endif

		return procID;
	}
}
