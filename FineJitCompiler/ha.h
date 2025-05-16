#pragma once
#include <vector>
#include <assert.h>
#include <iostream>
#include <asmjit/x86.h>
#include "qtype.h"

using namespace asmjit;
class QCompiler;

enum InstType {
	LOAD_R_L = 1000, // Register-Local
	LOAD_R_O, // Register-Offset takes three
	ADD,
	SUB,
	DIV,
	MUL,

	ASSIGN,
	ASSIGN_ADD,
	ASSIGN_SUB,

	GREATER_THAN,
	LESS_THAN,

	GREATER_EQUAL,
	LESS_EQUAL,

	EQUAL,
};

enum class ENodeType {
	Binary,
	Equal,
	Literal,
	Index,
	Memory,
	Register,
	Const,
	FunctionDef,
	ClassDef,
	Return,

	FunctionCall,

	Argument,
	Scope,

	None
};

enum class AddressMode {
	Literal = 0x0,
	Const,
	Local,
	Memory,
	ClassStatement,
	MethodStatement,
	Label
};

class QMethodDef;
class EvalValue {
public:
	using ptr = std::unique_ptr<EvalValue>;

	EvalValue(int64_t _x) {
		mode = AddressMode::Literal;
		intLiteral = _x;
	}

	EvalValue(x86::Mem& _x) {
		mode = AddressMode::Memory;
		memory = _x;
	}

	EvalValue(x86::Gp& _x) {
		mode = AddressMode::Local;
		local = _x;
	}

	EvalValue(QMethodDef* _x) {
		mode = AddressMode::MethodStatement;
		methodDef = _x;
	}

	AddressMode mode;
	union {
		x86::Mem memory;
		x86::Gp local;
		int64_t intLiteral;

		QMethodDef* methodDef;
		
	};
};

class Node {
public:
	int k;
	int base;
	ENodeType type;
	virtual EvalValue::ptr eval(QCompiler* c, Node* parent) { return nullptr; };
	virtual int Test() { return 1; };
};

class IndexNode;

//class RegisterNode : public Node {
//private:
//public:
//	int index;
//	RegisterNode(int i) {
//		type = ENodeType::Register;
//		index = i;
//	}
//
//	Node* eval(Node* parent) override {
//		return this;
//	}
//
//	int get() {
//		return index;
//	}
//};

class MemoryNode : public Node {
public:
	int val;
	MemoryNode(int v) {
		val = v;
		type = ENodeType::Memory;
	}

	///*void eval(Node* parent) override {
	//	depthTest();
	//	codes.push_back(LOAD_R_L);
	//	codes.push_back(base);
	//	codes.push_back(UseVar(val));
	//}

	EvalValue::ptr eval(QCompiler* c, Node* parent) override;

	//void depthTest() {
	//	varCount++;
	//	LiveInterval liv;
	//	if (GetInterval(val) == -1) {
	//		liv.startPoint = varCount;
	//		liv.endPoint = varCount;
	//		liv.symbol = val;
	//		liv.allocatedRegister = -1;
	//		liveIntervals.push_back(liv);
	//	}
	//	else {
	//		liveIntervals[GetInterval(val)].endPoint = varCount;
	//	}
	//}*/

	int Test() override {
		k = 1;
		return k;
	}
};

class LiteralNode : public Node {
private:
public:
	int64_t i;
	LiteralNode(int64_t _i) {
		i = _i;
		type = ENodeType::Literal;
	}
	EvalValue::ptr eval(QCompiler* c, Node* parent) override;
	/*Node* eval(Node* parent) override {
		codes.push_back(LOAD_R_L);
		codes.push_back(regCnt);
		codes.push_back(i);

		return new RegisterNode(regCnt++);
	}*/

	int Test() override {
		k = 1;
		return k;
	}
};

class ArgumentNode : public Node {
private:
public:
	std::vector<Node*> nodes;
	ArgumentNode(std::vector<Node*>& _nodes) {
		nodes = _nodes;
		type = ENodeType::Argument;
	}
	/*EvalValue::ptr eval(QCompiler* c, Node* parent) override {
		
	}*/

};


