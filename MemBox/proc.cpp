#include "pch.h"
#include "proc.hpp"
#include <TlHelp32.h>
#include <stdexcept>

namespace MemBox
{
	using std::wstring;
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
			throw std::runtime_error("Process not found.");
		}

		AttachProcess(desiredAccess);
	}
	Process::~Process()
	{
#ifdef _WIN32
		if (handle != NULL)
			CloseHandle(handle);
#elif __linux__
		if (handle != NULL)
			ptrace(PTRACE_DETACH, procId, NULL, NULL);
#endif
	}

	void Process::AttachProcess(ulong desiredAccess)
	{
#ifdef _WIN32
		handle = OpenProcess(desiredAccess, FALSE, procId);
		if (handle == NULL)
		{
			ulong err = GetLastError();
			//Log(LogLevel::Error, "Failed to attach process. Error code: " + std::to_string(err));
			throw std::runtime_error("Failed to attach process. Error code: " + std::to_string(err));
		}
#elif __linux__
		// Linux implementation todo
		if (ptrace(PTRACE_ATTACH, procId, NULL, NULL) == -1)
			throw std::runtime_error("Failed to attach process. Error code: " + std::to_string(errno));
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
		// linux impl
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
	// linux impl
#endif

		return procID;
	}
}
