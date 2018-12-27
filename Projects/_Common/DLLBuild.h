#pragma once

#pragma warning (disable : 4251)//std::vector<> needs to have dll-interface to be used by clients of class

#ifdef _WINDLL  
#define DLL_BUILD __declspec(dllexport)
#else  
//#define DLL_BUILD __declspec(dllimport)   
#define DLL_BUILD
#endif  

#define DEPRECATED(message) [[deprecated(message)]]


typedef unsigned int uint;

