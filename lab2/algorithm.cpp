//В строке текстового файла  задано  выражение  из  целых
//чисел и операций '+', '-', '*', '/', '^', SIN, COS, EXP.Порядок
//вычислений  определяется  приоритетом  операций   и   круглыми
//скобками.Возможен одноместный минус в  начале  выражения  или
//после открывающей скобки.Преобразовать выражение в постфиксную
//форму(алгоритм Дейкстры) и вычислить его  значение.Показать
//этапы  выполнения(11).
// 
//Общими требованиями к лабораторной работе являются :
//1) организовать  ввод  данных  из  файла  в   понятной  для
//пользователя форме;
//2) обеспечить   возможность   многократных   запросов   без
//повторного запуска программы;
//3) при реализации в С++ не использовать контейнерные классы
//для работы с линейными списками типа stack, queue и т.п.
//

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <cmath>

void error(std::string e) {
    std::cout << std::endl;
    std::cout << "Ошибка: " << e << std::endl;
    exit(1);
}

struct St
{
    std::string element;
    St* next;
};

void push(St*& p, std::string el)  
{
    St* t = new St;
    t->element = el;
    t->next = p;
    p = t;
}

void pop(St*& p)             
{
    if (p == nullptr) return;
    St* t = p;
    p = p->next;
    delete t;
}

std::string returnTop(St* p)     
{
    if (p == nullptr) return "";
    return p->element;
}

void clear(St*& p)           
{
    while (p != nullptr)
    {
        St* t = p;
        p = p->next;
        delete t;
    }
}

bool isOperator(const std::string& element) {
    return element == "+" || element == "-" || element == "*" || element == "/" ||
        element == "^" || element == "SIN" || element == "COS" || element == "EXP" || element == "~";
}

bool isLeftAssociative(const std::string& element) {
    return element == "+" || element == "-" || element == "*" || element == "/";
}

bool isRightAssociative(const std::string& element) {
    return element == "^" || element == "SIN" || element == "COS" || element == "EXP" || element == "~";
}

int operatorPriority(const std::string& element) {
    if (element == "+" || element == "-") return 1;
    if (element == "*" || element == "/") return 2;
    if (element == "^") return 3;
    if (element == "SIN" || element == "COS" || element == "EXP") return 4;
    if (element == "~") return 5;
    return 0;
}

bool isNumber(const std::string& str) {
    if (str[0] == '~') {
        if (str.length() == 1) return false;
        for (size_t i = 1; i < str.length(); i++) {
            if (!std::isdigit(str[i]) && str[i] != '.') return false;
        }
        return true;
    }

    for (char d : str) {
        if (!std::isdigit(d) && d != '.') return false;
    }

    return true;
}

std::string addSpacesAroundOperators(const std::string& expression) {
    std::string result = expression;
    result = std::regex_replace(result, std::regex("^\\s+"), "");
    if (!result.empty() && result[0] == '-') result[0] = '~';
    result = std::regex_replace(result, std::regex("\\(-"), "(~");
    result = std::regex_replace(result, std::regex("~"), " ~ ");
    result = std::regex_replace(result, std::regex("\\("), " ( ");
    result = std::regex_replace(result, std::regex("\\)"), " ) ");
    result = std::regex_replace(result, std::regex("\\+"), " + ");
    result = std::regex_replace(result, std::regex("\\-"), " - ");
    result = std::regex_replace(result, std::regex("\\*"), " * ");
    result = std::regex_replace(result, std::regex("\\/"), " / ");
    result = std::regex_replace(result, std::regex("\\^"), " ^ ");
    result = std::regex_replace(result, std::regex("SIN"), "SIN ");
    result = std::regex_replace(result, std::regex("COS"), "COS ");
    result = std::regex_replace(result, std::regex("EXP"), "EXP ");

    result = std::regex_replace(result, std::regex("\\s+"), " ");

    return result;
}

std::string convertPostfixForm(std::string expression) {
    std::string betterExpression = addSpacesAroundOperators(expression);
    /*std::cout << betterExpression << std::endl;*/

    std::string postfixForm = "";
    St* stack = nullptr;
    std::string element = "";

    for (size_t i = 0; i <= betterExpression.length(); i++) {
        if (betterExpression[i] == ' ' || i == betterExpression.length()) {
            if (element.empty()) continue;
            if (isNumber(element)) {
                postfixForm += element + " ";
                std::cout << element + " - печатаем" << std::endl;
            }
            else if (element == "(") {
                push(stack, element);
                std::cout << element + " - вносим в стек" << std::endl;
            }
            else if (element == ")") {
                std::cout << element + " - закрывающая скобка => печатаем всё из стека до (, удаляем (: ";
                while (stack != nullptr && returnTop(stack) != "(") {
                    postfixForm += returnTop(stack) + " ";
                    std::cout << returnTop(stack) + " ";
                    pop(stack);
                }
                if (stack != nullptr && returnTop(stack) == "(") {
                    std::cout << returnTop(stack);
                    pop(stack);
                }
                else {
                    error("Лишний элемент в стеке: )");
                }

                std::cout << std::endl;
            }
            else if (isOperator(element)) {
                std::cout << element + " - оператор в стек" << std::endl;
                while (stack != nullptr && isOperator(returnTop(stack))) {
                    std::string top = returnTop(stack);

                    if (isLeftAssociative(element) && isLeftAssociative(top)) {
                        if (operatorPriority(element) <= operatorPriority(top)) {
                            postfixForm += top + " ";
                            std::cout << top + " - печатаем" << std::endl;
                            pop(stack);
                        }
                        else {
                            break;
                        }
                    }
                    else if (isRightAssociative(element) && isRightAssociative(top)) {
                        if (operatorPriority(element) < operatorPriority(top)) {
                            postfixForm += top + " ";
                            std::cout << top + " - печатаем" << std::endl;
                            pop(stack);
                        }
                        else {
                            break;
                        }
                    }
                    else {
                        if (operatorPriority(element) <= operatorPriority(top)) {
                            postfixForm += top + " ";
                            std::cout << top + " - печатаем" << std::endl;
                            pop(stack);
                        }
                        else {
                            break;
                        }
                    }
                }
                push(stack, element);
            }
            else {
                error("Найдено ни число и ни оператор " + element);
            }
            element = "";

        }
        else {
            element += betterExpression[i];
        }
        
    }

    while (stack != nullptr) {
        if (returnTop(stack) != "(" && returnTop(stack) != ")") {
            postfixForm += returnTop(stack) + " ";
            std::cout << returnTop(stack) + " - печатаем" << std::endl;
        }
        else {
            error("Лишний элемент в стеке: " + returnTop(stack));
        }

        pop(stack);
    }

    clear(stack);
    return postfixForm;
}

