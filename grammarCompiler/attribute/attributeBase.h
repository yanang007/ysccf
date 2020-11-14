#ifndef ATTRIBUTEBASE_H
#define ATTRIBUTEBASE_H 1

#include <variant>
#include <string>
#include <vector>

#include "../../base/base.h"
#include "../grammarCompiler.h"

/**/
template<typename T>
concept Param =
	std::is_same_v<T, int> ||
	std::is_same_v<T, std::string>;


inline std::string unknown_type_code = "u";

template<Param T>
inline std::string type_code = unknown_type_code;

template<typename... Ty>
struct variant_pack
{
	using type = std::variant<Ty...>;

	static std::vector<std::string>& getMappings()
	{
		static std::vector<std::string> data;
		if (data.size() != sizeof...(Ty)) {
			data.reserve(sizeof...(Ty));
			(..., data.push_back(type_code<Ty>));
		}

		return data;
	}

	static std::string getTypeCode(size_t i)
	{
		if(getMappings().size() > i)
			return getMappings()[i];
		return unknown_type_code;
	}
};

using ParamPackHelper = variant_pack<int, std::string>;
using ParamType = ParamPackHelper::type;

template<>
inline std::string type_code<int> = "i";
template<>
inline std::string type_code<std::string> = "s";

namespace {
	struct _Ftag
	{};
}

template<typename T>
concept FType = requires(T t) {
	t.paramCode();
	t.invoked(std::declval<grammarCompiler&>(), std::vector<ParamType>());
};

template<Param... Args>
class F
{
public:
	std::string paramCode() const
	{
		return (... + type_code<Args>);
	}

	bool invoked(grammarCompiler& g, const std::vector<ParamType>& params)
	{
		invokedImpl(params, std::make_index_sequence<sizeof...(Args)>());
		return true;
	};

	virtual void invoked(grammarCompiler& g, Args... args) {}

private:
	template <size_t... I>
	void invokedImpl(grammarCompiler& g, const std::vector<ParamType>& params, std::index_sequence<I...>)
	{
		invoked(g, std::get<Args>(params[I])...);
	}
};

template<FType... Fs>
class attributeBase : Fs...
{
public:
	static std::string generateParamCode(const std::vector<ParamType>& params)
	{
		std::string ret;
		for (const auto& var : params) {
			ret += ParamPackHelper::getTypeCode(var.index());
		}
		return ret;
	}

public:
	attributeBase() {};
	virtual ~attributeBase() {};

	bool invoked(grammarCompiler& g, const std::vector<ParamType>& params)
	{
		std::string paramCode = generateParamCode(params);
		return (... || (paramCode == Fs::paramCode() && Fs::invoked(g, params)));
	};

	virtual stringViewType name() { return "\"NotImplemented\""; };
	virtual void init(grammarCompiler&) { return; };
};


#endif