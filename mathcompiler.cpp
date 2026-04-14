#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <sstream>

// ─────────────────────────────────────────────
// TOKEN
// ─────────────────────────────────────────────
enum class TokenType { IDENT, NUMBER, LPAREN, RPAREN, COMMA, END };

struct Token {
    TokenType type;
    std::string value;
};

// ─────────────────────────────────────────────
// LEXER
// ─────────────────────────────────────────────
class Lexer {
    std::string src;
    size_t pos = 0;

    char peek() { return pos < src.size() ? src[pos] : '\0'; }
    char advance() { return src[pos++]; }

public:
    Lexer(const std::string& s) : src(s) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;

        while (pos < src.size()) {
            char c = peek();

            if (isspace(c)) { advance(); continue; }

            if (isalpha(c)) {
                std::string id;
                while (isalpha(peek())) id += advance();
                tokens.push_back({TokenType::IDENT, id});
                continue;
            }

            if (isdigit(c) || (c == '-' && isdigit(src[pos + 1]))) {
                std::string num;
                num += advance();
                while (isdigit(peek()) || peek() == '.') num += advance();
                tokens.push_back({TokenType::NUMBER, num});
                continue;
            }

            if (c == '(') tokens.push_back({TokenType::LPAREN, "("});
            else if (c == ')') tokens.push_back({TokenType::RPAREN, ")"});
            else if (c == ',') tokens.push_back({TokenType::COMMA, ","});
            else throw std::runtime_error("Invalid character");

            advance();
        }

        tokens.push_back({TokenType::END, ""});
        return tokens;
    }
};

// ─────────────────────────────────────────────
// AST
// ─────────────────────────────────────────────
enum class FormulaType { LINEAR, QUADRATIC, DISCRIMINANT };

struct Node {
    FormulaType type;
    std::vector<double> args;
};

// ─────────────────────────────────────────────
// PARSER
// ─────────────────────────────────────────────
class Parser {
    std::vector<Token> tokens;
    size_t pos = 0;

    Token& current() { return tokens[pos]; }

    void expect(TokenType t) {
        if (current().type != t)
            throw std::runtime_error("Unexpected token: " + current().value);
        pos++;
    }

    double parseNumber() {
        if (current().type != TokenType::NUMBER)
            throw std::runtime_error("Expected number");
        return std::stod(tokens[pos++].value);
    }

public:
    Parser(std::vector<Token> t) : tokens(std::move(t)) {}

    Node parse() {
        std::string name = current().value;
        expect(TokenType::IDENT);

        FormulaType type;
        if (name == "linear") type = FormulaType::LINEAR;
        else if (name == "quadratic") type = FormulaType::QUADRATIC;
        else if (name == "discriminant") type = FormulaType::DISCRIMINANT;
        else throw std::runtime_error("Unknown formula");

        expect(TokenType::LPAREN);

        std::vector<double> args;
        args.push_back(parseNumber());

        int count = (type == FormulaType::LINEAR) ? 2 : 3;

        for (int i = 1; i < count; i++) {
            expect(TokenType::COMMA);
            args.push_back(parseNumber());
        }

        expect(TokenType::RPAREN);
        return {type, args};
    }
};

// ─────────────────────────────────────────────
// SEMANTIC CHECK
// ─────────────────────────────────────────────
void check(const Node& node) {
    if (node.args[0] == 0)
        throw std::runtime_error("Coefficient 'a' cannot be zero");
}

// ─────────────────────────────────────────────
// EVALUATION
// ─────────────────────────────────────────────
void evaluate(const Node& n) {
    double a = n.args[0];
    double b = n.args[1];

    if (n.type == FormulaType::LINEAR) {
        double x = -b / a;
        std::cout << "Result: x = " << x << "\n";
        return;
    }

    double c = n.args[2];
    double d = b*b - 4*a*c;

    if (n.type == FormulaType::DISCRIMINANT) {
        std::cout << "Discriminant = " << d << "\n";
        return;
    }

    if (n.type == FormulaType::QUADRATIC) {
        if (d > 0) {
            double x1 = (-b + sqrt(d)) / (2*a);
            double x2 = (-b - sqrt(d)) / (2*a);
            std::cout << "x1 = " << x1 << ", x2 = " << x2 << "\n";
        } 
        else if (d == 0) {
            std::cout << "x = " << -b/(2*a) << "\n";
        } 
        else {
            double real = -b/(2*a);
            double imag = sqrt(-d)/(2*a);
            std::cout << "x1 = " << real << " + " << imag << "i\n";
            std::cout << "x2 = " << real << " - " << imag << "i\n";
        }
    }
}

// ─────────────────────────────────────────────
// DRIVER
// ─────────────────────────────────────────────
void run(const std::string& input) {
    try {
        Lexer lex(input);
        auto tokens = lex.tokenize();

        Parser parser(tokens);
        Node ast = parser.parse();

        check(ast);
        evaluate(ast);
    }
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}

// ─────────────────────────────────────────────
// MAIN (WITH WARNINGS)
// ─────────────────────────────────────────────
int main() {
    std::string choice;

    std::cout << "=== Math Solver ===\n";

    while (true) {
        std::cout << "\nChoose (linear / quadratic / discriminant / exit): ";
        std::cin >> choice;

        if (choice == "exit") break;

        std::cin.ignore();

        std::string line;
        double a, b, c;
        std::ostringstream input;

        if (choice == "linear") {
            std::cout << "⚠️  Linear formula requires EXACTLY 2 values (a, b)\n";
            std::cout << "Example: 2 4  OR  2,4\n";
            std::cout << "Enter a b: ";

            std::getline(std::cin, line);
            for (char &ch : line) if (ch == ',') ch = ' ';

            std::stringstream ss(line);

            if (!(ss >> a >> b) || (ss >> c)) {
                std::cout << "❌ Error: You must enter ONLY 2 values!\n";
                continue;
            }

            input << "linear(" << a << "," << b << ")";
        }
        else if (choice == "quadratic" || choice == "discriminant") {
            std::cout << "⚠️  This formula requires EXACTLY 3 values (a, b, c)\n";
            std::cout << "Example: 1 -5 6  OR  1,-5,6\n";
            std::cout << "Enter a b c: ";

            std::getline(std::cin, line);
            for (char &ch : line) if (ch == ',') ch = ' ';

            std::stringstream ss(line);

            if (!(ss >> a >> b >> c)) {
                std::cout << "❌ Error: You must enter EXACTLY 3 values!\n";
                continue;
            }

            double extra;
            if (ss >> extra) {
                std::cout << "❌ Error: Too many values entered!\n";
                continue;
            }

            input << choice << "(" << a << "," << b << "," << c << ")";
        }
        else {
            std::cout << "❌ Invalid choice!\n";
            continue;
        }

        run(input.str());
    }

    return 0;
}