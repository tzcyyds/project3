#pragma once
#include <unordered_map>

using namespace std;

class UserDoc {

public:
	UserDoc();
	~UserDoc();

public:
	unordered_map<string, string> myMap;
	void writeP();
	void initDoc();

};
class UserList
{
public:
	UserList();
	~UserList();

public:
	unordered_map<SOCKET, string> myMap;
};



