#include "pch.h"
#include "MyUser.h"



UserDoc::UserDoc()
{
}

UserDoc::~UserDoc()
{
}

void UserDoc::writeP()
{
}

void UserDoc::initDoc()
{
	myMap.insert(std::pair<std::string, std::string>("test", "12345"));
}

