#include <iostream>
#include <fstream>
#include <sstream>
#include "../javascript/javascript.h"

int main(int ac, char** av)
{
	try
	{
		auto js = javascript::CreateDuktape();
		if (ac == 1)
		{
			std::string str;
			std::cout << "enter your javascript here:" << std::endl;
			std::cout << " >>> ";
			std::getline(std::cin, str);
			js->AddFromString(str);
		}
		else if (ac == 2)
		{
			std::string file_name = av[1];
			std::ifstream ifs(file_name.c_str());
			js->AddFromStream(ifs);
		} 
		else
		{
			std::ostringstream oss;
			oss << "incorrect list of parameters." << std::endl;
			oss << " > javascript_cmd [filename]";
			throw std::runtime_error(oss.str());
		}
		while (js->GetPrintLines())
		{
			std::cout << js->GetPrintString() << std::endl;
		}
	}
	catch (std::exception ex)
	{
		std::cerr << ex.what() << std::endl;
	}
	return 0;
}