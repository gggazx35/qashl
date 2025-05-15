//#include "qvm.h"
//#include <iostream>
//int main() {
//	QClass* cls = new QClass();
//
//	Instruction ins;
//	ins.op = (u8)Opcode::ADD_R_C;
//	ins.a = 0;
//	ins.b = 100;
//	cls->push(new QFunction(cls->code.size()));
//	cls->code.push_back(ins);
//
//	VM::init(cls->code.data());
//	std::cout << QFunction::call(cls->at(0), 64) << '\n';
//
//	return 0;
//}