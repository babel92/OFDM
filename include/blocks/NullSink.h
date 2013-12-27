#ifndef NULLSINK_H
#define NULLSINK_H

#include <BaseBlock.h>

namespace jsdsp{

	class NullSink : public BaseBlock
	{
	public:
		NullSink() :BaseBlock({ "any in" }, {}) { }
		virtual ~NullSink() {}
	protected:
		virtual int Work(INPINS In, OUTPINS Out)
		{
			return 0;
		}
	private:
	};

}

#endif // NULLSINK_H
