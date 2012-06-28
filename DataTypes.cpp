#include "LSMHttp.h"
#define DATATYPE(_class_,_variable_,_inherits_) class _class_ *_variable_=0
#include "DataTypeList.h"
#undef DATATYPE


/* Set up an easy way to access the object, given its 32-bit representation (e.g. a pointer to
   a class or structure) */
#define pJSON ((yajl_val)ObjectData.Ptr)


bool JSONNode::GetMember(LSOBJECTDATA ObjectData, PLSTYPEMEMBER pMember, int argc, char *argv[], LSOBJECT &Object)
{
/*******************************************
 * Parameters
 *
 * [in] LSOBJECTDATA ObjectData: ObjectData is a 32-bit value that can be accessed in any number of different ways
 *        by way of union.  Most commonly, ObjectData.Ptr, ObjectData.DWord, or ObjectData.CharPtr are useful.  This
 *        value is the representation of some object of this object type.  "fulltemplatefoo" works on FullTemplateFoo*
 *        so ObjectData is a FullTemplateFoo*
 *
 * [in] PLSTYPEMEMBER pMember: pMember is a pointer to the information on the member to be retrieved, including its
 *        Name and ID.  We use the ID in a switch statement in order to quickly process the member, since the Name
 *        has already been resolved by the LavishScript engine.
 *
 * [in] int argc, char *argv[]: argc and argv are *nearly* standard C console program parameters.  The difference here
 *        is that the name of the member is NOT given as the first argument (in contrast to LavishScript commands).
 *        Therefore, argc is 0 unless arguments are specifically given to the member retrieval.
 *
 * [out] LSOBJECT &Object: Object is an object that is the result of the member retrieval operation.  An LSOBJECT
 *        is an LSOBJECTDATA, in addition to a separate Type value used to give the value context (in the first
 *        parameter, the context is already known to be the fulltemplatefoo object type, so it is LSOBJECTDATA).
 *
 */

/*******************************************
 * Return Value
 *
 * The return value for this function is very simple.  If the member retrieval fails for any reason, return false.
 * If Object is a valid object and the member retrieval is successful, return true.
 *
 */

	/* Validate the pointer */
	if (!pJSON)
		return false;

	/* Perform the given member retrieval */
	switch(pMember->ID)
	{
	case Type:
		Object.DWord=pJSON->type;
		Object.Type=pUintType;
		return true;
	case Length:
		switch(pJSON->type)
		{
		case yajl_t_object:
			Object.DWord = pJSON->u.object.len;
			break;
		case yajl_t_array:
			Object.DWord = pJSON->u.array.len;
			break;
		default:
			Object.DWord=0;
		}
		Object.Type=pUintType;
		return true;
	case Value:
		switch(pJSON->type)
		{
		case yajl_t_string:
			Object.CharPtr = pJSON->u.string;
			Object.Type = pStringType;
			return true;
		case yajl_t_number:
			Object.CharPtr = pJSON->u.number.r;
			Object.Type = pStringType;
			return true;
		case yajl_t_true:
			Object.DWord = true;
			Object.Type = pBoolType;
			return true;
		case yajl_t_false:
			Object.DWord = false;
			Object.Type = pBoolType;
			return true;
		}
		return false;
	case Node:
		return GetIndex(ObjectData, argc, argv, Object);
	}
	return false;
}

