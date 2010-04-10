#include "stdafx.h"
#include "TProtocolUtil.h"

using namespace Thrift;
using namespace Thrift::Protocol;

void TProtocolUtil::Skip(TProtocol & prot, TType type)
{
	switch (type)
	{
	case TypeBool:
		prot.ReadBool();
		break;
	case TypeByte:
		prot.ReadByte();
		break;
	case TypeI16:
		prot.ReadI16();
		break;
	case TypeI32:
		prot.ReadI32();
		break;
	case TypeI64:
		prot.ReadI64();
		break;
	case TypeDouble:
		prot.ReadDouble();
		break;
	case TypeString:
		{
			// Don't try to decode the string, just skip it.
			TBinary bin;
			prot.ReadBinary(bin);
			break;
		}
	case TypeStruct:
		{
			TStruct struc;
			prot.ReadStructBegin(struc);
			for(;;)
			{
				TField field;
				prot.ReadFieldBegin(field);
				if (field.GetType() == TypeStop)
					break;
				Skip(prot, field.GetType());
				prot.ReadFieldEnd();
			}
			prot.ReadStructEnd();
			break;
		}
	case TypeMap:
		{
			TMap map;
			prot.ReadMapBegin(map);
			for (int i = 0; i < map.GetCount(); i++)
			{
				Skip(prot, map.GetKeyType());
				Skip(prot, map.GetValueType());
			}
			prot.ReadMapEnd();
			break;
		}
	case TypeSet:
		{
			TSet set;
			prot.ReadSetBegin(set);
			for (int i = 0; i < set.GetCount(); i++)
				Skip(prot, set.GetElementType());
			prot.ReadSetEnd();
			break;
		}
	case TypeList:
		{
			TList list;
			prot.ReadListBegin(list);
			for (int i = 0; i < list.GetCount(); i++)
				Skip(prot, list.GetElementType());
			prot.ReadListEnd();
			break;
		}
	}
}
