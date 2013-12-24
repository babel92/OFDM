#ifndef STRINGSOURCE_H
#define STRINGSOURCE_H

#include <BaseBlock.h>
#include <string>
#include <cstring>

namespace jsdsp{

	struct Pattern
	{
		char* P;
		int Size;

	};

	template<int N>
	Pattern PatternMaker(const char(&P)[N])
	{
		Pattern ret;
		ret.P = new char[N - 1];
		ret.Size = N - 1;
		memcpy(ret.P, P, N - 1);
		return ret;
	};

	class StringSource : public BaseBlock
	{
	public:
		StringSource(Pattern P) :BaseBlock({}, { "char out" })
		{
			M_pat = P;
			Ready();
		}
		virtual ~StringSource() {}
	protected:
		Pattern M_pat;
		virtual int Work(INPINS In, OUTPINS Out)
		{
			int i = 0;
			for (;;)
			{
				DataPinOut*out = Out[0];
				memcpy((char*)out->AllocData(M_pat.Size)->Get(), M_pat.P, M_pat.Size);
				Send();
			}
			return 0;
		}
	private:
	};

}

#endif // STRINGSOURCE_H
