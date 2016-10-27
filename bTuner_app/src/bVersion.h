#ifndef BVERSION_H
#define BVERSION_H

#define VERSION_TEXT_ "0, 0, 1, 0"
#define VERSION_ 0,0,1,0

#ifdef _WIN64	
	#define VERSION_TEXT "0.0.1.0 x64";
#else
	#define VERSION_TEXT "0.0.1.0 x86";
#endif

#define DESCRIPTION ".:: bTuner ::. Internet Radio Player"
#define APPNAME "bTuner"

#endif //BVERSION_H

