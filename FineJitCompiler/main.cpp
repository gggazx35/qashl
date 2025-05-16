#include <asmjit/asmjit.h>
#include <stdio.h>
#include <iostream>
#include "parser.h"
#include "qvm.h"
#include "QObject.h"

using namespace asmjit;

enum class Bytecode {
	PTR,
	GET_MEM,
	SET_MEM,
	SET_MEM_P
};


class OffsetTest {
public:
	int offset0;
	int offset1;
};


typedef long long (*Func)();

class MyErrorHandler : public ErrorHandler {
	void handleError(Error err, const char* message, BaseEmitter* emitter) {
		std::cout << message << '\n';
		errorCounter++;
		//throw err;
	}
public:
	int errorCounter = 0;
};


bool BuildSecretCodeAlgo(CodeHolder& code, QFunction* q) {
	x86::Assembler a(&code);
	auto label = a.newLabel();

	MyErrorHandler eh;
	code.setErrorHandler(&eh);

	a.push(x86::rbp);
	a.mov(x86::rbp, x86::rsp);

	a.mov(x86::rcx, q);
	a.mov(x86::rdx, 64);
	a.call(QFunction::call);

	a.nop();
	a.pop(x86::rbp);
	
	/*a.mov(x86::esi, x86::ecx);
	a.mov(x86::edi, x86::edx);
	
	a.sub(x86::esi, x86::edi);

	a.mov(x86::edi, x86::r8d);
	a.mov(x86::eax, x86::r9d);
	a.add(x86::edi, x86::eax);

	a.imul(x86::esi, x86::edi);
	a.mov(x86::edi, x86::r8d);
	a.mov(x86::eax, x86::r9d);

	a.sub(x86::edi, x86::eax);
	a.sub(x86::esi, x86::edi);

	a.mov(x86::eax, x86::esi);*/

//#ifdef _WIN64
//	a.push(x86::rdx);
//	a.mov(x86::rax, x86::rcx);
//	a.bind(label);
//	a.mov(x86::dl, x86::ptr(x86::rcx));
//	//a.rol(x86::eax, 1);
//	
//	//a.xor_(x86::al, x86::cl);
//	a.inc(x86::rcx);
//	a.cmp(x86::dl, 0);
//#else
//	a.mov(x86::edx, x86::ptr(x86::esp, 4));
//	a.mov(x86::eax, 0xAA2200BB);
//	a.bind(label);
//	a.mov(x86::cl, x86::ptr(x86::edx));
//	a.rol(x86::eax, 1);
//	a.xor_(x86::eax, x86::cl);
//	a.inc(x86::edx);
//	a.cmp(x86::cl, 0);
//#endif
//	a.jne(label);
//	a.sub(x86::rcx, x86::rax);
//	a.mov(x86::rax, x86::rcx);
//
//	a.pop(x86::rbx);
//	//a.mov(x86::rax, x86::rdx);
//	a.mov(x86::dword_ptr(x86::rbx, 4), x86::eax);
//	a.mov(x86::dword_ptr(x86::rbx), x86::eax);
//	a.mov(x86::eax, x86::dword_ptr(x86::rbx, 4));
//	
//	a.ret();
	a.ret();
	return eh.errorCounter == 0;
}

int GetSecretCode(const char* stirng, QFunction* q) {
	//std::cout << "czr code: " << std::hex << (long long)t << std::endl;
	
	JitRuntime rt;

	CodeHolder code;
	code.init(rt.environment());

	if (!BuildSecretCodeAlgo(code, q))
		return 0;

	Func fn;
	Error err = rt.add(&fn, &code);

	if (err)
		return 1;

	int result = fn();

	rt.release(fn);

	return result;
}

void compile(CodeHolder& code) {
	x86::Assembler a(&code);

	MyErrorHandler eh;
	code.setErrorHandler(&eh);

	x86::Gpd registers[3] = { x86::eax, x86::edi, x86::esi };
	x86::Gpd locals[4] = { x86::ecx, x86::edx, x86::r8d, x86::r9d };
}

class ClassForTest
{
public:
	char* cptr;
	static void print(ClassForTest* _self) {
		std::cout << _self->cptr << std::endl;
	}
};

