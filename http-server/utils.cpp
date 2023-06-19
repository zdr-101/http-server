#include "utils.h"

void utils::initialize()
{
	sdtapi_dll = nullptr;
	_init_comm = nullptr;
	_close_comm = nullptr;
	_authenticate = nullptr;
	_read_base_infos = nullptr;
	_get_samid_to_str = nullptr;

	sdtapi_dll = LoadLibrary("utils/Sdtapi.dll");
	if (nullptr == sdtapi_dll)
	{
		cout << "sdtapi_dll error" << endl;
		return;
	}

	_init_comm = (INIT_COMM)GetProcAddress(sdtapi_dll, "InitComm");
	if (nullptr == _init_comm)
	{
		cout << "_init_comm error" << endl;
		return;
	}

	_close_comm = (CLOSE_COMM)GetProcAddress(sdtapi_dll, "CloseComm");
	if (nullptr == _close_comm)
	{
		cout << "_close_comm error" << endl;
		return;
	}

	_authenticate = (AUTHENTICATE)GetProcAddress(sdtapi_dll, "Authenticate");
	if (nullptr == _authenticate)
	{
		cout << "_authenticate error" << endl;
		return;
	}
	_read_base_infos = (READ_BASE_INFOS)GetProcAddress(sdtapi_dll, "ReadBaseInfos");
	if (nullptr == _read_base_infos)
	{
		cout << "_read_base_infos error" << endl;
		return;
	}

	_get_samid_to_str = (GET_SAMID_TO_STR)GetProcAddress(sdtapi_dll, "GetSAMIDToStr");
	if (nullptr == _get_samid_to_str)
	{
		cout << "_get_samid_to_str error" << endl;
		return;
	}
}
