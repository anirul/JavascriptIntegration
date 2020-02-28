#include <fstream>
#include "javascript_test.h"

namespace test {

	TEST_F(JavascriptTest, DuktapeCreateTest)
	{
		ASSERT_TRUE(js_);
	}

	TEST_F(JavascriptTest, DuktapePrintTest)
	{
		ASSERT_TRUE(js_);
		EXPECT_EQ(0, js_->GetPrintLines());
		js_->AddFromString("print('hello world!')");
		EXPECT_EQ(1, js_->GetPrintLines());
		EXPECT_EQ("hello world!", js_->GetPrintString());
		EXPECT_EQ(0, js_->GetPrintLines());
	}

	TEST_F(JavascriptTest, DuktapeSimpleFunctionCallTest)
	{
		ASSERT_TRUE(js_);
		js_->AddFromString(
			R"(
				var test_func = function() {
					print('test');
				}
			)");
		EXPECT_EQ(0, js_->GetPrintLines());
		std::ignore = js_->CallFunction("test_func");
		EXPECT_EQ(1, js_->GetPrintLines());
		EXPECT_EQ("test", js_->GetPrintString());
		EXPECT_EQ(0, js_->GetPrintLines());
	}

	TEST_F(JavascriptTest, DuktapeParamIntRetVoidFunctionTest)
	{
		ASSERT_TRUE(js_);
		js_->AddFromString(
			R"(
				var test_func = function(a, b) {
					var c = a + b;
					print('result: ' + c);
				}
			)");
		EXPECT_EQ(0, js_->GetPrintLines());
		std::ignore = js_->CallFunction("test_func", { 2, 3 });
		EXPECT_EQ(1, js_->GetPrintLines());
		EXPECT_EQ("result: 5", js_->GetPrintString());
		EXPECT_EQ(0, js_->GetPrintLines());
	}

	TEST_F(JavascriptTest, DuktapeParamDoubleRetDoubleFunctionTest)
	{
		ASSERT_TRUE(js_);
		js_->AddFromString(
			R"(
				var test_func = function(a, b) {
					return a * b;
				}
			)");
		EXPECT_EQ(0, js_->GetPrintLines());
		auto rs = js_->CallFunction("test_func", { 2.0, 3.0 });
		EXPECT_EQ(6.0, std::any_cast<double>(rs));
	}

	TEST_F(JavascriptTest, DuktapeParamStringRetStringFunctionTest)
	{
		ASSERT_TRUE(js_);
		js_->AddFromString(
			R"(
				var test_func = function(a) {
					return 'test: ' + a;
				}
			)");
		EXPECT_EQ(0, js_->GetPrintLines());
		auto rs = js_->CallFunction("test_func", { "test" });
		EXPECT_EQ("test: test", std::any_cast<std::string>(rs));
	}

	TEST_F(JavascriptTest, DuktapeRepeatedParamBoolRetBoolFunctionTest)
	{
		ASSERT_TRUE(js_);
		js_->AddFromString(
			R"(
				var test_func = function(a) {
					return !a;
				}
			)");
		EXPECT_EQ(0, js_->GetPrintLines());
		for (const auto& i : { 0, 1, 2, 3, 4, 5 })
		{
			auto rs = js_->CallFunction("test_func", { true });
			EXPECT_EQ(false, std::any_cast<bool>(rs));
		}
	}

	TEST_F(JavascriptTest, DuktapeSimpleLoadFromFileTest)
	{
		std::ifstream ifs("../asset/person.js");
		ASSERT_TRUE(ifs.is_open());
		js_->AddFromStream(ifs);
	}

	TEST_F(JavascriptTest, DuketapeRepeatedRetStringDoubleMethodTest)
	{
		std::ifstream ifs("../asset/person.js");
		ASSERT_TRUE(ifs.is_open());
		js_->AddFromStream(ifs);
		EXPECT_EQ(0, js_->GetPrintLines());
		for (const auto& i : { 0, 1, 2, 3, 4, 5 })
		{
			auto rs = js_->CallMethod("teacher", "getName");
			EXPECT_EQ("John Doe", std::any_cast<std::string>(rs));
		}
		for (const auto& i : { 0, 1, 2, 3, 4, 5 })
		{
			auto rs = js_->CallMethod("student", "getAge");
			EXPECT_EQ(12, std::any_cast<double>(rs));
		}
	}

	TEST_F(JavascriptTest, DuktapeRetObjectFunctionTest)
	{
		ASSERT_TRUE(js_);
		js_->AddFromString(
			R"(
			var test_func = function(a) {
				return {
					'test' : 'test',
					'value' : 64,
					'obj' : {	'value' : 42	}
				};
			}
		)");
		EXPECT_EQ(0, js_->GetPrintLines());
		auto rs = js_->CallFunction("test_func");
		using map_type = std::map<std::string, std::any>;
		map_type should_be{
			{ "test", std::string("test") },
			{ "value", 64.0 },
			{ "obj", map_type{ { "value", 42.0 } } }
		};
		map_type is = std::any_cast<map_type>(rs);
 		EXPECT_EQ(
 			std::any_cast<std::string>(should_be["test"]), 
 			std::any_cast<std::string>(is["test"]));
		EXPECT_EQ(
			std::any_cast<double>(should_be["value"]),
			std::any_cast<double>(is["value"]));
		EXPECT_EQ(
			std::any_cast<map_type>(should_be["obj"]).size(),
			std::any_cast<map_type>(is["obj"]).size());
		EXPECT_EQ(
			std::any_cast<double>(
				std::any_cast<map_type>(should_be["obj"])["value"]),
			std::any_cast<double>(
				std::any_cast<map_type>(is["obj"])["value"]));
	}

} // End namespace test.