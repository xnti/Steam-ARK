#pragma once
uint64_t GetProcessBase(int pid);
uint64_t GetProcessId(const char* process_name);
uint64_t GetProcessId();
HWND getHWND(DWORD dwProcessId);
bool read_raw(int pid, uint64_t address, void* buffer, size_t size);
bool write_raw(int pid, uint64_t address, void* buffer, size_t size);
template<typename T>
inline bool read(uint64_t address, T& value)
 {
		return read_raw(GetProcessId(), address, &value, sizeof(T));
}
template<typename T>
inline bool write(uint64_t address, const T& value) {
	return write_raw(GetProcessId(), address, (void*)&value, sizeof(T));
}
template<typename T>
inline bool read_array(uint64_t address, T* array, size_t len) {
	return read_raw(GetProcessId(), address, array, sizeof(T) * len);
}
template<typename T>
inline T read(uint64_t address) 
{
	T buffer;

	if (address < 0xFFFFFF)
		return T();
	if (address > 0x7FFFFFFF0000)
		return T();

	read(address, buffer);
	return buffer;
}
template<typename T>
std::string read_string(uint64_t address)
{
	char buffer[70];

	if (address < 0xFFFFFF)
		return std::string();
	if (address > 0x7FFFFFFF0000)
		return std::string();

	read_array(address, &buffer, sizeof(buffer));

	std::string nameString;
	for (int i = 0; i < 70; i += 2) {
		if (buffer[i] == 0)
			break;
		else
			nameString += buffer[i];
	};

	return nameString;
}
