#pragma once
#include <gtest/gtest.h>
#include "../javascript/javascript.h"

namespace test {

	class JavascriptTest : public testing::Test
	{
	public:
		JavascriptTest() : js_(javascript::CreateDuktape()) {}
	protected:
		std::shared_ptr<javascript::Javascript> js_;
	};

} // End namespace test.