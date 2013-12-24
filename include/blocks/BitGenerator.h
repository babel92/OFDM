#ifndef BITGENERATOR_H
#define BITGENERATOR_H

#include <BaseBlock.h>
#include <string>
#include <cassert>

using namespace std;

namespace jsdsp{

	class BitGenerator : public BaseBlock
	{
	public:
		BitGenerator(const string&Pattern) :BaseBlock({}, { "byte out" })
		{
			m_pattern = Pattern;
			m_outsize = lcm(128, m_pattern.length());
			Ready();
		}
		virtual ~BitGenerator() {}
	protected:
		virtual int Work(INPINS In, OUTPINS Out)
		{
			DataPinOut*outpin = Out[0];
			int len = m_pattern.length();
			for (;;)
			{
				DataPtr ptr = outpin->AllocData(m_outsize);
				assert(ptr);
				for (int i = 0; i < m_outsize; i += len)
					memcpy(ptr->Get() + i, m_pattern.c_str(), len);
				Send();
			}
			return 0;
		}
	private:
		int gcd(int a, int b)
		{
			for (;;)
			{
				if (a == 0) return b;
				b %= a;
				if (b == 0) return a;
				a %= b;
			}
		}

		int lcm(int a, int b)
		{
			int temp = gcd(a, b);

			return temp ? (a / temp * b) : 0;
		}
		string m_pattern;
		int m_outsize;
	};

}

#endif // BITGENERATOR_H
