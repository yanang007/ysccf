#ifndef IGNORE_ATTRIBUTE_H
#define IGNORE_ATTRIBUTE_H 1

#include "./attributeBase.h"

class ignoreAttr : public attributeBase< F<> >
{
public:
	stringViewType name() override
	{
		return L"ignore";
	}

	void init(grammarCompiler&) override
	{

	}

	void invoked(grammarCompiler& compiler) override
	{

	}
};

#endif