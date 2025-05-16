#pragma once
#include <map>
#include <string>
#include "qtype.h"
#include <asmjit/x86.h>

using namespace asmjit;
typedef unsigned int QOffset;


struct QPropertyInfo {
	QOffset offset;
	QType type;
	QPropertyInfo(QOffset offset, QType type) : offset(offset), type(type) {}
};

struct QMethodInfo {
	QOffset offset;
	QType returnType;
	QType* argsType;
	QMethodInfo(QOffset offset, QType ret, QType* _args) : offset(offset), returnType(ret), argsType(_args) {}
};


class QClassInfo
{
	int capacity;
	std::map<std::string, QPropertyInfo> field;
	std::map<std::string, QMethodInfo> methods;

	void addField(std::string_view name, QType type, int offset) {
		field.emplace(std::make_pair(name.data(), QPropertyInfo(offset, type)));
	}

	void addMethod(std::string_view name, QType type, QType* argsType, int offset) {
		field.emplace(std::make_pair(name.data(), QMethodInfo(offset, type, argsType)));
	}

	QPropertyInfo getFieldInfo(std::string_view _name) {
		return field[_name.data()];
	}
};

class QClassPackage {
	std::map<std::string, QClassInfo*> classinfos;
};

class QObject {
	QClassInfo* info;
	void* data;

	QObject(void* _data, QClassInfo* _info)
		: data(_data), info(_info) {
	}
};
