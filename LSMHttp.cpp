//
// LSMHttp
//

// Version guideline: YYYYMMDD
// Add lettering to the end to indicate a new version on the same date, such as 20060305a, 20060305b, etc
// You can also use a standard version numbering system such as 1.00, 1.01, etc. 
// Be aware that for the versioning system, this text is simply compared to another version text from the 
// same extension to check for new versions -- if this version text comes before the compared text (in a 
// dictionary), then an update is available.  Equal text means the version is up to date.  After means this 
// is newer than the compared version.  With that said, use whatever version numbering system you'd like.
#define MODULE_VERSION "20120625"

#include "LSMHttp.h"

#pragma comment(lib,"LSModule.lib")
// The mandatory pre-setup function.  Our name is "LSMHttp", and the class is LSMHttp.
// This sets up a "ModulePath" variable which contains the path to this module in case we want it,
// and a "PluginLog" variable, which contains the path and filename of what we should use for our
// debug logging if we need it.  It also sets up a variable "pExtension" which is the pointer to
// our instanced class.
LSModulePreSetup("LSMHttp",LSMHttp);

// Basic LavishScript datatypes, these get retrieved on startup by our initialize function, so we can
// use them in our Top-Level Objects or custom datatypes
LSTypeDefinition *pStringType=0;
LSTypeDefinition *pIntType=0;
LSTypeDefinition *pUintType=0;
LSTypeDefinition *pBoolType=0;
LSTypeDefinition *pFloatType=0;
LSTypeDefinition *pTimeType=0;
LSTypeDefinition *pByteType=0;
LSTypeDefinition *pIntPtrType=0;
LSTypeDefinition *pBoolPtrType=0;
LSTypeDefinition *pFloatPtrType=0;
LSTypeDefinition *pBytePtrType=0;
LSTypeDefinition *pIndexType=0;
LSTypeDefinition *pIteratorType=0;
JSONLookup *JSONLookupHead=0;


LSInterface *pLSInterface=0;

char LSMHttp_Version[]=MODULE_VERSION;


// The constructor of our class.  General initialization cannot be done yet, because we're not given
// the pointer to the LavishScript interface until it is ready for us to initialize.  Just set the
// pointer we have to the interface to 0.  Initialize data members, too.
LSMHttp::LSMHttp(void)
{
}

// Free any remaining resources in the destructor.  This is called when the DLL is unloaded, but
// LavishScript calls the "Shutdown" function first.  Most, if not all, of the shutdown process should
// be done in Shutdown.
LSMHttp::~LSMHttp(void)
{
}

// Initialize is called by Inner Space when the extension should initialize.
bool LSMHttp::Initialize(LSInterface *p_LSInterface)
{
	pLSInterface=p_LSInterface;
	//RegisterModule();

	// retrieve basic ISData types
	pStringType=pLSInterface->FindLSType("string");
	pIntType=pLSInterface->FindLSType("int");
	pUintType=pLSInterface->FindLSType("uint");
	pBoolType=pLSInterface->FindLSType("bool");
	pFloatType=pLSInterface->FindLSType("float");
	pTimeType=pLSInterface->FindLSType("time");
	pByteType=pLSInterface->FindLSType("byte");
	pIntPtrType=pLSInterface->FindLSType("intptr");
	pBoolPtrType=pLSInterface->FindLSType("boolptr");
	pFloatPtrType=pLSInterface->FindLSType("floatptr");
	pBytePtrType=pLSInterface->FindLSType("byteptr");
	pIndexType=pLSInterface->FindLSType("index");
	pIteratorType=pLSInterface->FindLSType("iterator");
	JSONLookupHead=(JSONLookup*)malloc(sizeof(JSONLookup));
	JSONLookupHead->next = 0;
	

	RegisterEvents();
	RegisterCommands();
	RegisterAliases();
	RegisterDataTypes();
	RegisterTopLevelObjects();
    RegisterTriggers();

	curl_global_init(CURL_GLOBAL_ALL);

	printf("LSMHttp version %s Loaded",LSMHttp_Version);
	return true;
}

