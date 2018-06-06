#include <bits/stdc++.h>
#include "config.h"

int main(int argc, char **argv)
{
	ConfigManager::GetInstance().Reset({"thief"});
	std::cout << (*ConfigManager::GetInstance().GetConfig("thief"))["name"].Str();
	return 0;
}
