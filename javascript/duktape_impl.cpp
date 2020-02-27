#include <functional>
#include <stdexcept>
#include <sstream>
#include "duktape_impl.h"

namespace javascript {

	std::map<duk_context*, std::vector<std::string>> 
		DuktapeImpl::s_ctx_strings_ = {};

	DuktapeImpl::DuktapeImpl() : p_duk_ctx_(duk_create_heap_default())
	{
		duk_push_c_function(
			p_duk_ctx_, 
			[](duk_context* ctx) ->duk_ret_t
		{
			javascript::DuktapeImpl::s_ctx_strings_[ctx].emplace_back(
				duk_to_string(ctx, 0));
			return 0;
		},
			1);
		duk_put_global_string(p_duk_ctx_, "print");
	}

	DuktapeImpl::~DuktapeImpl() 
	{
		s_ctx_strings_.erase(p_duk_ctx_);
		duk_destroy_heap(p_duk_ctx_);
	}

	void DuktapeImpl::AddFromString(const std::string& source)
	{
		duk_push_string(p_duk_ctx_, source.c_str());
		if (duk_peval(p_duk_ctx_) != 0)
		{
			throw duk_safe_to_string(p_duk_ctx_, -1);
		}
	}

	void DuktapeImpl::AddFromStream(std::istream& is)
	{
		std::string temp(std::istreambuf_iterator<char>(is), {});
		AddFromString(temp);
	}

	const std::any DuktapeImpl::CallFunction(
		const std::string& func,
		const std::vector<std::any>& args /*= {}*/)
	{
		std::any ret;
		duk_get_global_string(p_duk_ctx_, func.c_str());
		std::size_t pushed = SetArgs(args);
		if (duk_pcall(p_duk_ctx_, static_cast<duk_idx_t>(args.size())) 
			!= DUK_EXEC_SUCCESS)
		{
			std::string error = duk_safe_to_string(p_duk_ctx_, -1);
			duk_pop(p_duk_ctx_);
			throw std::runtime_error(error);
		}
		ret = GetReturn();
		duk_pop(p_duk_ctx_);
		return ret;
	}

	const std::any DuktapeImpl::CallMethod(
		const std::string& obj, 
		const std::string& func, 
		const std::vector<std::any>& args /*= {}*/)
	{
		std::any ret;
		duk_get_global_string(p_duk_ctx_, obj.c_str());
		duk_get_prop_string(p_duk_ctx_, -1, func.c_str());
		duk_dup(p_duk_ctx_, -2);
		std::size_t pushed = SetArgs(args);
		if (duk_pcall_method(
				p_duk_ctx_, 
				static_cast<duk_idx_t>(args.size())) != DUK_EXEC_SUCCESS)
		{
			std::string error = duk_safe_to_string(p_duk_ctx_, -1);
			duk_pop_2(p_duk_ctx_);
			throw std::runtime_error(error);
		}
		ret = GetReturn();
		duk_pop_2(p_duk_ctx_);
		return ret;
	}

	const std::string DuktapeImpl::GetTopType() const
	{
		static std::map<int, std::string> type_names = 
		{
			{ DUK_TYPE_NONE,		"none" },
			{ DUK_TYPE_UNDEFINED,	"undefined" },
			{ DUK_TYPE_NULL,		"null" },
			{ DUK_TYPE_BOOLEAN,		"boolean" },
			{ DUK_TYPE_NUMBER,		"number" },
			{ DUK_TYPE_STRING,		"string" },
			{ DUK_TYPE_OBJECT,		"object" },
			{ DUK_TYPE_BUFFER,		"buffer" },
			{ DUK_TYPE_POINTER,		"pointer" },
			{ DUK_TYPE_LIGHTFUNC,	"light function" },
		};
		for (const auto& type_name : type_names)
		{
			if (duk_check_type(p_duk_ctx_, -1, type_name.first))
			{
				return type_name.second;
			}
		}
		return "unknown";
	}

	const std::size_t DuktapeImpl::SetArgs(
		const std::vector<std::any>& args) const
	{
		std::string ret;
		std::size_t pushed = 0;
		for (const auto& field : args)
		{
			if (field.type() == typeid(int))
			{
				pushed++;
				duk_push_int(
					p_duk_ctx_,
					std::any_cast<int>(field));
			}
			else if (field.type() == typeid(bool))
			{
				pushed++;
				duk_push_boolean(
					p_duk_ctx_,
					std::any_cast<bool>(field));
			}
			else if (field.type() == typeid(double))
			{
				pushed++;
				duk_push_number(
					p_duk_ctx_,
					std::any_cast<double>(field));
			}
			else if (field.type() == typeid(std::string))
			{
				pushed++;
				duk_push_string(
					p_duk_ctx_,
					std::any_cast<std::string>(field).c_str());
			}
			else if (field.type() == typeid(const char*))
			{
				pushed++;
				duk_push_string(
					p_duk_ctx_,
					std::any_cast<const char*>(field));
			}
			else
			{
				std::ostringstream oss{};
				oss << "unknown arg type: (" << field.type().name() << ") ";
				ret += oss.str();
			}
		}
		return pushed;
	}

	const std::any DuktapeImpl::GetReturn() const
	{
		if (duk_check_type(p_duk_ctx_, -1, DUK_TYPE_BOOLEAN))
		{
			return static_cast<bool>(
				duk_get_boolean(p_duk_ctx_, -1));
		}
		else if (duk_check_type(p_duk_ctx_, -1, DUK_TYPE_NUMBER))
		{
			return static_cast<double>(
				duk_get_number(p_duk_ctx_, -1));
		}
		else if (duk_check_type(p_duk_ctx_, -1, DUK_TYPE_STRING))
		{
			return static_cast<std::string>(
				duk_get_string(p_duk_ctx_, -1));
		}
		else if (duk_check_type(p_duk_ctx_, -1, DUK_TYPE_OBJECT))
		{
			duk_enum(p_duk_ctx_, -1, 0);
			std::map<std::string, std::any> insider_map;
			while (duk_next(p_duk_ctx_, -1, 1))
			{
				insider_map[duk_get_string(p_duk_ctx_, -2)] = GetReturn();
				duk_pop_2(p_duk_ctx_);
			}
			duk_pop(p_duk_ctx_);
			return std::move(insider_map);
		}
		else if (duk_check_type(p_duk_ctx_, -1, DUK_TYPE_NONE))
		{
			return nullptr;
		}
		else if (duk_check_type(p_duk_ctx_, -1, DUK_TYPE_UNDEFINED))
		{
			return nullptr;
		}
		else if (duk_check_type(p_duk_ctx_, -1, DUK_TYPE_NULL))
		{
			return nullptr;
		}
		else
		{
			// Should be buffer, pointer and light function?
			std::ostringstream oss{};
			oss << "unknown return type: (" << GetTopType() << ") ";
			throw std::runtime_error(oss.str());
		}
	}

	const std::size_t DuktapeImpl::GetPrintLines() const
	{
		return s_ctx_strings_[p_duk_ctx_].size();
	}

	const std::string DuktapeImpl::GetPrintString()
	{
		std::vector<std::string>& vs = s_ctx_strings_[p_duk_ctx_];
		if (vs.empty())
		{
			return {};
		}
		std::string ret = vs.front();
		vs.erase(vs.begin());
		return ret;
	}

} // End namespace javascript.