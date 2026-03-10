//22. При  заданном  четном  N(N <= 16)  перечислить  все
//правильные скобочные формы длины N из скобок '(', ')', '[', ']',
//'{', '}'.Скобки '{}' не должны быть внутри '[]' и '()', а скобки
//'[]' не могут быть внутри '()'.Время счета не более 2 сек. (10).
//Например, для N = 4 правильные формы :
//(())
//()()
//()[]
//(){}
//[()]
//[[]]
//[]()
//[][]
//[]{}
//{ () }
//{ [] }
//{ {} }
//{}()
//{}[]
//{} {}

#include <iostream>
#include <fstream>
#include <deque>
#include <vector>
#include <string>
#include <stack>
#include <algorithm>
#include <chrono>

std::ofstream out("out.txt");
std::deque<char> allBrackets{ '(', '[', '{' };

void Error(const std::string& e) {
    std::cout << std::endl;
    std::cout << "Ошибка: " << e << std::endl;
    exit(1);
}

bool IsNeedNumber(int number) {
    return number >= 0 && number <= 16 && number % 2 == 0;
}

char RightBracket(char leftBracket) {
    switch (leftBracket) {
    case '(': return ')';
    case '[': return ']';
    case '{': return '}';
    default: return '\0';
    }
}

bool IsCanAddBracket(char beforBracket, char afterBracket) {
    if (beforBracket == '\0') return true;
    if (beforBracket == '(') return afterBracket == '(';
    if (beforBracket == '[') return afterBracket == '(' || afterBracket == '[';
    if (beforBracket == '{') return true;
    return false;
}

void PrintOption(const std::string& option) {
    out << option << '\n';
}

void GenerateSequences(std::string& current,
    int openCount, int closeCount,
    int countLeftBrackets,
    std::stack<char>& bracketStack,
    char lastOpen, int& countOptions) {

    if (current.length() == countLeftBrackets * 2) {
        PrintOption(current);
        countOptions += 1;
        return;
    }

    if (openCount < countLeftBrackets) {
        for (char bracket : allBrackets) {
            if (IsCanAddBracket(lastOpen, bracket)) {  
                current.push_back(bracket);
                bracketStack.push(bracket);

                GenerateSequences(current, openCount + 1, closeCount,
                    countLeftBrackets, bracketStack, bracket, countOptions);

                current.pop_back();
                bracketStack.pop();
            }
        }
    }

    if (closeCount < openCount && !bracketStack.empty()) {
        char lastOpenBracket = bracketStack.top();
        char closing = RightBracket(lastOpenBracket);

        current.push_back(closing);
        bracketStack.pop();

        char prevOpen = bracketStack.empty() ? '\0' : bracketStack.top();

        GenerateSequences(current, openCount, closeCount + 1,
            countLeftBrackets, bracketStack, prevOpen, countOptions);

        current.pop_back();
        bracketStack.push(lastOpenBracket);
    }
}

int WorkWithBrackets(int countBrackets) {
    if (countBrackets == 0) {
        return 0;
    }

    int countLeftBrackets = countBrackets / 2;
    std::string current;
    std::stack<char> bracketStack;
    int countOptions = 0;

    GenerateSequences(current, 0, 0, countLeftBrackets, bracketStack, '\0', countOptions);

    return countOptions;
}

int main() {
    setlocale(LC_ALL, "");

    if (!out.is_open()) {
        Error("Не удалось открыть файл для записи!");
    }

    int countBrackets;
    std::cout << "Введите чётное число <= 16: ";
    std::cin >> countBrackets;

    auto start = std::chrono::high_resolution_clock::now(); //начало

    if (!IsNeedNumber(countBrackets)) {
        Error("Число должно быть четным и в диапазоне [0, 16]");
    }

    int countOptions = WorkWithBrackets(countBrackets);

    auto end = std::chrono::high_resolution_clock::now(); //конец
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "\nВремя выполнения: " << duration.count() / 1000.0 << " сек" << std::endl;
    std::cout << "Найдено " << countOptions << " правильных скобочных форм" << std::endl;

    out.close();
    std::cout << "Результаты сохранены в файл" << std::endl;

    return 0;
}