int main(int argc, char* argv[]) {
	///*std::string name;
	//std::cout << "What is your name? : ";
	//std::cin >> name;

	//OffsetTest* T = new OffsetTest();
	//T->offset0 = 20;
	//T->offset1 = 0xFF;

	//QClass* cls = new QClass();

	//Instruction ins;
	//ins.op = (u8)Opcode::ADD_R_C;
	//ins.a = 0;
	//ins.b = 100;
	//
	//cls->push(new QFunction(cls->code.size()));

	//cls->code.push_back(ins);

	//ins.op = (u8)Opcode::RET;
	//cls->code.push_back(ins);

	//VM::init(cls->code.data());*/


	/*Opcode opcodes[20];

	opcodes[0].op = (unsigned int)Bytecode::PTR;
	opcodes[0].offset = 0;

	opcodes[1].op = (unsigned int)Bytecode::SET_MEM;
	opcodes[1].value = 20;
	opcodes[1].offset = 0;

	opcodes[2].op = (unsigned int)Bytecode::PTR;
	opcodes[2].offset = 0;

	opcodes[3].op = (unsigned int)Bytecode::SET_MEM;
	opcodes[3].value = 10;
	opcodes[3].offset = 4;


	opcodes[4].op = (unsigned int)Bytecode::PTR;
	opcodes[4].offset = 0;

	opcodes[5].op = (unsigned int)Bytecode::GET_MEM;
	opcodes[5].offset = 0;

	opcodes[6].op = (unsigned int)Bytecode::PTR;
	opcodes[6].offset = 4;

	opcodes[7].op = (unsigned int)Bytecode::SET_MEM;
	opcodes[7].offset = 0;*/
	//std::cout << QFunction::call(cls->at(0), 64) << '\n';
	/*int code = GetSecretCode(name.c_str(), cls->at(0));
	std::cout << "Your code: " << code << " and 01 " << std::endl;*/

	//Node* top = new BinaryNode(new BinaryNode(
	//	new BinaryNode(
	//		new MemoryNode(0),
	//		new MemoryNode(1),
	//		ADD
	//	),
	//	new BinaryNode(
	//		new MemoryNode(2),
	//		new MemoryNode(3), 
	//		SUB
	//	), DIV
	//),
	//new BinaryNode(new MemoryNode(2), new MemoryNode(3), MUL), SUB);
	///*top->Test();
	//top->eval(1, 4);
	//allocateRegisters();*/
	//x86::Gpq coolAdult = x86::rax;

	//for (auto i : codes) {
	//	std::cout << i << '\n';
	//}

	std::vector<Token> tokens;
	char* str20 = (char*)"20";
	char* str10 = (char*)"10";
	char* str50 = (char*)"50";

	char* strP = (char*)("+");
	char* strM = (char*)("*");

	char* strF = (char*)("init");
	char* strX = (char*)("xender");

	char* strDef = (char*)("def");
	char* strInt = (char*)("int");

	tokens.push_back(Token{ ETokenType::Ident, strDef });

	tokens.push_back(Token{ ETokenType::Ident, strF });
	tokens.push_back(Token{ ETokenType::LParen, "" });
	
	tokens.push_back(Token{ ETokenType::Ident, strX });
	tokens.push_back(Token{ ETokenType::Colon, ""});
	tokens.push_back(Token{ ETokenType::Ident, strInt });

	tokens.push_back(Token{ ETokenType::RParen, "" });

	tokens.push_back(Token{ ETokenType::Colon, ""});
	tokens.push_back(Token{ ETokenType::Ident, strInt });

	tokens.push_back(Token{ ETokenType::Tab, "" });

	tokens.push_back(Token{ ETokenType::Ident, (char*)("ret") });
	//tokens.push_back(Token{ ETokenType::LParen, nullptr });

	tokens.push_back(Token{ ETokenType::Ident, "func" });

	tokens.push_back(Token{ ETokenType::LParen, "" });
	tokens.push_back(Token{ ETokenType::Ident, strX });
	tokens.push_back(Token{ ETokenType::RParen, "" });

	tokens.push_back(Token{ ETokenType::Binary, strP });
	tokens.push_back(Token{ ETokenType::Integer, str20 });
	tokens.push_back(Token{ ETokenType::Binary, strM });
	tokens.push_back(Token{ ETokenType::Integer, str50 });
	tokens.push_back(Token{ ETokenType::Binary, strM });
	tokens.push_back(Token{ ETokenType::Integer, str10 });

	// func

	tokens.push_back(Token{ ETokenType::Ident, strDef });

	tokens.push_back(Token{ ETokenType::Ident, "func" });
	tokens.push_back(Token{ ETokenType::LParen, "" });

	tokens.push_back(Token{ ETokenType::Ident, strX });
	tokens.push_back(Token{ ETokenType::Colon, "" });
	tokens.push_back(Token{ ETokenType::Ident, strInt });

	tokens.push_back(Token{ ETokenType::RParen, "" });

	tokens.push_back(Token{ ETokenType::Colon, "" });
	tokens.push_back(Token{ ETokenType::Ident, strInt });

	tokens.push_back(Token{ ETokenType::Tab, "" });

	tokens.push_back(Token{ ETokenType::Ident, (char*)("ret") });
	//tokens.push_back(Token{ ETokenType::LParen, nullptr });

	tokens.push_back(Token{ ETokenType::Ident, strX });
	tokens.push_back(Token{ ETokenType::Binary, strM });
	tokens.push_back(Token{ ETokenType::Integer, "2" });



	///////////////////////////////////

	//tokens.push_back(Token{ ETokenType::RParen, nullptr });

	tokens.push_back(Token{ ETokenType::Eof, "" });
	/*tokens.push_back(Token{ ETokenType::Binary, strP });
	tokens.push_back(Token{ ETokenType::Integer, str20 });*/

	std::string aTitel = "main";
	Parser* p = new Parser(tokens, aTitel);
	auto comp = new QCompiler();
	comp->compile(p->getTop());




	QObject* obj = new QObject()


	/*

	
	Node* pTop = p->parseExpr_1(p->parsePrimary(), 0);*/
	//pTop->eval(1, 4);

	return 0;
}