bool JSONNode::GetMethod(LSOBJECTDATA &ObjectData, PLSTYPEMETHOD pMethod, int argc, char *argv[])
{
/*******************************************
 * Parameters
 *
 * [in] LSOBJECTDATA ObjectData: ObjectData is a 32-bit value that can be accessed in any number of different ways
 *        by way of union.  Most commonly, ObjectData.Ptr, ObjectData.DWord, or ObjectData.CharPtr are useful.  This
 *        value is the representation of some object of this object type.  "fulltemplatefoo" works on FullTemplateFoo*
 *        so ObjectData is a FullTemplateFoo*
 *
 * [in] PLSTYPEMETHOD pMethod: pMethod is a pointer to the information on the method to be retrieved, including its
 *        Name and ID.  We use the ID in a switch statement in order to quickly process the method, since the Name
 *        has already been resolved by the LavishScript engine.
 *
 * [in] int argc, char *argv[]: argc and argv are *nearly* standard C console program parameters.  The difference here
 *        is that the name of the method is NOT given as the first argument (in contrast to LavishScript commands).
 *        Therefore, argc is 0 unless arguments are specifically given to the method retrieval.
 */

/*******************************************
 * Return Value
 *
 * The return value for this function is very simple.  If the method execution fails for any reason, OR the object
 * is destroyed during execution, return false.  Otherwise, return true (indicating the object still exists AND
 * the method execution succeeded).
 *
 */

	/* Perform the given member retrieval */
	switch(pMethod->ID)
	{
	case Set:
		if(pJSON)
			yajl_tree_free(pJSON);
		ObjectData.Ptr = yajl_tree_parse(argv[0], NULL, NULL);
		return true;

		
	}
	return false;
}

/* Retrieve some object, an index of an object of this type, using the given arguments */
int JSONNode::GetIndex(LSOBJECTDATA ObjectData, int argc, char *argv[], LSOBJECT &Object)
{
	if(!pJSON)
		return false;
	if(argc!=1)
		return false;
	switch(pJSON->type)
	{
	case yajl_t_object:
		for(unsigned int l1 = 0; l1 < pJSON->u.object.len; l1++)
		{
			if(strcmp(pJSON->u.object.keys[l1], argv[0])==0)
			{
				Object.Ptr = pJSON->u.object.values[l1];
				Object.Type = pJSONNode;
				return true;
			}
		}
		break;
	case yajl_t_array:
		unsigned int index = atoi(argv[0]);
		if(index > pJSON->u.array.len)
			return false;
		index--;
		Object.Ptr = pJSON->u.array.values[index];
		Object.Type = pJSONNode;
		return true;
	}
	return false;
}

/* Reduce an object of this type to text */
bool JSONNode::ToText(LSOBJECTDATA ObjectData, char *buf, unsigned int buflen)
{
	switch(pJSON->type)
	{
	case yajl_t_string:
		strncpy(buf,pJSON->u.string,buflen);
		buf[buflen-1] = 0;
		return true;
	case yajl_t_number:
		strncpy(buf,pJSON->u.number.r,buflen);
		buf[buflen-1] = 0;
		return true;
	case yajl_t_true:
		strncpy(buf,"TRUE",buflen);
		buf[buflen-1] = 0;
		return true;
	case yajl_t_false:
		strncpy(buf,"FALSE",buflen);
		buf[buflen-1] = 0;
		return true;
	}
	return FALSE;
}

bool JSONNode::InitVariable(LSOBJECTDATA &ObjectData, const char *SubType)
{
	if (SubType && SubType[0])
	{
		return false;
	}
	return true;
}

/* As the second stage of initializing a variable of this type, assign the given value(s).  This will
 * ALWAYS be called immediately after a successful InitVariable, even if no value is given. */
bool JSONNode::FromText(LSOBJECTDATA &ObjectData, int argc, char *argv[])
{
	if (argc==1)
	{
		if(argv[0][0]=='J')
		{
			JSONLookup* ptr = (JSONLookup*)atoi(argv[0]+1);
			if(ptr!=0)
			{
				if(ptr->exists)
				{
					ObjectData.Ptr = ptr->cur;
				}
				else
				{
					return false;
				}
			}
			else
			{
				ObjectData.Ptr = yajl_tree_parse(argv[0], NULL, NULL);
			}
		}
	}
	return true;
}

/* Destroy a variable of this type */
void JSONNode::FreeVariable(LSOBJECTDATA &ObjectData)
{
	if(pJSON)
	{
		yajl_tree_free(pJSON);
		JSONLookup* node = JSONLookupHead;
		while(node = node->next)
		{
			if(node->cur == pJSON)
			{
				node->exists = 0;
			}

		}
	}
}

#undef pJSON
