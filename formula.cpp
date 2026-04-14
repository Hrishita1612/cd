#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <sstream>

// ─────────────────────────────────────────────
// 1. TOKEN
// ─────────────────────────────────────────────
enum class TokenType {
    IDENT,      // linear, quadratic, discriminant
    NUMBER,     // 1, -3.5, 2
    LPAREN,     // (
    RPAREN,     // )
    COMMA,      // ,
    MINUS,      // standalone minus for negatives
    END
};

struct Token {
    TokenType type;
    std::string value;
};

// ─────────────────────────────────────────────
// 2. LEXER
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

            // skip whitespace
            if (isspace(c)) { advance(); continue; }

            // identifier: linear, quadratic, discriminant
            if (isalpha(c)) {
                std::string id;
                while (isalpha(peek())) id += advance();
                tokens.push_back({TokenType::IDENT, id});
                continue;
            }

            // number: supports decimals and negatives
            if (isdigit(c) || (c == '-' && isdigit(src[pos+1]))) {
                std::string num;
                num += advance();  // '-' or first digit
                while (isdigit(peek()) || peek() == '.') num += advance();
                tokens.push_back({TokenType::NUMBER, num});
                continue;
            }

            switch (c) {
                case '(': tokens.push_back({TokenType::LPAREN,  "("}); break;
                case ')': tokens.push_back({TokenType::RPAREN,  ")"}); break;
                case ',': tokens.push_back({TokenType::COMMA,   ","}); break;
                case '-': tokens.push_back({TokenType::MINUS,   "-"}); break;
                default:
                    throw std::runtime_error(std::string("Unknown character: ") + c);
            }
            advance();
        }

        tokens.push_back({TokenType::END, ""});
        return tokens;
    }
};

// ─────────────────────────────────────────────
// 3. AST NODE
// ─────────────────────────────────────────────
enum class FormulaType { LINEAR, QUADRATIC, DISCRIMINANT };

struct MathExprNode {
    FormulaType formula;
    std::vector<double> args;  // a, b  or  a, b, c
};

// ─────────────────────────────────────────────
// 4. PARSER
// ─────────────────────────────────────────────
class Parser {
    std::vector<Token> tokens;
    size_t pos = 0;

    Token& current() { return tokens[pos]; }

    Token expect(TokenType type) {
        if (current().type != type)
            throw std::runtime_error("Unexpected token: " + current().value);
        return tokens[pos++];
    }

    double parseNumber() {
        // Handle negatives preceded by MINUS token or NUMBER token with '-'
        double sign = 1.0;
        if (current().type == TokenType::MINUS) {
            sign = -1.0;
            pos++;
        }
        if (current().type != TokenType::NUMBER)
            throw std::runtime_error("Expected number, got: " + current().value);
        double val = std::stod(tokens[pos++].value);
        return sign * val;
    }

public:
    Parser(std::vector<Token> toks) : tokens(std::move(toks)) {}

    MathExprNode parse() {
        if (current().type != TokenType::IDENT)
            throw std::runtime_error("Expected formula name (linear/quadratic/discriminant)");

        std::string name = tokens[pos++].value;
        FormulaType type;

        if      (name == "linear")        type = FormulaType::LINEAR;
        else if (name == "quadratic")     type = FormulaType::QUADRATIC;
        else if (name == "discriminant")  type = FormulaType::DISCRIMINANT;
        else throw std::runtime_error("Unknown formula: " + name);

        expect(TokenType::LPAREN);

        std::vector<double> args;
        args.push_back(parseNumber());

        size_t expectedArgs = (type == FormulaType::LINEAR) ? 2 : 3;
        for (size_t i = 1; i < expectedArgs; ++i) {
            expect(TokenType::COMMA);
            args.push_back(parseNumber());
        }

        expect(TokenType::RPAREN);
        return {type, args};
    }
};

