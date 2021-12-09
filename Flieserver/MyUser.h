#pragma once
#include "pch.h"

using namespace std;

struct myUser
{
	DWORD ip = 0;
	WORD port = 0;//WORDµÈÍ¬ÓÚunsigned short
	string username = "";
	int state = 0;
};

class UserDoc {

public:
	UserDoc();
	~UserDoc();

public:
	unordered_map<string, string> myMap;
	void writeP();
	void initDoc();

};
class WaitList
{

public:
	unordered_map<SOCKET, string> myMap;
};

class LinkInfo
{
public:
	unordered_map<SOCKET, myUser> myMap;
};


