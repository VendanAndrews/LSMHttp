#include "LSMHttp.h"
#include <sstream>
#include "Shlwapi.h"

DWORD WINAPI HTTPSGetThreadRoutine( LPVOID lpParam );
DWORD WINAPI HTTPSGetJSONThreadRoutine( LPVOID lpParam );
DWORD WINAPI HTTPSGetToFileThreadRoutine( LPVOID lpParam );
size_t CURLStringWrite( void *ptr, size_t size, size_t nmemb, void *stream);

struct UrlAndFile {
	char *url;
	char *file;
};

int CMD_HTTPSGet(int argc, char *argv[])
{
	if(argc != 2)
		return 0;
	
	char *url = (char*)malloc(1+strlen(argv[1]));
	strcpy(url, argv[1]);
	CWindowsThread *HTTPSGetThread = new CWindowsThread();
	HTTPSGetThread->BeginThread(&HTTPSGetThreadRoutine, (void*)url, false);
	return 0;
}

int CMD_HTTPSGetJSON(int argc, char *argv[])
{
	if(argc != 2)
		return 0;
	
	char *url = (char*)malloc(1+strlen(argv[1]));
	strcpy(url, argv[1]);
	CWindowsThread *HTTPSGetJSONThread = new CWindowsThread();
	HTTPSGetJSONThread->BeginThread(&HTTPSGetJSONThreadRoutine, (void*)url, false);
	return 0;
}

int CMD_HTTPSGetToFile(int argc, char *argv[])
{
	if(argc != 3)
		return 0;
	UrlAndFile *info = (UrlAndFile *)malloc(sizeof(UrlAndFile));
	info->url = (char*)malloc(1+strlen(argv[1]));
	strcpy(info->url, argv[1]);
	if(PathIsRelative(argv[2]))
	{
		info->file = pLSInterface->GetCWD((char *)malloc(513), 512);
		info->file[strlen(info->file)+1] = 0;
		if(info->file[strlen(info->file)-1]!='\\')
			info->file[strlen(info->file)] = '\\';
		strcpy(info->file+strlen(info->file), argv[2]);
	}
	else
	{
		info->file = (char*)malloc(1+strlen(argv[2]));
		strcpy(info->file, argv[2]);
	}
	CWindowsThread *HTTPSGetThread = new CWindowsThread();
	HTTPSGetThread->BeginThread(&HTTPSGetToFileThreadRoutine, (void*)info, false);
	return 0;
}

DWORD WINAPI HTTPSGetThreadRoutine( LPVOID lpParam )
{
	CWindowsThread *Thread = (CWindowsThread*)lpParam;
	CURL *HTTPSGetRequest;
	char *url = (char *)Thread->Info;
	OpenSSL_Lock->Lock();
	HTTPSGetRequest = curl_easy_init();
	stringstream *HTTPSGetBuffer = new stringstream(stringstream::out);

	curl_easy_setopt(HTTPSGetRequest, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(HTTPSGetRequest, CURLOPT_NOSIGNAL, 1);

	curl_easy_setopt(HTTPSGetRequest, CURLOPT_WRITEDATA, HTTPSGetBuffer);
	curl_easy_setopt(HTTPSGetRequest, CURLOPT_WRITEFUNCTION, CURLStringWrite);
	curl_easy_setopt(HTTPSGetRequest, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(HTTPSGetRequest, CURLOPT_URL, url);

	printf("Error %d", curl_easy_perform(HTTPSGetRequest));
	//curl_easy_cleanup(HTTPSGetRequest);
	OpenSSL_Lock->Unlock();

	pModule->ExecuteLSMHttp_ReceiveEvent(HTTPSGetBuffer->str().c_str());
	delete HTTPSGetBuffer;
	free(url);
	return 0;
}

DWORD WINAPI HTTPSGetJSONThreadRoutine( LPVOID lpParam )
{
	CWindowsThread *Thread = (CWindowsThread*)lpParam;
	CURL *HTTPSGetJSONRequest;
	char *url = (char *)Thread->Info;
	OpenSSL_Lock->Lock();
	HTTPSGetJSONRequest = curl_easy_init();
	stringstream *HTTPSGetJSONBuffer = new stringstream(stringstream::out);

	curl_easy_setopt(HTTPSGetJSONRequest, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(HTTPSGetJSONRequest, CURLOPT_NOSIGNAL, 1);

	curl_easy_setopt(HTTPSGetJSONRequest, CURLOPT_WRITEDATA, HTTPSGetJSONBuffer);
	curl_easy_setopt(HTTPSGetJSONRequest, CURLOPT_WRITEFUNCTION, CURLStringWrite);
	curl_easy_setopt(HTTPSGetJSONRequest, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(HTTPSGetJSONRequest, CURLOPT_URL, url);

	printf("Error %d", curl_easy_perform(HTTPSGetJSONRequest));
	//curl_easy_cleanup(HTTPSGetJSONRequest);
	OpenSSL_Lock->Unlock();
	yajl_val JSON = yajl_tree_parse(HTTPSGetJSONBuffer->str().c_str(), NULL, NULL);

	pModule->ExecuteLSMHttp_ReceiveJSONEvent(JSON);
	delete HTTPSGetJSONBuffer;
	free(url);
	return 0;
}

DWORD WINAPI HTTPSGetToFileThreadRoutine( LPVOID lpParam )
{
	CWindowsThread *Thread = (CWindowsThread*)lpParam;
	CURL *HTTPSGetRequest;
	FILE *output;
	UrlAndFile *info = (UrlAndFile *)Thread->Info;
	OpenSSL_Lock->Lock();
	HTTPSGetRequest = curl_easy_init();
	output = fopen(info->file, "w+");

	curl_easy_setopt(HTTPSGetRequest, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(HTTPSGetRequest, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(HTTPSGetRequest, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(HTTPSGetRequest, CURLOPT_WRITEDATA, output);
	curl_easy_setopt(HTTPSGetRequest, CURLOPT_URL, info->url);

	curl_easy_perform(HTTPSGetRequest);
	//curl_easy_cleanup(HTTPSGetRequest);
	OpenSSL_Lock->Unlock();
	fclose(output);
	pModule->ExecuteLSMHttp_DownloadEvent(info->file);
	free(info->file);
	free(info->url);
	free(info);
	return 0;
}

size_t CURLStringWrite( void *ptr, size_t size, size_t nmemb, void *stream)
{
	stringstream *Buffer = (stringstream*)stream;
	streamsize len = size * nmemb;
	if(Buffer->write(static_cast<char*>(ptr), len))
		return len;
	return 0;
}