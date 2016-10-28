#ifndef BVERSION_H
#define BVERSION_H

#define VERSION_TEXT_ "0, 0, 1, 0"
#define VERSION_ 0,0,1,0

#ifdef _WIN64	
	#ifdef _DEBUG	
		#define VERSION_TEXT "0.0.1.0_x64_DEBUG"
	#else
		#define VERSION_TEXT "0.0.1.0 x64"
	#endif
#else
	#ifdef _DEBUG	
		#define VERSION_TEXT "0.0.1.0_x86_DEBUG"
	#else
		#define VERSION_TEXT "0.0.1.0 x86"
	#endif
#endif

#ifdef _WIN64	
	#ifdef _DEBUG	
		#define APP_TITLE ".:: bTuner x64_DEBUG ::."
	#else
		#define APP_TITLE ".:: bTuner ::."
	#endif
#else
	#ifdef _DEBUG	
		#define APP_TITLE ".:: bTuner x86_DEBUG ::."
	#else
		#define APP_TITLE ".:: bTuner ::."
	#endif
#endif

#define DESCRIPTION ".:: bTuner ::. Internet Radio Player"
#define APP_NAME "bTuner"

#endif //BVERSION_H

