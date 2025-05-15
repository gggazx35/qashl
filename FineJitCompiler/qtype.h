#pragma once
#include <unordered_map>

enum class QType {
	Int
};

//class QVariable {
//	std::string name;
//	QType type;
//
//public:
//	QVariable(QType _type, std::string& _name) {
//		name = _name;
//		type = _type;
//	}
//};

class QMethod {
	QType type;
	std::unordered_map<std::string, QType> variables;
	std::unordered_map<std::string, QType> parameters;
public:
	QMethod(QType _returnType) {
		//name = _name;
		type = _returnType;
	}
};

//class QClass {
//	std::unordered_map<std::string, QType> properties;
//	std::unordered_map<std::string, QMethod> methods;
//};

