#ifndef COMMAND
#define COMMAND_SELF
#define COMMAND(name,cmd,parse,hide) extern int cmd(int argc, char *argv[])
#endif
// ----------------------------------------------------
// commands

COMMAND("HTTPSGet", CMD_HTTPSGet, true, false);
COMMAND("HTTPSGetJSON", CMD_HTTPSGetJSON, true, false);
COMMAND("HTTPSGetToFile", CMD_HTTPSGetToFile, true, false);




// ----------------------------------------------------
#ifdef COMMAND_SELF
#undef COMMAND_SELF
#undef COMMAND
#endif