#ifndef IGNORE_ATTRIBUTE_H
#define IGNORE_ATTRIBUTE_H 1

#include "./attributeBase.h"

class ignoreAttr : public overloadableAttribute< OnTokenDef<> >
{
public:
	/*stringViewType name() override
	{
		return L"ignore";
	}*/

	void init(grammarCompiler&) override
	{

	}

	void invoked(grammarCompiler& compiler, lexer::tokenID id) override
	{
		compiler.grammarFrontend().setTokenToBeIgnored(id);
	}
};

#endif