double calculate(double twoDigit, std::string operatorExpression, double oneDigit = 0) {
    if (operatorExpression == "+") return oneDigit + twoDigit;
    if (operatorExpression == "-") return oneDigit - twoDigit;
    if (operatorExpression == "*") return oneDigit * twoDigit;
    if (operatorExpression == "/") {
        if (twoDigit != 0) return oneDigit / twoDigit;
        else error("На 0 делить нельзя");
    }
    if (operatorExpression == "^") return pow(oneDigit, twoDigit);
    if (operatorExpression == "SIN") return sin(twoDigit);
    if (operatorExpression == "COS") return cos(twoDigit);
    if (operatorExpression == "EXP") return exp(twoDigit);
    if (operatorExpression == "~") return -twoDigit;

    return 0;
}

double calculatePostfixForm(std::string expression) {
    double result = 0;
    St* stack = nullptr;
    std::string element = "";
    double twoDigit = 0;
    double oneDigit = 0;

    for (size_t i = 0; i <= expression.length(); i++) {
        if (expression[i] == ' ' || i == expression.length()) {
            if (element.empty()) continue;

            if (isOperator(element)) {
                if (element == "+" || element == "-" || element == "*" || element == "/" || element == "^") {
                    if (stack != nullptr) {
                        twoDigit = std::stod(returnTop(stack));
                        pop(stack);
                        if (stack != nullptr) {
                            oneDigit = std::stod(returnTop(stack));
                            pop(stack);
                            double calcResult = calculate(twoDigit, element, oneDigit);
                            push(stack, std::to_string(calcResult));
                            std::cout << "Вычисляем: " << oneDigit << " " << element << " " << twoDigit << " = " << calcResult << std::endl;
                        }
                        else {
                            error("Недостаточно операндов для оператора " + element);
                        }
                    }
                    else {
                        error("Недостаточно операндов для оператора " + element);
                    }
                }
                else {
                    if (stack != nullptr) {
                        oneDigit = std::stod(returnTop(stack));
                        pop(stack);
                        double calcResult = calculate(oneDigit, element);
                        push(stack, std::to_string(calcResult));
                        std::cout << "Вычисляем: " << element << " " << oneDigit << " = " << calcResult << std::endl;
                    }
                    else {
                        error("Недостаточно операндов для оператора " + element);
                    }
                }
            }
            else {
                push(stack, element);
            }
            element = "";
            oneDigit = 0;
        }
        else {
            element += expression[i];
        }
    }
    if (stack != nullptr) {
        result = std::stod(returnTop(stack));
        pop(stack);
        if (stack != nullptr) {
            error("Некорректное выражение(лишняя цифра): " + returnTop(stack));
        }
    }

    clear(stack);
    return result;
}

int main()
{
    setlocale(LC_ALL, "");

    std::string fileName;
    std::cout << "Введите файл для чтения: ";
    std::cin >> fileName;
    std::ifstream in(fileName);

    if (in.is_open())
    {
        bool flagWork = true;
        std::string expression;

        while (flagWork) {
            if (std::getline(in, expression)) {
                std::cout << "Вы ввели: " << expression << std::endl;
                if (expression.empty()) {
                    error("Пустая строка");
                }
                std::string resultExpression = convertPostfixForm(expression);
                std::cout << "Постфиксная форма: " << resultExpression << std::endl;
                double result = calculatePostfixForm(resultExpression);
                std::cout << "Итоговая сумма: " << result << std::endl;
                std::cout << "" << std::endl;
            }
            else {
                std::cout << "Достигнут конец файла!" << std::endl;
                break;
            }

            std::cout << "Вы хотите продолжить?(y/n): ";
            std::string answer;
            std::cin >> answer;
            std::cout << "" << std::endl;
            if (answer != "y" && answer != "Y") {
                flagWork = false;
            }
        }
    }
    else {
        error("Ошибка открытия файла!");
    }
    in.close();

    return 0;
}