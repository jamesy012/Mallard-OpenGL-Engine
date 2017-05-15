#pragma once

#ifdef _WINDLL  
#define DLL_BUILD __declspec(dllexport)   
#else  
//#define DLL_BUILD __declspec(dllimport)   
#define DLL_BUILD
#endif  