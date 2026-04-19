#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <sstream>

#define _USE_MATH_DEFINES
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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
                while (isalpha(peek()) || peek() == '_') id += advance();
                tokens.push_back({TokenType::IDENT, id});
                continue;
            }

            if (isdigit(c) || (c == '-' && pos + 1 < src.size() && isdigit(src[pos + 1]))) {
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
enum class FormulaType {
    LINEAR,
    QUADRATIC,
    DISCRIMINANT,
    DISTANCE,       // NEW
    CIRCLE,         // NEW
    AP_TERM         // NEW
};

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
        int argCount = 0;

        if      (name == "linear")        { type = FormulaType::LINEAR;        argCount = 2; }
        else if (name == "quadratic")     { type = FormulaType::QUADRATIC;     argCount = 3; }
        else if (name == "discriminant")  { type = FormulaType::DISCRIMINANT;  argCount = 3; }
        else if (name == "distance")      { type = FormulaType::DISTANCE;      argCount = 4; }
        else if (name == "circle")        { type = FormulaType::CIRCLE;        argCount = 1; }
        else if (name == "apterm")        { type = FormulaType::AP_TERM;       argCount = 3; }
        else throw std::runtime_error("Unknown formula: " + name);

        expect(TokenType::LPAREN);

        std::vector<double> args;
        args.push_back(parseNumber());

        for (int i = 1; i < argCount; i++) {
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
    if (node.type == FormulaType::LINEAR ||
        node.type == FormulaType::QUADRATIC ||
        node.type == FormulaType::DISCRIMINANT) {
        if (node.args[0] == 0)
            throw std::runtime_error("Coefficient 'a' cannot be zero");
    }

    if (node.type == FormulaType::CIRCLE) {
        if (node.args[0] <= 0)
            throw std::runtime_error("Radius must be greater than 0");
    }

    if (node.type == FormulaType::AP_TERM) {
        double n = node.args[1];
        if (n < 1 || std::floor(n) != n)
            throw std::runtime_error("Term number 'n' must be a positive integer (>= 1)");
    }
}

// ─────────────────────────────────────────────
// EVALUATION
// ─────────────────────────────────────────────
void evaluate(const Node& n) {
    double a = n.args[0];
    double b = (n.args.size() > 1) ? n.args[1] : 0;

    // ── Linear: x = -b/a ────────────────────────────────────
    if (n.type == FormulaType::LINEAR) {
        std::cout << "\n  Step 1 : x = -b / a\n";
        std::cout << "  Step 2 : x = -(" << b << ") / " << a << "\n";
        double x = -b / a;
        std::cout << "  Result : x = " << x << "\n";
        return;
    }

    double c = (n.args.size() > 2) ? n.args[2] : 0;
    double d = b*b - 4*a*c;

    // ── Discriminant: D = b^2 - 4ac ─────────────────────────
    if (n.type == FormulaType::DISCRIMINANT) {
        std::cout << "\n  Step 1 : D = b^2 - 4ac\n";
        std::cout << "  Step 2 : D = (" << b << ")^2 - 4*(" << a << ")*(" << c << ")\n";
        std::cout << "  Step 3 : D = " << b*b << " - " << 4*a*c << "\n";
        std::cout << "  Result : D = " << d << "\n";
        if      (d > 0) std::cout << "  Info   : Two distinct real roots\n";
        else if (d == 0) std::cout << "  Info   : One repeated real root\n";
        else             std::cout << "  Info   : Two complex roots\n";
        return;
    }

    // ── Quadratic: x = (-b ± √D) / 2a ───────────────────────
    if (n.type == FormulaType::QUADRATIC) {
        std::cout << "\n  Step 1 : D = b^2 - 4ac = " << d << "\n";
        if (d > 0) {
            double x1 = (-b + sqrt(d)) / (2*a);
            double x2 = (-b - sqrt(d)) / (2*a);
            std::cout << "  Step 2 : sqrt(D) = " << sqrt(d) << "\n";
            std::cout << "  Step 3 : x1 = (-b + sqrt(D)) / 2a = " << x1 << "\n";
            std::cout << "  Step 4 : x2 = (-b - sqrt(D)) / 2a = " << x2 << "\n";
            std::cout << "  Result : x1 = " << x1 << ", x2 = " << x2 << "\n";
        }
        else if (d == 0) {
            double x = -b / (2*a);
            std::cout << "  Step 2 : x = -b / 2a = " << x << "\n";
            std::cout << "  Result : x = " << x << " (repeated root)\n";
        }
        else {
            double real = -b / (2*a);
            double imag = sqrt(-d) / (2*a);
            std::cout << "  Step 2 : Complex roots (D < 0)\n";
            std::cout << "  Step 3 : Real part = " << real << "\n";
            std::cout << "  Step 4 : Imaginary part = " << imag << "\n";
            std::cout << "  Result : x1 = " << real << " + " << imag << "i\n";
            std::cout << "           x2 = " << real << " - " << imag << "i\n";
        }
        return;
    }

    // ── Distance: d = sqrt((x2-x1)^2 + (y2-y1)^2) ──────────
    if (n.type == FormulaType::DISTANCE) {
        double x1 = n.args[0], y1 = n.args[1];
        double x2 = n.args[2], y2 = n.args[3];
        double dx = x2 - x1;
        double dy = y2 - y1;
        double dx2 = dx * dx;
        double dy2 = dy * dy;
        double dist = sqrt(dx2 + dy2);
        std::cout << "\n  Step 1 : dx = x2 - x1 = " << x2 << " - " << x1 << " = " << dx << "\n";
        std::cout << "  Step 2 : dy = y2 - y1 = " << y2 << " - " << y1 << " = " << dy << "\n";
        std::cout << "  Step 3 : dx^2 = " << dx2 << ",  dy^2 = " << dy2 << "\n";
        std::cout << "  Step 4 : dx^2 + dy^2 = " << (dx2 + dy2) << "\n";
        std::cout << "  Step 5 : d = sqrt(" << (dx2 + dy2) << ")\n";
        std::cout << "  Result : d = " << dist << "\n";
        return;
    }

    // ── Circle Area: A = pi * r^2 ────────────────────────────
    if (n.type == FormulaType::CIRCLE) {
        double r  = n.args[0];
        double r2 = r * r;
        double A  = M_PI * r2;
        std::cout << "\n  Step 1 : A = pi * r^2\n";
        std::cout << "  Step 2 : r^2 = " << r << "^2 = " << r2 << "\n";
        std::cout << "  Step 3 : A = " << M_PI << " * " << r2 << "\n";
        std::cout << "  Result : A = " << A << "\n";
        return;
    }

    // ── AP nth Term: a_n = a + (n-1)*d ──────────────────────
    if (n.type == FormulaType::AP_TERM) {
        double first = n.args[0];
        int    ni    = static_cast<int>(n.args[1]);
        double diff  = n.args[2];
        double nm1   = ni - 1;
        double nd    = nm1 * diff;
        double an    = first + nd;
        std::cout << "\n  Step 1 : a_n = a + (n-1) * d\n";
        std::cout << "  Step 2 : (n-1) = " << ni << " - 1 = " << nm1 << "\n";
        std::cout << "  Step 3 : (n-1)*d = " << nm1 << " * " << diff << " = " << nd << "\n";
        std::cout << "  Step 4 : a_" << ni << " = " << first << " + " << nd << "\n";
        std::cout << "  Result : a_" << ni << " = " << an << "\n";
        return;
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
// MAIN
// ─────────────────────────────────────────────
int main() {
    std::string choice;

    std::cout << "========================================\n";
    std::cout << "         Math Formula Compiler\n";
    std::cout << "========================================\n";

    while (true) {
        std::cout << "\nChoose formula:\n";
        std::cout << "  linear / quadratic / discriminant\n";
        std::cout << "  distance / circle / apterm / exit\n";
        std::cout << "> ";
        std::cin >> choice;

        if (choice == "exit") break;

        std::cin.ignore();

        std::string line;
        double a, b, c, d_val, extra;
        std::ostringstream input;

        // ── Linear ──────────────────────────────────────────
        if (choice == "linear") {
            std::cout << "  Requires: a, b  (2 values)\n";
            std::cout << "  Formula : x = -b / a\n";
            std::cout << "  Example : 2 4\n";
            std::cout << "  Enter a b: ";
            std::getline(std::cin, line);
            for (char& ch : line) if (ch == ',') ch = ' ';
            std::stringstream ss(line);
            if (!(ss >> a >> b) || (ss >> extra)) {
                std::cout << "Error: Enter EXACTLY 2 values!\n"; continue;
            }
            input << "linear(" << a << "," << b << ")";
        }

        // ── Quadratic / Discriminant ─────────────────────────
        else if (choice == "quadratic" || choice == "discriminant") {
            std::cout << "  Requires: a, b, c  (3 values)\n";
            std::cout << "  Example : 1 -5 6\n";
            std::cout << "  Enter a b c: ";
            std::getline(std::cin, line);
            for (char& ch : line) if (ch == ',') ch = ' ';
            std::stringstream ss(line);
            if (!(ss >> a >> b >> c)) {
                std::cout << "Error: Enter EXACTLY 3 values!\n"; continue;
            }
            if (ss >> extra) {
                std::cout << "Error: Too many values!\n"; continue;
            }
            input << choice << "(" << a << "," << b << "," << c << ")";
        }

        // ── Distance ─────────────────────────────────────────
        else if (choice == "distance") {
            std::cout << "  Requires: x1, y1, x2, y2  (4 values)\n";
            std::cout << "  Formula : d = sqrt((x2-x1)^2 + (y2-y1)^2)\n";
            std::cout << "  Example : 0 0 3 4\n";
            std::cout << "  Enter x1 y1 x2 y2: ";
            std::getline(std::cin, line);
            for (char& ch : line) if (ch == ',') ch = ' ';
            std::stringstream ss(line);
            if (!(ss >> a >> b >> c >> d_val)) {
                std::cout << "Error: Enter EXACTLY 4 values!\n"; continue;
            }
            if (ss >> extra) {
                std::cout << "Error: Too many values!\n"; continue;
            }
            input << "distance(" << a << "," << b << "," << c << "," << d_val << ")";
        }

        // ── Circle ───────────────────────────────────────────
        else if (choice == "circle") {
            std::cout << "  Requires: r  (1 value — radius)\n";
            std::cout << "  Formula : A = pi * r^2\n";
            std::cout << "  Example : 7\n";
            std::cout << "  Enter r: ";
            std::getline(std::cin, line);
            std::stringstream ss(line);
            if (!(ss >> a)) {
                std::cout << "Error: Enter a valid radius!\n"; continue;
            }
            if (ss >> extra) {
                std::cout << "Error: Enter ONLY 1 value!\n"; continue;
            }
            input << "circle(" << a << ")";
        }

        // ── AP nth Term ──────────────────────────────────────
        else if (choice == "apterm") {
            std::cout << "  Requires: a, n, d  (3 values)\n";
            std::cout << "  Formula : a_n = a + (n-1) * d\n";
            std::cout << "  a = first term, n = term number, d = common difference\n";
            std::cout << "  Example : 2 10 3\n";
            std::cout << "  Enter a n d: ";
            std::getline(std::cin, line);
            for (char& ch : line) if (ch == ',') ch = ' ';
            std::stringstream ss(line);
            if (!(ss >> a >> b >> c)) {
                std::cout << "Error: Enter EXACTLY 3 values!\n"; continue;
            }
            if (ss >> extra) {
                std::cout << "Error: Too many values!\n"; continue;
            }
            input << "apterm(" << a << "," << b << "," << c << ")";
        }

        else {
            std::cout << "Error: Invalid choice!\n";
            continue;
        }

        run(input.str());
    }

    std::cout << "\nGoodbye!\n";
    return 0;
}