// shutdown sequence
void LSMHttp::Shutdown()
{
	UnRegisterTopLevelObjects();
	UnRegisterDataTypes();
	UnRegisterAliases();
	UnRegisterCommands();
}

void LSMHttp::RegisterCommands()
{
	// add any commands
//	pLSInterface->AddCommand("LSMHttp",CMD_LSMHttp,true,false);
#define COMMAND(name,cmd,parse,hide) pLSInterface->AddCommand(name,cmd,parse,hide);
#include "Commands.h"
#undef COMMAND
}

void LSMHttp::RegisterAliases()
{
	// add any aliases
}

void LSMHttp::RegisterDataTypes()
{
	// add any datatypes
	// pMyType = new MyType;
	// pLSInterface->AddLSType(*pMyType);


#define DATATYPE(_class_,_variable_,_inherits_) {_variable_ = new _class_; pLSInterface->AddLSType(*_variable_); _variable_->SetInheritance(_inherits_);}
#include "DataTypeList.h"
#undef DATATYPE
}

#define EVENT(_eventname_,_variable_) unsigned int _variable_=0
#include "Events.h"
#undef EVENT

void LSMHttp::RegisterEvents()
{
	// Register any events
#define EVENT(_eventname_,_variable_) _variable_=pLSInterface->RegisterEvent(_eventname_)
#include "Events.h"
#undef EVENT
}

void LSMHttp::RegisterTopLevelObjects()
{
	// add any Top-Level Objects
	//pLSInterface->AddTopLevelObject("LSMHttp",TLO_LSMHttp);
#define TOPLEVELOBJECT(name,funcname) pLSInterface->AddTopLevelObject(name,funcname);
#include "TopLevelObjects.h"
#undef TOPLEVELOBJECT
}

void LSMHttp::RegisterTriggers()
{
	// add any Triggers
}

void LSMHttp::UnRegisterCommands()
{
	// remove commands
//	pLSInterface->RemoveCommand("LSMHttp");
#define COMMAND(name,cmd,parse,hide) pLSInterface->RemoveCommand(name);
#include "Commands.h"
#undef COMMAND
}
void LSMHttp::UnRegisterAliases()
{
	// remove aliases
}
void LSMHttp::UnRegisterDataTypes()
{
	// remove data types
#define DATATYPE(_class_,_variable_,_inherits_) {pLSInterface->RemoveLSType(*_variable_); delete _variable_;}
#include "DataTypeList.h"
#undef DATATYPE

}
void LSMHttp::UnRegisterTopLevelObjects()
{
	// remove Top-Level Objects
//	pLSInterface->RemoveTopLevelObject("LSMHttp");
#define TOPLEVELOBJECT(name,funcname) pLSInterface->RemoveTopLevelObject(name);
#include "TopLevelObjects.h"
#undef TOPLEVELOBJECT
}

void LSMHttp::ExecuteLSMHttp_ReceiveEvent(const char *text)
{
	LSEventHelper Event(Event_LSMHttp_Receive);

	Event.AddArg(text);

	Event.Execute();
}

void LSMHttp::ExecuteLSMHttp_ReceiveJSONEvent(yajl_val JSON)
{
	LSEventHelper Event(Event_LSMHttp_ReceiveJSON);

	JSONLookup *newNode = JSONLookupHead;
	while(newNode->next)
		newNode=newNode->next;
	newNode->next = (JSONLookup*)malloc(sizeof(newNode));
	newNode = newNode->next;
	newNode->next = 0;
	newNode->exists = 1;
	newNode->cur = JSON;
	char* ID = itoa((int) newNode, (char*)malloc(20)+1, 10)-1;
	ID[0] = 'J';
	Event.AddArg(ID);

	Event.Execute();
}

void LSMHttp::ExecuteLSMHttp_DownloadEvent(const char *file)
{
	LSEventHelper Event(Event_LSMHttp_Download);

	Event.AddArg(file);

	Event.Execute();
}

