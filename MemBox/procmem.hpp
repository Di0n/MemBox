#pragma once
#include "pch.h"
#include "proc.hpp"
#include <expected>

namespace MemBox
{
	class ProcessMemory
	{
	public:
		ProcessMemory(const Process& proc) : process(proc) {}
		~ProcessMemory() {}
		/// <summary>
		/// Resolves a dynamic memory address using a base pointer and a list of offsets.
		/// </summary>
		/// <param name="ptr"></param>
		/// <param name="offsets"></param>
		/// <returns>Returns an std::expected with either the address or an error code</returns>
		std::expected<uintptr_t, int> FindDynamicAddress(uintptr_t ptr, const std::vector<uint>& offsets);

		/// <summary>
		/// Reads data from a memory address into a buffer
		/// </summary>
		/// <param name="address"></param>
		/// <param name="source"></param>
		/// <param name="size"></param>
		/// <returns>Returns an std::expected with either the read size or an error code</returns>
		std::expected<size_t, int> Read(uintptr_t address, void* source, size_t size);

		/// <summary>
		/// Writes data buffer to specified memory address
		/// </summary>
		/// <param name="address"></param>
		/// <param name="source"></param>
		/// <param name="size"></param>
		/// <returns>Returns 0 on success, otherwise returns error code.</returns>
		int Write(uintptr_t address, const void* source, size_t size);

		/// <summary>
		/// NOPS a memory region
		/// </summary>
		/// <param name="address"></param>
		/// <param name="size"></param>
		/// <returns>Returns 0 on success, otherwise returns error code.</returns>
		int WriteNOPInstruction(uintptr_t address, size_t size);

		/// <summary>
		/// Reads a value of type T from the specified memory address in another process.
		/// </summary>
		/// <typeparam name="T">The type of the value to read from memory.</typeparam>
		/// <param name="address">The memory address to read from.</param>
		/// <param name="val">Reference to a variable where the read value will be stored.</param>
		/// <returns>Returns 0 on success, or an error code if the read operation fails.</returns>
		template<typename T>
		int Read(uintptr_t address, T& val)
		{
#ifdef _WIN32
			SIZE_T bytesRead = 0;
			BOOL success = ReadProcessMemory(process.GetHandle(), reinterpret_cast<LPCVOID>(address), &val, sizeof(T), &bytesRead);
			if (!success || bytesRead != sizeof(T))
			{
				return static_cast<int>(GetLastError());
			}
#elif __linux__
			// Linux implementation todo
#endif
			return 0;
		}

		/// <summary>
		/// Writes a value of type T to the specified memory address.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="address"></param>
		/// <param name="val"></param>
		/// <returns>Returns 0 on success, otherwise returns error code.</returns>
		template<typename T>
		int Write(uintptr_t address, const T& val)
		{

#ifdef _WIN32
			SIZE_T bytesWritten = 0;
			DWORD oldprotect;
			LPVOID addr = reinterpret_cast<LPVOID>(address);
			const auto size = sizeof(T);
			
			VirtualProtectEx(process.GetHandle(), addr, size, PAGE_EXECUTE_READWRITE, &oldprotect);
			BOOL success = WriteProcessMemory(
				process.GetHandle(),
				addr,
				&val,
				size,
				&bytesWritten
			);
			VirtualProtectEx(process.GetHandle(), addr, size, oldprotect, &oldprotect);

			if (!success || bytesWritten != sizeof(T))
			{
				return static_cast<int>(GetLastError());
			}
		#elif __linux__
			// Linux implementation todo
		#endif

			return 0;
		}

	private:
		const Process& process;
	};
}

