#pragma once
#include <string>
#include "ha.h"
#include "qtype.h"


enum class ETokenType
{
	Binary,
	Assign,
	LParen,
	RParen,
	Integer,
	Ident,
	Comma,
	Colon,
	Tab,
	Eol,
	Eof
};

typedef struct Token {
	ETokenType type;
	char* value;
};

typedef struct OperatorData {
	int precedence;
	InstType inst;
	bool isRightAssco;
};

typedef struct TypeKeywordData {
	int precedence;
	InstType inst;
	bool isRightAssco;
};

class Parser
{
private:
	int index;
	std::string title;
	std::vector<Token> tokens;
	std::unordered_map<std::string, OperatorData> opLookup;
	std::unordered_map<std::string, QType> typeLookup;
	int tabCount;
	ClassDefinition* cls;
	/*class ParserFactory {
		OperatorData& opd{

		}
	};*/

public:
	Parser(std::vector<Token>& _tokens, std::string& _title);

	Token& peekNext();

	/*bool isEnd() {
		return index >= tokens.size();
	}*/

	void Advance();

	int calcPrecedence(Token& val);

	bool isRightAssco(Token& val);

	Node* parseArguement();
	Node* parseVariableDefinition();

	Node* parseScope();

	FunctionDefinition* parseMethod();


	Node* parseVariable();
	Node* parseReturn();
	
	Node* parsePrimary();

	Node* parseExpr();

	Node* parseExpr_1(Node* lhs, int minPrecedence);

	ClassDefinition* getTop();
};