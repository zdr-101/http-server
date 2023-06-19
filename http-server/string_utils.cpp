#include "string_utils.h"

std::string string_utils::gbk_to_utf8(const std::string& gbkstr)
{
	// ½« GBK ±àÂëµÄ×Ö·û´®×ª»»Îª Unicode ±àÂëµÄ×Ö·û´®
	int wlen = MultiByteToWideChar(CP_ACP, 0, gbkstr.c_str(), -1, nullptr, 0);
	wchar_t* wbuf = new wchar_t[wlen];
	MultiByteToWideChar(CP_ACP, 0, gbkstr.c_str(), -1, wbuf, wlen);

	// ½« Unicode ±àÂëµÄ×Ö·û´®×ª»»Îª UTF-8 ±àÂëµÄ×Ö·û´®
	int len = WideCharToMultiByte(CP_UTF8, 0, wbuf, -1, nullptr, 0, nullptr, nullptr);
	char* buf = new char[len];
	WideCharToMultiByte(CP_UTF8, 0, wbuf, -1, buf, len, nullptr, nullptr);

	std::string utf8str(buf);
	delete[] wbuf;
	delete[] buf;
	return utf8str;
}

std::string string_utils::get_path()
{
	char path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	std::string exePath(path);
	std::string exeDir = exePath.substr(0, exePath.find_last_of("\\/"));
	for (char& c : exeDir) {
		if (c == '\\') {
			c = '/';
		}
	}
	return exeDir;
}

std::string string_utils::base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len)
{
	static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];
	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;
			for (i = 0; (i < 4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}
	if (i) {
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';
		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;
		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];
		while ((i++ < 3))
			ret += '=';
	}
	return ret;
}

