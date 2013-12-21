#ifndef PACKET
#define PACKET

#include "BaseBlock.h"

const char HEADER[] = "HEAD"; //32 bit

class PacketWrapper: public BaseBlock
{
public:
	PacketWrapper() 
		:BaseBlock({ "any in" }, { "any out" })
	{
		Ready();
	}
	virtual ~PacketWrapper()
	{

	}
protected:
	virtual int Work(INPINS In, OUTPINS Out)
	{
		Data*in = In[0]->GetData();
		Data*out = Out[0]->AllocData(in->Size() + 4 + 4);
		memcpy(*out, (const void*)HEADER, 4);
		int size = in->Size();
		memcpy(*out + 4, &size, 4);
		memcpy(*out + 8, in->Get(), in->Size());
		return 0;
	}
};

class PacketExtractor : public BaseBlock
{
public:
	PacketExtractor()
		:BaseBlock({ "any in" }, { "any out" })
	{
		Ready();
	}
	virtual ~PacketExtractor()
	{

	}
protected:
	virtual int Work(INPINS In, OUTPINS Out)
	{
		Data*in = In[0]->GetData();
		if (0 != memcmp(*in, (const void*)HEADER, 4))
			return 0;

		int size;
		memcpy(&size, *in + 4, 4);
		Data*out = Out[0]->AllocData(size);

		memcpy(*out, *in + 8, size);
		return 0;
	}
};

#endif