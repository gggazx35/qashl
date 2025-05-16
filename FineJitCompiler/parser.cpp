#include "parser.h"




Parser::Parser(std::vector<Token>& _tokens, std::string& _title)  {
	title = _title;
	cls = new ClassDefinition(_title);
	tokens = _tokens;
	index = 0;
	tabCount = 0;

	int order = 1;

	OperatorData opd;


	opd.isRightAssco = true;
	opd.precedence = order++;

	//////////////

	opd.inst = ASSIGN;
	opLookup.insert(std::make_pair("=", opd));
	
	opd.inst = ASSIGN_ADD;
	opLookup.insert(std::make_pair("+=", opd));

	opd.inst = ASSIGN_SUB;
	opLookup.insert(std::make_pair("-=", opd));

	///////////

	opd.isRightAssco = false;

	opd.precedence = order++;


	////////////////

	opd.inst = EQUAL;
	opLookup.insert(std::make_pair("==", opd));

	///////////////

	opd.precedence = order++;
	
	///////////

	opd.inst = GREATER_THAN;
	opLookup.insert(std::make_pair(">", opd));

	opd.inst = LESS_THAN;
	opLookup.insert(std::make_pair("<", opd));

	opd.inst = GREATER_EQUAL;
	opLookup.insert(std::make_pair(">=", opd));

	opd.inst = LESS_EQUAL;
	opLookup.insert(std::make_pair("<=", opd));

	///////////////

	opd.precedence = order++;
	
	/////////////////
	
	opd.inst = ADD;
	opLookup.insert(std::make_pair("+", opd));

	opd.inst = SUB;
	opLookup.insert(std::make_pair("-", opd));

	///////////////

	opd.precedence = order++;

	//////////////////

	opd.inst = MUL;
	opLookup.insert(std::make_pair("*", opd));

	opd.inst = DIV;
	opLookup.insert(std::make_pair("/", opd));

	///////////////

	typeLookup.insert(std::make_pair("int", QType::Int));

	Token t = peekNext();
	while (t.type != ETokenType::Eof) {
		cls->pushMethod(parseMethod());
		t = peekNext();
	}

}

Token& Parser::peekNext() {
	return tokens[index];
}

void Parser::Advance() {
	index++;
}

int Parser::calcPrecedence(Token& val) {
	std::string str(val.value);
	return opLookup[str].precedence;
}

bool Parser::isRightAssco(Token& val) {
	std::string str(val.value);
	return opLookup[str].isRightAssco;
}

Node* Parser::parseArguement() {
	Token lookAhead = peekNext();
	int commaCount = 0;
	std::vector<Node*> nodes;
	if (lookAhead.type == ETokenType::LParen) {
		Advance();
		while (1) {
			nodes.push_back(parseExpr());
			lookAhead = peekNext();
			if (lookAhead.type != ETokenType::Comma) {

				if (lookAhead.type != ETokenType::RParen) {
					std::cout << "RParen Expected\n";
				}
				else {
					Advance();
					return new ArgumentNode(nodes);
				}
			}
			else {
				Advance();
				commaCount++;
			}
		}
	}
}

Node* Parser::parseVariableDefinition() {
	Token t = peekNext();
	std::string ident;
	Advance();
	ident = t.value;
	if (t.type == ETokenType::Ident) {
		Token lookAhead = peekNext();
		if (lookAhead.type == ETokenType::Colon) {
			Advance();
			lookAhead = peekNext();
			if (lookAhead.type == ETokenType::Ident) {
				std::string s(lookAhead.value);
				if (typeLookup.find(s) != typeLookup.end()) {
					QType qt = typeLookup[s];
					return new VariableDefinition(ident, qt);
				}
				else
					std::cout << "need type expression\n";
			}
		}
		else
			std::cout << "colon needed\n";
	}
}

Node* Parser::parseScope()
{
	Token lookAhead = peekNext();
	tabCount++;
	
	int commaCount = 0;
	int tabs = 0;
	
	std::vector<Node*> nodes;
	while (lookAhead.type != ETokenType::Eof) {
		while (lookAhead.type == ETokenType::Tab) {
			Advance();
			lookAhead = peekNext();
			tabs++;
		}

		if (tabs < tabCount) {
			std::cout << tabCount << " scope ended\n";
			tabCount--;
			return new ScopeNode(nodes);
		}
		else {
			tabs = 0;
			nodes.push_back(parsePrimary());
		}
	}

}

