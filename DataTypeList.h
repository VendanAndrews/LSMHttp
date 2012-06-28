#ifndef DATATYPE
#define DATATYPE_SELF
#define DATATYPE(_class_,_variable_,_inherits_) extern class _class_ *_variable_
#endif
// ----------------------------------------------------
// data types

DATATYPE(JSONNode,pJSONNode,0);










// ----------------------------------------------------
#ifdef DATATYPE_SELF
#undef DATATYPE_SELF
#undef DATATYPE
#endif