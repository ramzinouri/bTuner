#include "TuneIn.h"



TuneIn::TuneIn()
{
	Locale="en-US";
	_BaseUrl="opml.radiotime.com";
	_partnerId="4cJcz1b1";

};

TuneIn::~TuneIn()
{

};

bool TuneIn::_result_ok()
{
	if (_Value["head"]["status"].asString() == "200")
		return true;
	else
		return false;
};

string TuneIn::_formatReq(string url)
{
	if (url.find("/")==string::npos)
		url="/"+url;
	if (url.find("?")==string::npos)
		url+="?";
	else
		url+="&";
	if (url.find("partnerId")==string::npos)
		url +="partnerId="+_partnerId+"&";
	if (url.find("locale")==string::npos)
		url +="locale="+Locale+"&";
	//if (url.find("playlists")==string::npos)
	//	url +="playlists=none&";
	if (url.find("render=json")==string::npos)
		url+= "render=json";

	return url;
};

void TuneIn::LoadJSON(string url)
{
	string *GET=new string("GET "+_formatReq(url)+" HTTP/1.1\r\nHost: "+_BaseUrl+"\r\n\r\n");
	TCPSocket *sock=new TCPSocket;
	sock->connect(_BaseUrl,80);
	sock->send(GET->c_str(),GET->length());

	char _buf[1024];
	memset(&_buf,0,1024);
	int n=0;
	int i=0;
	i=sock->recv(_buf,1024);
	string buf(_buf);
	if(sock->recv(_buf,1024) >0)	
		buf+=_buf;
	sock->Disconnect();
	char *dat;
	dat=strstr((char*)buf.c_str(),"\r\n\r\n")+4;
	string data(dat);

	bool parsingSuccessful = _Reader.parse(data, _Value ,false);
	if (_result_ok())
	{
		Json::Value body=_Value["body"];
		string str=body[0]["url"].asString();
	}
	

};

void TuneIn::GetRootMenu()
{
	LoadJSON(_formatReq("Browse.ashx?c="));

};

void TuneIn::Tune(string id)
{
	LoadJSON(_formatReq("Tune.ashx?id="+id));
	if (_result_ok())
	{
		MessageBoxA(0, _Value["body"][0]["url"].asCString(), "url", MB_OK);
	}
	else
	{
		string _err;
		_err = "Title: " + _Value["head"]["title"].asString() + "\nFault: " + _Value["head"]["fault"].asString() + "\nFault Code: " + _Value["head"]["fault_code"].asString();
		MessageBoxA(0, _err.c_str(), "ERROR", MB_ICONERROR);
	}
};