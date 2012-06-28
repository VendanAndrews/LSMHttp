#pragma once
#include "DataTypeList.h"

// custom object type declarations 

class JSONNode : public LSTypeDefinition
{
public:
	// All data members (retrieving data) should be listed in this enumeration
	enum JSONNodeMembers
	{
		Type,
		Length,
		Value,
		Node,
	};
	// All data methods (performing actions on or with the object) should be listed in this enumeration
	enum JSONNodeMethods
	{
		Set,		
	};

	JSONNode() : LSTypeDefinition("JSONNode")
	{
		/*
		 * Use the TypeMember macro to activate each member, or use AddMember if the name
		 * would conflict with something else in the source code.
		 */

		TypeMember(Type);
		TypeMember(Length);
		TypeMember(Value);
		TypeMember(Node);

		/*
		 * Use the TypeMethod macro to activate each method, or use AddMethod if the name
		 * would conflict with something else in the source code.
		 */

		TypeMethod(Set);

	}



	/*********************************************************************************************
	 * Basic operations 
	 *
	 */
	
	/* Retrieve some object, a member of an object of this type, using the given arguments */
	virtual bool GetMember(LSOBJECTDATA ObjectData, PLSTYPEMEMBER pMember, int argc, char *argv[], LSOBJECT &Object);

	/* Perform some action on behalf of this object */
	virtual bool GetMethod(LSOBJECTDATA &ObjectData, PLSTYPEMETHOD pMethod, int argc, char *argv[]);

	/* Retrieve some object, an index of an object of this type, using the given arguments */
	virtual int GetIndex(LSOBJECTDATA ObjectData, int argc, char *argv[], LSOBJECT &Object);

	/* Reduce an object of this type to text */
	virtual bool ToText(LSOBJECTDATA ObjectData, char *buf, unsigned int buflen);

	/*********************************************************************************************
	 * Variable initialization and destruction (allows LavishScript variables to be created with this type)
	 *
	 * If variables are not to be supported by the object type, simply return false from the InitVariable
	 * function, or remove the functions below (they will be replaced by the defaults, which do not allow
	 * variables)
	 *
	 */

	/* Initialize an object of this type, potentially with an object sub-type */
    virtual bool InitVariable(LSOBJECTDATA &ObjectData, const char *SubType);

	/* As the second stage of initializing the object, assign the given value(s).  This will
	 * ALWAYS be called immediately after a successful InitVariable, even if no value is given. */
	virtual bool FromText(LSOBJECTDATA &ObjectData, int argc, char *argv[]);

	/* Destroy an object of this type */
	virtual void FreeVariable(LSOBJECTDATA &ObjectData);



	/*********************************************************************************************
	 * Variable type redirection
	 *
	 * In many cases, there is reason to keep a separate datatype for variables, since variables
	 * may be modified.  For example, the string type works on const char *, which would be unsafe
	 * for a LavishScript variable to modify -- the text may be a constant built right into a module.  
	 * The string type redirects variables to the mutablestring type, which works on utf8string *
	 * and inherits the string type (by accessing the const char *).
     */

	/* This function indicates that the variable type is >this< type.  GetVariableType() can also
	   return 0 (NULL) to indicate that variables are not allowed. */
	virtual LSTypeDefinition *GetVariableType()
	{
		return this; /* Redirection example: return pMutableStringType; */
	}


};
