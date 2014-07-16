#pragma once

#include <iostream>
#include <vector>
#include <stdexcept>
#include <Windows.h>
#include <TlHelp32.h>

class Process
{
public:

	//Process(HANDLE hProcess);
	Process(DWORD processID);
	Process(const Process& instance);
	Process& operator=(const Process& instance);
	~Process();
	
	LPVOID allocMem(DWORD size) const;
	LPVOID allocMem(DWORD size, DWORD allocationType) const;
	LPVOID allocMem(DWORD size, LPVOID desiredAddress, DWORD allocationType) const;
	bool freeMem(LPVOID address) const;
	void writeMemory(LPVOID address, LPCVOID data, DWORD size) const;
	void readMemory(LPVOID address, LPVOID buffer, DWORD size) const;
	MEMORY_BASIC_INFORMATION queryMemory(LPVOID address) const;
	DWORD protectMemory(LPVOID address, SIZE_T size, DWORD protect) const;
	bool startThread(LPVOID address, LPVOID param);
	void waitForThread();
	std::vector<MODULEENTRY32> getModules() const;
	
	uintptr_t getImageBase(HANDLE hThread) const;
	uintptr_t getImageBase() const;

private:

	bool duplicateHandle(HANDLE hSrc, HANDLE* hDest);
	void throwSysError(const char* msg, DWORD lastError) const;

	HANDLE hProcess_;
	HANDLE hThread_;
	DWORD processID_;
};

// handle error
class ProcessHandleException : public std::runtime_error
{
public:
	ProcessHandleException(const std::string& msg) : std::runtime_error(msg) {};
};

// anything with memory
class ProcessMemoryException : public std::runtime_error
{
public:
	ProcessMemoryException(const std::string& msg, LPVOID address) : std::runtime_error(msg), address_(address) {};
	LPVOID getAddress() { return address_; };
private:
	LPVOID address_;
};

// access memory
class MemoryAccessException : public std::runtime_error
{
public:
	MemoryAccessException(const std::string& msg) : std::runtime_error(msg) {};
};

// allocate
class MemoryAllocationException : public std::runtime_error
{
public:
	MemoryAllocationException(const std::string& msg) : std::runtime_error(msg) {};
};

// query memory
class MemoryQueryException : public std::runtime_error
{
public:
	MemoryQueryException(const std::string& msg) : std::runtime_error(msg) {};
};

// protect memory
class MemoryProtectException : public ProcessMemoryException
{
public:
	MemoryProtectException(const std::string& msg, LPVOID address) : ProcessMemoryException(msg, address) {};
};
