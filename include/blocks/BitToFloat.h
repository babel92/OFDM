#ifndef BITTOFLOAT_H
#define BITTOFLOAT_H

#include <BaseBlock.h>

namespace jsdsp{

	class BitToFloat : public BaseBlock
	{
	public:
		BitToFloat() :BaseBlock({ "byte in" }, { "float out" }) { }
	protected:
		virtual int Work(INPINS In, OUTPINS Out)
		{
			DataPtr in = In[0]->GetData();
			DataPtr out = Out[0]->AllocData(in->Size());
			for (int i = 0; i < in->Size(); ++i)
				((float*)out->Get())[i] = (in->Get()[i] == '1') ? 1.f : -1.f;
			return 0;
		}
	private:
	};

}

#endif // BITTOFLOAT_H
