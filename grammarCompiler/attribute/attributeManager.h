#ifndef ATTRIBUTE_MANAGER_H
#define ATTRIBUTE_MANAGER_H 1

#include "./attributeBase.h"

using upAttribute = std::unique_ptr<attributeBase>;

class attributeManager
{
public:
	attributeManager() {}
	~attributeManager() {}

	enum class InvokeResult
	{
		irSuccessful,
		irAttributeNotExist,
		irWrongParameter
	};

	InvokeResult invoke(const stringType& name, grammarCompiler& g, const std::vector<ParamType>& params) {
		if (auto pAttr = getAttributeByName(name);
			pAttr != nullptr)
		{
			bool needInit = !initiatedAttributes.contains(pAttr);
			if (needInit) {
				if (pAttr->verify(g, params)) {
					pAttr->init(g);
				}
				else {
					return InvokeResult::irWrongParameter;
				}
			}

			if (pAttr->invoked(g, params)) {
				if (needInit) {
					initiatedAttributes.insert(pAttr);
				}
				return InvokeResult::irSuccessful;
			}
			else {
				if (needInit) {
					pAttr->dispose(g);
				}
				return InvokeResult::irWrongParameter;
			}
		}

		return InvokeResult::irAttributeNotExist;
	}

	attributeBase* getAttributeByName(const stringType& name) { 
		if (auto iter = attrs.find(name); iter != attrs.end()) {
			return iter->second.get();
		}
		return nullptr;
	}

	void declareNewAttribute(const stringType& name, upAttribute&& attr) {
		attrs.emplace(name, std::move(attr));
	}

private:
	std::map<stringType, std::unique_ptr<attributeBase>> attrs;
	std::set<attributeBase*> initiatedAttributes;
};

#endif // ATTRIBUTE_MANAGER_H