class ScopeNode : public Node {
	std::vector<Node*> nodes;

public:
	ScopeNode(std::vector<Node*>& _node) {
		nodes = _node;
		type = ENodeType::Scope;
	}

	EvalValue::ptr eval(QCompiler* c, Node* parent) override;
};

class FunctionCall : public Node{
private:
public:
	std::string name;
	Node* args;
	FunctionCall(std::string_view _name, Node* _node) {
		name = _name;
		args = _node;
		type = ENodeType::FunctionCall;
	}
	/*EvalValue::ptr eval(QCompiler* c, Node* parent) override {

	}*/

};

class FunctionDefinition : public Node {
private:
public:
	QMethodDef* funcNode;
	
	std::string name;
	std::vector<Node*> params;
	QType type;
	Node* scope;
	FunctionDefinition(std::string_view _name, QType _type, std::vector<Node*>& _node, Node* _scope) {
		name = _name;
		params = _node;
		type = _type;
		scope = _scope;
	}
	EvalValue::ptr eval(QCompiler* c, Node* parent) override;
	void preEval(QCompiler* c);
	std::string& getName() {
		return name;
	}
};

class VariableDefinition : public Node {
private:
public:
	std::string name;
	QType qtype;
	VariableDefinition(std::string_view _name, QType _ty) {
		name = _name;
		qtype = _ty;
	}
	std::string& getName() {
		return name;
	}
};


class ClassDefinition : public Node {
	std::string name;
	std::unordered_map<std::string, FunctionDefinition*> methods;
	std::unordered_map<std::string, VariableDefinition*> properties;
public:
	EvalValue::ptr eval(QCompiler* c, Node* parent) override;

	ClassDefinition(std::string_view _name) {
		name = _name;
	}
	void pushProperty(VariableDefinition* prop) {
		properties.insert(std::make_pair(prop->getName(), prop));
	}
	void pushMethod(FunctionDefinition* func) {
		methods.insert(std::make_pair(func->getName(), func));
	}
};


class Identifier : public Node {
private:
public:
	std::string name;
	Identifier(std::string_view _name) {
		name = _name;
	}
	EvalValue::ptr eval(QCompiler* c, Node* parent) override;
	//x86::Gp& get(QCompiler* c, Node* parent) override;
};


//class ParameterNode : public Node {
//private:
//public:
//	std::vector<VariableDefinition*> nodes;
//	ParameterNode(std::vector<VariableDefinition*>& _nodes) {
//		nodes = _nodes;
//	}
//	void eval(Node* parent) override {
//
//	}
//
//};

////class IndexNode : public Node {
////public:
////	Node* val;
////	IndexNode(Node* v) {
////		val = v;
////		type = ENodeType::Index;
////	}
////
////	Node* eval(Node* parent) override;
////};

class BinaryNode : public Node {
private:
	int op;
public:
	Node* l;
	Node* r;
	BinaryNode(Node* _l, Node* _r, int op) {
		l = _l;
		r = _r;
		this->op = op;
		//op = _op;
		type = ENodeType::Binary;
	}

	EvalValue::ptr eval(QCompiler* c, Node* parent);
		
	//Node* Pattern() {
	//	if (l->type == ENodeType::Register && r->type == ENodeType::Register) { // #7
	//		codes.push_back(op);
	//		codes.push_back(((RegisterNode*)l)->get());
	//		codes.push_back(((RegisterNode*)r)->get());
	//		return l;
	//	}

	//	if (l->type == ENodeType::Register && r->type == ENodeType::Literal) { // #8
	//		codes.push_back(op);
	//		codes.push_back(((RegisterNode*)l)->get());
	//		codes.push_back(((LiteralNode*)r)->get());
	//		return l;
	//	}

	//	

	//	if (l->type == ENodeType::Literal && r->type == ENodeType::Index) { // #6
	//		l = l->eval(this);
	//		r = r->eval(this);
	//		IndexNode* ind = (IndexNode*)r;
	//		if (ind->val->type == ENodeType::Binary) {
	//			BinaryNode* bin = ((BinaryNode*)ind->val);
	//			if (bin->l->type == ENodeType::Literal && bin->l->type == ENodeType::Register) {
	//				LiteralNode* literal = (LiteralNode*)bin->l;
	//				RegisterNode* reg = (RegisterNode*)bin->r;
	//				// (((LiteralNode*)l)->i, new OffsetOperand(l->i, r->index))
	//				// delete r;
	//				codes.push_back(ADD_R_O);
	//				codes.push_back(((LiteralNode*)l)->i);
	//				codes.push_back(literal->i);
	//				codes.push_back(reg->index);
	//				return l;
	//			}
	//		}
	//	}

