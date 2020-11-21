#ifndef ATTRIBUTE_MANAGER_H
#define ATTRIBUTE_MANAGER_H 1

#include "./attributeBase.h"

using upAttribute = std::unique_ptr<attributeBase>;

class attributeManager
{
public:
	attributeManager() {}
	~attributeManager() {}

	attributeBase* getAttributeByName(const std::string& name) { 
		if (auto iter = attrs.find(name); iter != attrs.end()) {
			return iter->second.get();
		}
		return nullptr;
	}

	void declareNewAttribute(const std::string& name, upAttribute&& attr) {
		attrs.emplace(name, std::move(attr));
	}

private:
	std::map<std::string, std::unique_ptr<attributeBase>> attrs;
};

#endif // ATTRIBUTE_MANAGER_H