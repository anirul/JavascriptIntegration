#include "javascript.h"
#include "duktape_impl.h"

namespace javascript {

	std::shared_ptr<Javascript> CreateDuktape()
	{
		return std::make_shared<DuktapeImpl>();
	}

} // End namespace javascript.