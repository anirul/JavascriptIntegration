#pragma once
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <any>
#include <istream>
#include "javascript.h"
#include "duktape.h"

namespace javascript {

	class DuktapeImpl : public Javascript
	{
	public:
		DuktapeImpl();
		virtual ~DuktapeImpl();
		void AddFromString(const std::string& source) override;
		void AddFromStream(std::istream& is) override;
		const std::size_t GetPrintLines() const override;
		const std::string GetPrintString() override;
		const std::any CallFunction(
			const std::string& func,
			const std::vector<std::any>& args = {}) override;
		const std::any CallMethod(
			const std::string& obj,
			const std::string& func,
			const std::vector<std::any>& args = {}) override;

	protected:
		const std::string GetTopType(int id = -1) const;
		const std::size_t SetArgs(
			const std::vector<std::any>& args) const;
		const std::any GetReturn() const;

	private:
		duk_context* p_duk_ctx_ = nullptr;
		static std::map<duk_context*, std::vector<std::string>> s_ctx_strings_;
	};

} // End namespace javascript.