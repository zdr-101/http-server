#pragma once

#include <iostream>
#include <Windows.h>
#include <cstring>

using namespace std;

class utils
{
public:
	void initialize();

public:
	HINSTANCE sdtapi_dll;

private:
	typedef int(__stdcall* INIT_COMM)(int iPort);
	typedef int(__stdcall* CLOSE_COMM)();
	typedef int(__stdcall* AUTHENTICATE)();
	typedef int(__stdcall* READ_BASE_INFOS)(char* Name, char* Gender, char* Folk, char* BirthDay, char* Code, char* Address, char* Agency, char* ExpireStart, char* ExpireEnd);
	typedef int(__stdcall* GET_SAMID_TO_STR)(char* pcSAMID);

public:
	INIT_COMM _init_comm;
	CLOSE_COMM _close_comm;
	AUTHENTICATE _authenticate;
	READ_BASE_INFOS _read_base_infos;
	//∂¡»°sm¬Î
	GET_SAMID_TO_STR _get_samid_to_str;
};

