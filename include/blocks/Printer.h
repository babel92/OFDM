#ifndef PRINTER_H
#define PRINTER_H

#include <BaseBlock.h>
#include <cstdio>

namespace jsdsp{

	class Printer : public BaseBlock
	{
	public:
		Printer() :BaseBlock({ "char in" }, {}) { }
		virtual ~Printer() {}
	protected:
		virtual int Work(INPINS In, OUTPINS Out)
		{
			char buf[10240];
			DataPtr input = In[0]->GetData();
			memcpy(buf, *input, input->Size());
			buf[input->Size()] = '\0';
			puts(buf);
			return 0;
		}
	private:
	};

}

#endif // PRINTER_H
