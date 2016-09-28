#include <string.h>
#define JSON_IS_AMALGAMATION 1
#include "../JSON\json.h"
#include "winsock2.h"
#include "../TCP\PracticalSocket.h"

#pragma comment(lib,"ws2_32.lib")


using namespace std;


class TuneIn {
public:
	TuneIn();
	~TuneIn();

	void Tune(string id);
	void GetRootMenu();
	void LoadJSON(string url);
	string _formatReq(string url);

private:
	bool _result_ok(void);
	string Locale;
	string _BaseUrl;
	string _partnerId;
	Json::Value _Value;
	Json::Reader _Reader;
	

};