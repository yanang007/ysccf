#ifndef IGNORE_ATTRIBUTE_H
#define IGNORE_ATTRIBUTE_H 1

#include "./attributeBase.h"

class ignoreAttr : public overloadableAttribute< OnTokenDef<> >
{
public:
	void init(grammarCompiler& compiler) override
	{

	}

	void invoked(grammarCompiler& compiler, lexer::tokenID id) override
	{
		compiler.grammarFrontend().addIgnoredToken(id);
	}

	void dispose(grammarCompiler& compiler) override
	{
		compiler.grammarFrontend().clearIgnores();
	}
};

#endif