#pragma once
#include <LSModule.h>
#include <windows.h>
#include "curl/curl.h"
#include "yajl/yajl_tree.h"

class LSMHttp :
	public LSModuleInterface
{
public:
	LSMHttp(void);
	~LSMHttp(void);

	virtual bool Initialize(LSInterface *p_LSInterface);
	virtual void Shutdown();

	void RegisterCommands();
	void RegisterAliases();
	void RegisterDataTypes();
	void RegisterTopLevelObjects();
	void RegisterTriggers();
	void RegisterEvents(); // Events should NOT get unregistered

	void UnRegisterCommands();
	void UnRegisterAliases();
	void UnRegisterDataTypes();
	void UnRegisterTopLevelObjects();
	void UnRegisterTriggers();

	void ExecuteLSMHttp_ReceiveEvent(const char *text);
	void ExecuteLSMHttp_ReceiveJSONEvent(yajl_val JSON);
	void ExecuteLSMHttp_DownloadEvent(const char *file);
};

extern LSInterface *pLSInterface;

extern LSMHttp *pModule;
#define printf pLSInterface->Printf

#define EzCheckTriggers(Text) IS_CheckTriggers(pExtension,pLSInterface,hTriggerService,Text)

extern LSTypeDefinition *pStringType;
extern LSTypeDefinition *pIntType;
extern LSTypeDefinition *pUintType;
extern LSTypeDefinition *pBoolType;
extern LSTypeDefinition *pFloatType;
extern LSTypeDefinition *pTimeType;
extern LSTypeDefinition *pByteType;
extern LSTypeDefinition *pIntPtrType;
extern LSTypeDefinition *pBoolPtrType;
extern LSTypeDefinition *pFloatPtrType;
extern LSTypeDefinition *pBytePtrType;
extern LSTypeDefinition *pIndexType;
extern LSTypeDefinition *pIteratorType;

extern CLock *OpenSSL_Lock;

extern char LSMHttp_Version[];

typedef struct JSONLookupStart
{
	yajl_val cur;
	int exists;
	JSONLookupStart* next;
} JSONLookup;

extern JSONLookup *JSONLookupHead;

#include "Commands.h"
#include "DataTypes.h"
#include "TopLevelObjects.h"
#include "Events.h"
