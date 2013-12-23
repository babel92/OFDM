#ifndef PACKET
#define PACKET

#include "BaseBlock.h"

const char HEADER[] = "HEAD"; 
const int HEADER_SIZE = sizeof(HEADER)-1;
const int FIELD_SIZE = 4;

class PacketWrapper : public BaseBlock
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
		DataPtr in = In[0]->GetData();
		DataPtr out = Out[0]->AllocData(in->Size() + HEADER_SIZE + 4);
		memcpy(*out, (const void*)HEADER, HEADER_SIZE);
		int size = in->Size();
		memcpy(*out + HEADER_SIZE, &size, 4);
		memcpy(*out + HEADER_SIZE + 4, in->Get(), in->Size());
		return 0;
	}
};

class PacketExtractor : public BaseBlock
{
public:
	PacketExtractor()
		:BaseBlock({ "any in" }, { "any out" }), M_state(HEADER), M_datatoread(-1), M_fieldtoread(-1)
	{
		Ready();
	}
	virtual ~PacketExtractor()
	{

	}
protected:
	const char* M_state;
	int M_datatoread;
	int M_fieldtoread;
	DataPtr out;
	virtual int Work(INPINS In, OUTPINS Out)
	{
		
		DataPtr in = In[0]->GetData();
		char* ptr = (char*)in->Get();
		static char* wptr,fptr;
		static char fieldbuffer[FIELD_SIZE];
		
		// Here we implement a FSM to detect data header
		for (;;)
		{
			if ((unsigned char*)ptr >= in->Get() + in->Size())
			{
				// Make sure no unprepared data are sent
				Out[0]->DetachData();
				return 0;
			}

			if (M_state >= HEADER + HEADER_SIZE)
			{
				// End of header, it's the packet we want
				M_fieldtoread = FIELD_SIZE;
				wptr = fieldbuffer;
				M_state = HEADER - 1;
				continue;
			}
			else if (M_state < HEADER)
			{
				if (M_fieldtoread>0)
				{
					int bytetowrite = min(((int)(in->Get()) + in->Size()) - (int)ptr, M_fieldtoread);
					assert(bytetowrite != 0);
					memcpy(wptr, ptr, bytetowrite);
					wptr += bytetowrite;
					ptr += bytetowrite;
					M_fieldtoread -= bytetowrite;
				}
				if (M_fieldtoread == 0)
				{
					// We've retrieved the field, now do some allocation
					M_fieldtoread = -1;
					M_datatoread = *reinterpret_cast<int*>(fieldbuffer);
					// AllocDataOnly() allocates space but does not attach to m_data 
					out = Out[0]->AllocDataOnly(M_datatoread);
					wptr = (char*)out->Get();
					continue;
				}
				else if (M_datatoread>0)
				{
					int bytetowrite = min(((int)(in->Get()) + in->Size()) - (int)ptr, M_datatoread);
					assert(bytetowrite != 0);
					memcpy(wptr, ptr, bytetowrite);
					wptr += bytetowrite;
					ptr += bytetowrite;
					M_datatoread -= bytetowrite;
				}
				// We want it done in one pass
				if (M_datatoread == 0)
				{
					M_datatoread = -1;
					M_state = HEADER;
					Out[0]->SetData(out);
					Send();
					//return 0;
				}
			}
			else if (*M_state == *ptr)
			{
				M_state++, ptr++;
				continue;
			}
			else
			{
				// Return to the beginning 
				M_state = HEADER, ptr++;
				out.reset();
				continue;
			}
		}
		
		return 0;
	}
};

#endif