	//	
	//}

	//void eval(Node* parent) override {
	//	//l = l->eval(this);
	//	//r = r->eval(this);

	//	//codes.push_back(op);
	//	
	//	// pattern #7

	//	//int b = base;
	//	//
	//	//

	//	////codes.push_back(((RegisterNode*)l)->get());
	//	////codes.push_back(((RegisterNode*)r)->get());
	//	//if (l->k > r->k) {
	//	//	l->base = b;
	//	//	l->eval(this);
	//	//	r->eval(this);
	//	//	//b = base + 1;
	//	//	codes.push_back(op);
	//	//	codes.push_back(base + 1);
	//	//	codes.push_back(base + 2);
	//	//	return;
	//	//} else
	//	//{
	//	//	r->eval(this);
	//	//	l->eval(this);

	//	//	codes.push_back(op);
	//	//	
	//	//}

	//	//if (parent->base - 1 != k) {
	//	//	codes.push_back(base + 1);
	//	//	codes.push_back(base + 2);
	//	//}
	//	//else {
	//	//	codes.push_back(base);
	//	//	codes.push_back(base + 1);
	//	//}

	//	/*codes.push_back(op);
	//	codes.push_back(regCnt);
	//	codes.push_back(regCnt + 1);*/

	//	//return this;
	//}

	
};


class ReturnNode : public Node {
public:
	Node* expr;
	ReturnNode(Node* _l) {
		expr = _l;
		type = ENodeType::Return;
	}

	EvalValue::ptr eval(QCompiler* c, Node* parent);
};

typedef int32_t(*QashEntry)(int32_t);

class QCompiler {
private:
	x86::Compiler* compiler;
	std::unordered_map<std::string, FuncNode*> functions;
	FunctionDefinition* currMehtod;
public:
	//QCompiler(x86::Compiler* compiler);
	int compile(ClassDefinition* cls);
	
	FuncNode* getFunc(std::string_view name) {
		return functions[name.data()];
	}

	FuncNode* addFunc(std::string name, FuncNode* node) {
		functions.insert(std::make_pair(name, node));
		return node;
	}

	void setCurrentMethod(FunctionDefinition* def) {
		currMehtod = def;
	}

	FunctionDefinition* getCurrentMethod() {
		return currMehtod;
	}

	x86::Compiler* get() {
		return compiler;
	}

	
};


class QMethodDef {
private:
	FuncNode* callee;
	std::unordered_map<std::string, int> params;

	std::unordered_map<std::string, x86::Gp> locals;
public:
	QMethodDef() {
		callee = nullptr;
	}

	void addArg(std::string_view name, FuncSignature& funcSig, QType qtype) {
		switch (qtype) {
		case QType::Int:
			funcSig.addArg(TypeId::kInt32);
			break;
		}
		params.insert(std::make_pair(name, params.size()));
	}

	void addLocal(x86::Compiler* c, std::string& name) {
		locals.insert(std::make_pair(name, c->newInt32(name.c_str())));
	}

	FuncNode* build(QCompiler* c, FuncSignature& funcSig) {
		callee = c->get()->addFunc(funcSig);
		return callee;
	}

	void setRet(TypeId retType, FuncSignature& funcSig) {
		funcSig.setRet(retType);
	}

	void bindArg(x86::Compiler* c, std::string& name) {
		addLocal(c, name);
		callee->setArg(getArg(name), getLocal(name));
	}

	int getArg(std::string& name) {
		return params[name];
	}

	bool containsLocal(std::string& name) {
		return locals.count(name);
	}

	bool containsArg(std::string& name) {
		return params.count(name);
	}

	x86::Gp& getLocal(std::string& name) {
		return locals[name];
	}

	FuncNode* getCallee() { return callee; }
};
