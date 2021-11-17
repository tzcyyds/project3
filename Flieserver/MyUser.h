#pragma once
#include <unordered_map>

using namespace std;

struct myUser
{
	//SOCKET s;
	DWORD ip = 0;
	WORD port = 0;//WORDµÈÍ¬ÓÚunsigned short
	string username = "";
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


