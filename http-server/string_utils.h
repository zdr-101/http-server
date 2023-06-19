#pragma once
#include <iostream>
#include <Windows.h>
#include <cstring>

class string_utils
{
public:
	std::string gbk_to_utf8(const std::string& gbkstr);
	std::string get_path();
	std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
};

