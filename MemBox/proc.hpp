#pragma once
#include "pch.h"

namespace MemBox
{
	class Process
	{
	public:
		Process(ulong desiredAccess, ulong procId);
		Process(ulong desiredAccess, const std::wstring& procName);
		~Process();

		uintptr_t GetModuleBaseAddress(const std::wstring& modName) const;
		void* GetHandle() const noexcept { return handle; }
		ulong GetId() const noexcept { return procId; }
	private:
		void* handle;
		ulong procId;

		void AttachProcess(ulong desiredAccess);
		static ulong GetProcId(const std::wstring& procName);
	};
}
