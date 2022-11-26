#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <iostream>
#include <dwmapi.h>
#include <d3d9.h>
#include <string>
#include "Mhyprot/baseadress.h"
#include "offset.h"
#include "cfg.h"


#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")

class text
{
public:
	char word[1024];
};

class textx
{
public:
	wchar_t word[64];
};

template<class T>
class TArray
{
public:
	int Length() const
	{
		return m_nCount;
	}

	bool IsValid() const
	{
		if (m_nCount > m_nMax)
			return false;
		if (!m_Data)
			return false;
		return true;
	}

	uint64_t GetAddress() const
	{
		return m_Data;
	}

	T GetById(int i)
	{
		return read<T>(m_Data + i * 8);
	}

protected:
	uint64_t m_Data;
	uint32_t m_nCount;
	uint32_t m_nMax;
};

struct FString : private TArray<wchar_t>
{
	std::wstring ToWString() const
	{
		wchar_t* buffer = new wchar_t[m_nCount];
		read_array(m_Data, buffer, m_nCount);
		std::wstring ws(buffer);
		delete[] buffer;

		return ws;
	}

	std::string ToString() const
	{
		std::wstring ws = ToWString();
		std::string str(ws.begin(), ws.end());

		if (str.empty())
		{
			return "";
		}
		return str;
	}

};

struct FLinearColor
{
public:
	float                                                      R;                                                       // 0x0000(0x0004) Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor
	float                                                      G;                                                       // 0x0004(0x0004) Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor
	float                                                      B;                                                       // 0x0008(0x0004) Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor
	float                                                      A;                                                       // 0x000C(0x0004) Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor
};

struct FPrimalPlayerCharacterConfigStructReplicated
{
public:
	unsigned char                                              bIsFemale : 1;                                           // 0x0000(0x0001) BIT_FIELD NoDestructor
	unsigned char                                              UnknownData_GC3L[0x3];                                   // 0x0001(0x0003) MISSED OFFSET (FIX SPACE BETWEEN PREVIOUS PROPERTY)
	struct FLinearColor                                        BodyColors[0x4];                                         // 0x0004(0x0040) ZeroConstructor, IsPlainOldData, NoDestructor
	unsigned char                                              UnknownData_E9I7[0x4];                                   // 0x0044(0x0004) MISSED OFFSET (FIX SPACE BETWEEN PREVIOUS PROPERTY)
	class FString                                              PlayerCharacterName;                                     // 0x0048(0x0010) ZeroConstructor
	float                                                      RawBoneModifiers[0x16];                                  // 0x0058(0x0058) ZeroConstructor, IsPlainOldData, NoDestructor
	int32_t                                                    PlayerSpawnRegionIndex;                                  // 0x00B0(0x0004) ZeroConstructor, IsPlainOldData, NoDestructor
	unsigned char                                              UnknownData_UL7Z[0x4];                                   // 0x00B4(0x0004) MISSED OFFSET (PADDING)
};

std::string RandomString(int len)
{
	srand(time(NULL));
	std::string str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::string newstr;
	int pos;
	while (newstr.size() != len) {
		pos = ((rand() % (str.size() - 1)));
		newstr += str.substr(pos, 1);
	}
	return newstr;
}
DWORD GetVisibleColor(bool bVisible)
{

	ImColor Color;
	if (bVisible)
		Color = CFG.VisibleColor;
	else
		Color = CFG.InvisibleColor;

	return Color;
}
void move_to(float x, float y)
{
	float center_x = GameVars.ScreenWidth / 2;
	float center_y = GameVars.ScreenHeight / 2;

	float smooth = CFG.Smoothing;

	float target_x = 0.f;
	float target_y = 0.f;

	if (x != 0.f)
	{
		if (x > center_x)
		{
			target_x = -(center_x - x);
			target_x /= smooth;
			if (target_x + center_x > center_x * 2.f) target_x = 0.f;
		}

		if (x < center_x)
		{
			target_x = x - center_x;
			target_x /= smooth;
			if (target_x + center_x < 0.f) target_x = 0.f;
		}
	}

	if (y != 0.f)
	{
		if (y > center_y)
		{
			target_y = -(center_y - y);
			target_y /= smooth;
			if (target_y + center_y > center_y * 2.f) target_y = 0.f;
		}

		if (y < center_y)
		{
			target_y = y - center_y;
			target_y /= smooth;
			if (target_y + center_y < 0.f) target_y = 0.f;
		}
	}

	mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(target_x), static_cast<DWORD>(target_y), 0, 0);
}

std::string getNameFromID(int ID) {

	try {
		auto GName_PTR = read<uintptr_t>(GameVars.dwProcess_Base + GameOffset.offset_g_names);
		DWORD_PTR fNamePtr = read<DWORD_PTR>(GName_PTR + int(ID / 0x4000) * 0x8);
		DWORD_PTR fName = read<DWORD_PTR>(fNamePtr + 0x8 * int(ID % 0x4000));
		char buff[256];
		read_array(fName + 0x10, buff, 256);
		return buff;
	}
	catch (int e) { return std::string(""); }

}


