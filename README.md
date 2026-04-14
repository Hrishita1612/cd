# 🧮 Math Formula Compiler (C++)

## 📌 Overview

This project is a **mini compiler** built using C++ that parses and evaluates mathematical formulas. It follows core concepts of **Compiler Design**, including:

* Lexical Analysis (Lexer)
* Syntax Analysis (Parser)
* Abstract Syntax Tree (AST)
* Semantic Analysis
* Evaluation (Code Generation)

The system allows users to compute results for multiple mathematical formulas interactively.

---

## 🚀 Supported Formulas

### 1. Linear Equation

Form:

```
ax + b = 0
```

Output:

```
x = -b / a
```

---

### 2. Quadratic Equation

Form:

```
ax² + bx + c = 0
```

Output:

* Two real roots
* One repeated root
* Complex roots

---

### 3. Discriminant

Form:

```
Δ = b² - 4ac
```

Used to determine nature of roots:

* Δ > 0 → Two real roots
* Δ = 0 → One root
* Δ < 0 → Complex roots

---

### 4. Circle Area

Form:

```
Area = πr²
```

---

### 5. Distance Formula

Form:

```
Distance = √((x₂ - x₁)² + (y₂ - y₁)²)
```

---

### 6. AP (Arithmetic Progression) – Nth Term

Form:

```
aₙ = a + (n - 1)d
```

---

## 🛠 How to Run

### 🔹 Step 1: Compile

```bash
g++ mathcompiler.cpp -o mathcompiler
```

### 🔹 Step 2: Run

```bash
./mathcompiler
```

---

## 📖 Usage

### Example:

```
Choose (linear / quadratic / discriminant / circle / distance / ap / exit):
```

---

### Sample Inputs

#### Linear

```
Input: 2 4
Output: x = -2
```

#### Quadratic

```
Input: 1 -5 6
Output: x1 = 3, x2 = 2
```

#### Circle Area

```
Input: r = 3
Output: Area = 28.27
```

#### Distance

```
Input: (1,2), (4,6)
Output: Distance = 5
```

#### AP Nth Term

```
Input: a=2, d=3, n=5
Output: 14
```

---

## ⚠️ Input Rules

* Linear → EXACTLY 2 values
* Quadratic / Discriminant → EXACTLY 3 values
* Distance → 4 values (x₁, y₁, x₂, y₂)
* Circle → 1 value (radius)
* AP → 3 values (a, d, n)

Supports:

* Space-separated input (`2 4`)
* Comma-separated input (`2,4`)

---

## 🧠 Compiler Design Structure

### 🔹 1. Lexer

* Converts input string into tokens

### 🔹 2. Parser

* Validates syntax
* Builds AST

### 🔹 3. AST (Abstract Syntax Tree)

* Represents formula structure

### 🔹 4. Semantic Analysis

* Checks:

  * Valid inputs
  * `a ≠ 0` where required

### 🔹 5. Evaluation

* Computes final result

---

## ❌ Error Handling

* Invalid formula name
* Incorrect number of inputs
* Invalid characters
* Division by zero
* Extra/missing parameters

---

##  Future Enhancements

* Support full equation input (e.g., `2x² - 5x + 6`)
* Graph plotting of functions
* GUI-based interface
* Integration with AI (RAG-based hint system)
* More formulas (integration, matrices, etc.)

---

## 👨‍💻 Author

Hrishita 
Anushka 
