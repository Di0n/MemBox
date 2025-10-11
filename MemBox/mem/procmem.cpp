#include "pch.h"
#include "procmem.hpp"

namespace MemBox 
{
	std::expected<size_t, int> ProcessMemory::Read(uintptr_t address, void* source, size_t size)
	{
		#ifdef _WIN32
		size_t bytesRead = 0;
		BOOL success = ReadProcessMemory(process.GetHandle(), reinterpret_cast<LPCVOID>(address), source, size, &bytesRead);
		if (!success)
		{
			return std::unexpected(static_cast<int>(GetLastError()));
		}
		
		return bytesRead;
#elif __linux__
		// Linux implementation todo

		struct iovec local { .iov_base = source, .iov_len = size };
		struct iovec remote { .iov_base = reinterpret_cast<void*>(address), .iov_len = size };

		ssize_t nread = process_vm_readv(process.GetId(), &local, 1, &remote, 1, 0);

		if (nread == -1 || nread != static_cast<ssize_t>(size))
		{
			// Couldn't peek data
			return std::unexpected(errno);
		}

		return static_cast<ssize_t>(nread);
#endif
	}

	int ProcessMemory::Write(uintptr_t address, const void* source, size_t size)
	{
#ifdef _WIN32
		SIZE_T bytesWritten = 0;
		DWORD oldprotect;
		LPVOID addr = reinterpret_cast<LPVOID>(address);

		VirtualProtectEx(process.GetHandle(), addr, size, PAGE_EXECUTE_READWRITE, &oldprotect);
		BOOL success = WriteProcessMemory(
			process.GetHandle(),
			addr,
			source,
			size,
			&bytesWritten
		);
		VirtualProtectEx(process.GetHandle(), addr, size, oldprotect, &oldprotect);

		if (!success || bytesWritten != size)
		{
			return static_cast<int>(GetLastError());
		}
#elif __linux__
		// Linux implementation todo
		struct iovec local 
		{
			.iov_base = const_cast<void*>(source),
			.iov_len = size
		};
		struct iovec remote 
		{
			.iov_base = reinterpret_cast<void*>(address),
			.iov_len = size
		};

		ssize_t nwritten = process_vm_writev(pid, &local, 1, &remote, 1, 0);
		if (nwritten != static_cast<ssize_t>(size))
			return errno;
#endif

		return 0;
	}

	int ProcessMemory::WriteNOPInstruction(uintptr_t address, size_t size)
	{
		std::vector<std::uint8_t> nopArray(size, 0x90); // 0x90 = no operation instruction
		return Write(address, nopArray.data(), nopArray.size());
	}

	std::expected<uintptr_t, int> ProcessMemory::FindDynamicAddress(uintptr_t ptr, const std::vector<uint>& offsets)
	{
		uintptr_t addr = ptr;
		for (uint offset : offsets) 
		{
			size_t bytesRead = 0;
			auto result = Read(addr, &addr, sizeof(addr));
			
			if (!result)
				return result;
			
			addr += offset;
		}
		return addr;
	}
}
