#pragma once
#include "pch.h"


struct myUser
{
	IN_ADDR ip = { 0 };
	WORD port = 0;//WORD等同于unsigned short
	std::string username = "";
	int state = 0;
	CString strdirpath = ""; // 文件路径
};

class UserDoc {

public:
	UserDoc();
	~UserDoc();

public:
	std::unordered_map<std::string, std::string> myMap;
	void writeP();
	void initDoc();

};
class WaitList
{

public:
	std::unordered_map<SOCKET, std::string> myMap;
};

class LinkInfo
{
public:
	std::unordered_map<SOCKET, myUser> myMap;
};


