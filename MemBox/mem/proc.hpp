#pragma once
#include <optional>

namespace MemBox
{
	class Process
	{
	public:
		~Process();

		/// <summary>
		/// Attach to a running process
		/// </summary>
		/// <param name="desiredAccess"></param>
		/// <param name="procId"></param>
		/// <returns></returns>
		static Process Attach(ulong desiredAccess, ulong procId);

		/// <summary>
		/// Attach to a running process
		/// </summary>
		/// <param name="desiredAccess"></param>
		/// <param name="procName"></param>
		/// <returns></returns>
		static Process Attach(ulong desiredAccess, const std::wstring& procName);

		/// <summary>
		/// Gets the base address of a module in the attached process
		/// </summary>
		/// <param name="modName"></param>
		/// <returns></returns>
		uintptr_t GetModuleBaseAddress(const std::wstring& modName) const;

		/// <summary>
		/// Get process handle associated with this object
		/// </summary>
		/// <returns></returns>
		void* GetHandle() const noexcept { return handle; }

		/// <summary>
		/// Get pid of attached process
		/// </summary>
		/// <returns></returns>
		ulong GetId() const noexcept { return procId; }
	private:
		void* handle;
		ulong procId;

		Process(ulong desiredAccess, ulong procId);
		Process(ulong desiredAccess, const std::wstring& procName);

		void AttachProcess(ulong desiredAccess);
		static ulong GetProcId(const std::wstring& procName);
	};
}