// ─────────────────────────────────────────────
// 5. SEMANTIC ANALYSIS
// ─────────────────────────────────────────────
void semanticCheck(const MathExprNode& node) {
    if (node.formula == FormulaType::LINEAR) {
        if (node.args[0] == 0)
            throw std::runtime_error("Semantic error: coefficient 'a' cannot be 0 in linear(a, b)");
    }
    if (node.formula == FormulaType::QUADRATIC || node.formula == FormulaType::DISCRIMINANT) {
        if (node.args[0] == 0)
            throw std::runtime_error("Semantic error: coefficient 'a' cannot be 0 in quadratic/discriminant(a, b, c)");
    }
}

// ─────────────────────────────────────────────
// 6. EVALUATOR / CODE GEN
// ─────────────────────────────────────────────
void evaluate(const MathExprNode& node) {
    double a = node.args[0];
    double b = node.args[1];

    if (node.formula == FormulaType::LINEAR) {
        // ax + b = 0  →  x = -b/a
        double x = -b / a;
        std::cout << "  Linear equation:  " << a << "x + " << b << " = 0\n";
        std::cout << "  Formula:          x = -b / a\n";
        std::cout << "  Result:           x = " << x << "\n";
        return;
    }

    double c = node.args[2];
    double discriminant = (b * b) - (4 * a * c);

    if (node.formula == FormulaType::DISCRIMINANT) {
        std::cout << "  Discriminant for: " << a << "x² + " << b << "x + " << c << " = 0\n";
        std::cout << "  Formula:          Δ = b² - 4ac\n";
        std::cout << "  Result:           Δ = " << discriminant << "\n";
        if      (discriminant > 0) std::cout << "  → Two distinct real roots\n";
        else if (discriminant == 0) std::cout << "  → One repeated real root\n";
        else                        std::cout << "  → No real roots (complex)\n";
        return;
    }

    if (node.formula == FormulaType::QUADRATIC) {
        std::cout << "  Quadratic:        " << a << "x² + " << b << "x + " << c << " = 0\n";
        std::cout << "  Formula:          x = (-b ± √(b²−4ac)) / 2a\n";
        std::cout << "  Discriminant Δ  = " << discriminant << "\n";

        if (discriminant > 0) {
            double x1 = (-b + std::sqrt(discriminant)) / (2 * a);
            double x2 = (-b - std::sqrt(discriminant)) / (2 * a);
            std::cout << "  Result:           x1 = " << x1 << ",  x2 = " << x2 << "\n";
        } else if (discriminant == 0) {
            double x = -b / (2 * a);
            std::cout << "  Result:           x = " << x << " (repeated root)\n";
        } else {
            double realPart = -b / (2 * a);
            double imagPart = std::sqrt(-discriminant) / (2 * a);
            std::cout << "  Result:           x1 = " << realPart << " + " << imagPart << "i\n";
            std::cout << "                    x2 = " << realPart << " - " << imagPart << "i\n";
        }
    }
}

// ─────────────────────────────────────────────
// 7. COMPILER DRIVER
// ─────────────────────────────────────────────
void compile(const std::string& source) {
    std::cout << "\n>>> Input: " << source << "\n";
    try {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();

        Parser parser(std::move(tokens));
        auto ast = parser.parse();

        semanticCheck(ast);
        evaluate(ast);
    } catch (const std::exception& e) {
        std::cerr << "  Error: " << e.what() << "\n";
    }
    std::cout << "\n";
}

// ─────────────────────────────────────────────
// 8. MAIN
// ─────────────────────────────────────────────
int main() {
    // Linear:  2x - 6 = 0  → x = 3
    compile("linear(2, -6)");

    // Quadratic: x² - 5x + 6 = 0  → x=3, x=2
    compile("quadratic(1, -5, 6)");

    // Quadratic: x² + 2x + 5 = 0  → complex roots
    compile("quadratic(1, 2, 5)");

    // Discriminant only
    compile("discriminant(1, -3, 2)");

    // Repeated root: x² - 4x + 4 = 0
    compile("quadratic(1, -4, 4)");

    // Error: a=0 is invalid
    compile("quadratic(0, 2, 1)");

    return 0;
}