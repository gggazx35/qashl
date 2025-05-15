#pragma once

#include <vector>

typedef unsigned long long u64;
typedef unsigned char u8;

enum class Opcode : u8
{
	MOV_R_R,
	MOV_R_M,
	MOV_R_C,
	MOV_R_O,

	MOV_M_R,
	MOV_M_C,

	LEA_R_R,
	LEA_R_M,
	LEA_R_C,
	LEA_R_O,
	
	LEA_M_R,
	LEA_M_C,
	
	ADD_R_R,
	ADD_R_M,
	ADD_R_C,
	ADD_R_O,
	ADD_M_R,
	ADD_M_C,
	CALL,
	RET
};

typedef struct Instruction {
	u8 op : 8;
	unsigned short a : 12;
	unsigned short b : 12;
};

class VM
{
public:
	static u64 call(u64 _ip, int _localCount) {
		static u64 reg[4] = { 0, 0, 0, 0 };


		u64 ip = _ip;
		u64 bp = sp;
		
		reg[0] = stack[bp-1];
		while (1) {
			Instruction inst = code[ip];
			switch (inst.op)
			{
			case (u8)Opcode::MOV_R_R:
				reg[inst.a] = reg[inst.b];
				break;
			case (u8)Opcode::MOV_R_C:
				reg[inst.a] = inst.b;
				break;
			case (u8)Opcode::ADD_R_R:
				reg[inst.a] += reg[inst.b];
				break;
			case (u8)Opcode::ADD_R_C:
				reg[inst.a] += inst.b;
				break;
			case (u8)Opcode::CALL: 
				
				break;
			case (u8)Opcode::RET:
				sp -= _localCount;
				return reg[0];
			}
			ip++;
		}
	}

	static void init(Instruction* _code) {
		code = _code;
		stack = new u64[20];
	}

	static void push(u64 _i) {
		stack[sp++] = _i;
	}
private:
	static Instruction* code;
	static u64* stack;
	static int sp;
};

Instruction* VM::code = nullptr;
u64* VM::stack = nullptr;
int VM::sp = 0;

class QFunction;
class QClass {
private:
	std::vector<QFunction*> funcs;
public:
	std::vector<Instruction> code;

	void push(QFunction* _func) {
		funcs.push_back(_func);
	}
	QFunction* at(int _i) {
		return funcs[_i];
	}
};

class QFunction {
private:
	typedef u64(*_intarg_1)(u64);
	typedef u64(*_intarg_2)(u64, u64);
	typedef u64(*_intarg_3)(u64, u64, u64);
	bool isNative;
	union {
		_intarg_1 _i1;
		_intarg_2 _i2;
		_intarg_3 _i3;
		u64 ip;
	} callee;
public:
	QFunction(u64 _ip) {
		isNative = false;
		callee.ip = _ip;
	}

	static u64 call(QFunction* q, u64 _0) {
		if (!q->isNative) {
			VM::push(_0);
			return VM::call(q->callee.ip, 1);
		}
		return q->callee._i1(_0);
	}

	static u64 callFromVM(QFunction* q, u64 _0, u64 _1) {
		q->callee._i2(_0, _1);
	}
};