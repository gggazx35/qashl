#pragma once

enum class InstructionType {
	ADD_R_R,
	SUB_R_R,
	MUL_R_R,
	DIV_R_R,

	ADD_R_C,
	SUB_R_C,
	MUL_R_C,
	DIV_R_C,

	ADD_M_R, // register - register aka [rbx - rcx]
	
	ADD_MC_R, // register - const

	LOAD_MEM // lea rbx (reg)
};