FunctionDefinition* Parser::parseMethod() {
	Token lookAhead = peekNext();
	int commaCount = 0;
	std::vector<Node*> nodes;

	if (lookAhead.type == ETokenType::Ident && lookAhead.value == "def") {
		Advance();
		lookAhead = peekNext();
		std::string_view ident = lookAhead.value;

		Advance();
		lookAhead = peekNext();
		if (lookAhead.type == ETokenType::LParen) {
			Advance();
			while (1) {
				nodes.push_back(parseVariableDefinition());
				Advance();
				lookAhead = peekNext();
				if (lookAhead.type != ETokenType::Comma) {

					if (lookAhead.type != ETokenType::RParen) {
						std::cout << "RParen Expected\n";
					}
					else {
						Advance();
						lookAhead = peekNext();
						if (lookAhead.type == ETokenType::Colon) {
							Advance();
							lookAhead = peekNext();
							std::string s(lookAhead.value);
							if (typeLookup.find(s) != typeLookup.end()) {
								QType qt = typeLookup[s];
								Advance();
								return new FunctionDefinition(ident, qt, nodes, parseScope());
							}
							else
								std::cout << "need type expression\n";
						}
						else
							std::cout << "need colon\n";
					}
				}
				else {
					Advance();
					commaCount++;
				}
			}
		}
		else
			std::cout << "fahfsihug\n";
	}
	assert(false, "no def");
}

Node* Parser::parseVariable() {
	Token t = peekNext();
	std::string ident;
	ident = t.value;
	Advance();

	if (t.type == ETokenType::Ident && t.value == "ret")
		return parseReturn();

	if (t.type == ETokenType::Ident) {

		Token lookAhead = peekNext();
		if (lookAhead.type == ETokenType::LParen) {
			Node* arguement = parseArguement();
			return new FunctionCall(ident, arguement);
		}
		/*else if (lookAhead.type == ETokenType::Assign) {
			Node* expr = parseExpr();
			return new BinaryNode(new Identifier(ident), expr, ADD_R_O);
		}*/
		else
			return new Identifier(ident);
	}
	else
		std::cout << "wrong\n";
}

Node* Parser::parseReturn()
{
	Node* expr = parseExpr();
	return new ReturnNode(expr);
}

Node* Parser::parsePrimary() {
	Token t = peekNext();
	Node* node = nullptr;
	if (t.type == ETokenType::LParen) {
		Advance();
		Token lookAhead = peekNext();
		int commaCount = 0;
		node = parseExpr();
		lookAhead = peekNext();

		if (lookAhead.type != ETokenType::RParen) {
			std::cout << "RParen Expected\n";
		}
		Advance();
	}
	else if (t.type == ETokenType::Ident) {
		node = parseVariable();
	}
	else {
		Advance();
		node = new LiteralNode(atoi(t.value.data()));
	}
	return node;
}

Node* Parser::parseExpr() {
	return parseExpr_1(parsePrimary(), 0);
}

Node* Parser::parseExpr_1(Node* lhs, int minPrecedence) {

	Token lookAhead = peekNext();
	while (lookAhead.type == ETokenType::Binary && calcPrecedence(lookAhead) >= minPrecedence) {
		Token op = lookAhead;


		Advance();
		Node* rhs = parsePrimary();

		lookAhead = peekNext();

		while ((lookAhead.type == ETokenType::Binary) &&
			(calcPrecedence(lookAhead) > calcPrecedence(op) ||
				(isRightAssco(lookAhead) && calcPrecedence(op) == calcPrecedence(lookAhead)))
			) {
			int addPrece = 0;
			if (calcPrecedence(lookAhead) > calcPrecedence(op)) addPrece = 1;

			rhs = parseExpr_1(rhs, calcPrecedence(op) + addPrece);
			lookAhead = peekNext();
		}

		std::string opToFind(op.value);
		lhs = new BinaryNode(lhs, rhs, opLookup[opToFind].inst);
	}
	return lhs;
}

ClassDefinition* Parser::getTop()
{
	return cls;
}
