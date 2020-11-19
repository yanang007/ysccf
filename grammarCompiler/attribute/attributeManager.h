#ifndef ATTRIBUTE_MANAGER_H
#define ATTRIBUTE_MANAGER_H 1

#include "./attributeBase.h"

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

private:
	std::map<std::string, std::unique_ptr<attributeBase>> attrs;
};

#endif // ATTRIBUTE_MANAGER_H