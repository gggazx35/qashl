#include "ha.h"

EvalValue::ptr MemoryNode::eval(QCompiler* c, Node* parent)
{
	return nullptr;
}

EvalValue::ptr ClassDefinition::eval(QCompiler* c, Node* parent)
{
	return methods["init"]->eval(c, this);
}

EvalValue::ptr FunctionDefinition::eval(QCompiler* c, Node* parent)
{
	FuncSignature funcSig = FuncSignature();
	FuncNode* funcNode;
	QMethodDef* method = new QMethodDef();
	this->funcNode = method;

	for (int i = 0; i < params.size(); i++) {
		VariableDefinition* var = ((VariableDefinition*)params[i]);
		method->addArg(var->getName(), funcSig, var->qtype);
	}
	method->setRet(TypeId::kInt32, funcSig);

	funcNode = method->build(c, funcSig);

	
	c->setCurrentMethod(this);
	
	scope->eval(c, this);

	c->addFunc(name, funcNode);
	c->setCurrentMethod(nullptr);

	c->get()->endFunc();
	c->get()->finalize();

	return std::make_unique<EvalValue>(method);
}

EvalValue::ptr ScopeNode::eval(QCompiler* c, Node* parent)
{
	for (auto node : nodes) {
		auto x = node->eval(c, parent);
	}
	return nullptr;
}

EvalValue::ptr Identifier::eval(QCompiler* c, Node* parent)
{
	x86::Gp reg;
	if (c->getCurrentMethod() != nullptr) {
		auto func = c->getCurrentMethod()->funcNode;
		if (func->containsLocal(name)) {
			reg = func->getLocal(name);
		}
		else if (func->containsArg(name)) {
			func->bindArg(c->get(), name);
			reg = func->getLocal(name);
		}
		else {
			std::cout << "Error this method does not contains variable named " << name << '\n';
		}
	}
	return std::make_unique<EvalValue>(reg);
}

EvalValue::ptr BinaryNode::eval(QCompiler* c, Node* parent)
{
	auto left = l->eval(c, this);
	auto right = r->eval(c, this);

	if (left->mode == AddressMode::Local && right->mode == AddressMode::Local) {
		switch (op) {
		case InstType::ADD:
			c->get()->add(left->local, right->local);
			break;
		case InstType::SUB:
			c->get()->sub(left->local, right->local);
			break;
		}
		return left;
	}

	if (left->mode == AddressMode::Literal && right->mode == AddressMode::Literal) {
		int64_t result = 0;
		switch (op) {
		case InstType::ADD:
			result = (left->intLiteral + right->intLiteral); break;
		case InstType::SUB:
			result = (left->intLiteral - right->intLiteral); break;
		case InstType::MUL:
			result = (left->intLiteral * right->intLiteral); break;
		case InstType::DIV:
			result = (left->intLiteral / right->intLiteral); break;
		}

		return std::make_unique<EvalValue>(result);
	}

	if (left->mode == AddressMode::Literal && right->mode == AddressMode::Local) {
		switch (op) {
		case InstType::ADD:
			c->get()->add(right->local, left->intLiteral);
			break;
		case InstType::SUB:
			c->get()->neg(right->local);
			if (left->intLiteral != 0) {
				c->get()->add(right->local, left->intLiteral);
			}
			break;
		}
		return right;
	}

	if (left->mode == AddressMode::Local && right->mode == AddressMode::Literal) {
		switch (op) {
		case InstType::ADD:
			c->get()->add(left->local, right->intLiteral);
			break;
		case InstType::SUB:
			c->get()->sub(left->local, right->intLiteral);
			break;
		}
		return left;
	}
}

EvalValue::ptr LiteralNode::eval(QCompiler* c, Node* parent)
{
	return std::make_unique<EvalValue>(i);
}


int QCompiler::compile(ClassDefinition* cls)
{
	currMehtod = nullptr;


	JitRuntime rt;                           // Runtime specialized for JIT code execution.
	CodeHolder code;                         // Holds code and relocation information.

	code.init(rt.environment(),              // Initialize code to match the JIT environment.
		rt.cpuFeatures());
	compiler = new x86::Compiler(&code);
	

	auto xcls = cls->eval(this, nullptr);

	QashEntry qashEnt;
	Error err = rt.add(&qashEnt, &code);
	if (err) return 1;

	auto forTest = qashEnt(20);
	std::cout << forTest << '\n';
	rt.release(qashEnt);
	return 0;
}

EvalValue::ptr ReturnNode::eval(QCompiler* c, Node* parent)
{
	auto ret = expr->eval(c, this);
	if (ret->mode == AddressMode::Local) {
		c->get()->ret(ret->local);
	}
	else if (ret->mode == AddressMode::Literal) {
		x86::Gp result = c->get()->newInt32();
		c->get()->mov(result, ret->intLiteral);
		c->get()->ret(result);
	}
	return ret;
}