D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
	D3DMATRIX pOut;
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return pOut;
}
FTransform GetBoneIndex(DWORD_PTR mesh, int index)
{
	DWORD_PTR bonearray = read<DWORD_PTR>(mesh + GameOffset.offset_bone_array);
	//if(!bonearray)
	//	bonearray = read<DWORD_PTR>(mesh + GameOffset.offset_bone_array + 0x30);
	//if (bonearray > 0x7FFFFFFF) {
	//	Vector3(0, 0, 0);
	//}
	//else
	return read<FTransform>(bonearray + (index * 0x30));
}
bool isVisible(DWORD_PTR mesh)
{
	float fLastSubmitTime = read<float>(mesh + GameOffset.offset_last_submit_time);
	float fLastRenderTimeOnScreen = read<float>(mesh + GameOffset.offset_last_render_time);
	const float fVisionTick = 0.06f;
	bool bVisible = fLastRenderTimeOnScreen + fVisionTick >= fLastSubmitTime;
	return bVisible;
}
Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id)
{
	FTransform bone = GetBoneIndex(mesh, id);
	FTransform ComponentToWorld = read<FTransform>(mesh + GameOffset.offset_component_to_world);
	D3DMATRIX Matrix;
	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());
	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}

D3DXMATRIX Matrix(Vector3 rot, Vector3 origin)
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}
struct FMinimalViewInfo
{
	Vector3 Location; // 0x00(0x0c)
	Vector3 Rotation; // 0x0c(0x0c)
	Vector3 AimRotation; // 0x18(0xC)
	char pad; // 0x24(0x04)
	float FOV; // 0x28(0x04)
};

struct FCameraCacheEntry
{
	float TimeStamp; // 0x00(0x04)
	char pad1[0x4]; // 0x04(0x04)
	FMinimalViewInfo POV; // 0x08(0x0398)
};
Vector3 ProjectWorldToScreen(Vector3 WorldLocation)
{
	Vector3 Screenlocation = Vector3(0, 0, 0);

	auto CameraCache = read<FCameraCacheEntry>(GameVars.CameraManager + GameOffset.offset_camera_cache);

	D3DMATRIX tempMatrix = Matrix(CameraCache.POV.Rotation, Vector3(0, 0, 0));

	Vector3 vAxisX, vAxisY, vAxisZ;

	vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = WorldLocation - CameraCache.POV.Location;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	float FovAngle = CameraCache.POV.FOV;

	float ScreenCenterX = GameVars.ScreenWidth / 2.0f;
	float ScreenCenterY = GameVars.ScreenHeight / 2.0f;

	Screenlocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
	Screenlocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;

	return Screenlocation;
}
/*
BOOL CheckMask(PCHAR base, PCHAR pattern, PCHAR mask) {
	for (; *mask; ++base, ++pattern, ++mask) {
		if (*mask == 'x' && *base != *pattern) {
			return FALSE;
		}
	}

	return TRUE;
}
PVOID FindPattern(uint64_t base, DWORD64 length, PCHAR pattern, PCHAR mask) {
	length -= (DWORD)strlen(mask);
	auto buffer = new char[length];
	read_array(base, buffer, length);
	for (DWORD i = 0; i <= length; ++i) {
		PVOID addr = &buffer[i];
		if (CheckMask((PCHAR)addr, pattern, mask)) {
			delete[] buffer;
			return addr;
		}
	}
	delete[] buffer;
	return 0;
}
#define IMAGE_FIRST_SECTION_CUSTOM( ntheader, pntheader ) ((PIMAGE_SECTION_HEADER)        \
    ((ULONG_PTR)(pntheader) +                                            \
     FIELD_OFFSET( IMAGE_NT_HEADERS, OptionalHeader ) +                 \
     ((ntheader)).FileHeader.SizeOfOptionalHeader   \
    ))
PVOID FindPatternImage(uintptr_t base, PCHAR pattern, PCHAR mask) {
	PVOID match = 0;

	IMAGE_DOS_HEADER dos_header = { 0 };
	IMAGE_NT_HEADERS64 nt_headers = { 0 };

	read_array(base, &dos_header, sizeof(dos_header));

	if (dos_header.e_magic != IMAGE_DOS_SIGNATURE)
		return 0;

	read_array(base + dos_header.e_lfanew, &nt_headers, sizeof(nt_headers));

	if (nt_headers.Signature != IMAGE_NT_SIGNATURE)
		return 0;

	for (DWORD i = 0; i < nt_headers.FileHeader.NumberOfSections; ++i) {
		IMAGE_SECTION_HEADER section = { 0 };
		read_array((base + dos_header.e_lfanew) + (i * sizeof(IMAGE_SECTION_HEADER)), &section, sizeof(section));
		if (*(PINT)section.Name == 'EGAP' || memcmp(section.Name, ".text", 5) == 0) {
			match = FindPattern(base + section.VirtualAddress, section.Misc.VirtualSize, pattern, mask);
			if (match) {
				break;
			}
		}
	}

	return match;
}
*/

