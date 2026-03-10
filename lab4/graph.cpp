//Общими требованиями к лабораторной работе являются :
//1) вводить  граф из файла в понятной для пользователя форме
//(не в виде матрицы смежности, без дублирования информации и т.п.);
//2) обеспечить   возможность   многократных   запросов   без
//повторного запуска программы.

//Имеется сеть автомобильных  дорог с двусторонним движением.
//Для каждой дороги известна максимальная возможная масса провозимого
//по ней груза.С помощью алгоритма Дейкстры определить максимальный
//груз, который можно доставить  между  двумя  указанными  городами,
//заезжая по пути в заданный промежуточный город(9).

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <regex>
#include <conio.h>

struct Road {
    int to;
    int maxWeight;
};

struct Settlement {
    int number;
    std::string name;
    std::vector<Road> roads;
};

struct PathResult {
    int maxWeight;
    std::vector<int> path;
};

void Error(const std::string& e) {
    std::cout << std::endl;
    std::cout << "Ошибка: " << e << std::endl;
    exit(1);
}

int SafeInputInt(const std::string& prompt) {
    int value;
    std::string input;

    while (true) {
        std::cout << prompt;
        std::cin >> input;

        bool isValid = true;
        for (char c : input) {
            if (!std::isdigit(c)) {
                isValid = false;
                break;
            }
        }

        if (isValid) {
            try {
                value = std::stoi(input);
                break;
            }
            catch (const std::exception&) {
                std::cout << "Ошибка: Некорректный ввод числа. Оно слишком большое\n";
            }
        }
        else {
            std::cout << "Ошибка: Ввод должен содержать только положительные цифры. Пожалуйста, попробуйте снова.\n";
        }

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    return value;
}

int SafeInputChoice(const std::string& prompt) {
    int value;
    std::string input;

    while (true) {
        std::cout << prompt;
        std::cin >> input;

        if (input.length() == 1 && std::isdigit(input[0])) {
            value = std::stoi(input);
            if (value >= 1 && value <= 4) {
                break;
            }
        }

        std::cout << "Ошибка: Пожалуйста, введите число от 1 до 4.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    return value;
}

void LoadSettlements(std::ifstream& input, std::vector<Settlement>& settlements) {
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        int number;
        std::string name;

        if (iss >> number) {
            std::getline(iss, name);
            name = std::regex_replace(name, std::regex("^\\s+"), "");
            name = std::regex_replace(name, std::regex("\\s+$"), "");

            bool settlementsFound = false;
            for (const auto& settlement : settlements) {
                if (settlement.number == number) {
                    settlementsFound = true;
                    break;
                }
            }

            if (!settlementsFound) {
                Settlement settlement;
                settlement.number = number;
                settlement.name = name;
                settlements.push_back(settlement);
            }
        }
    }
}

void AddRoad(std::vector<Settlement>& settlements, int from, int to, int weight) {
    for (auto& settlement : settlements) {
        if (settlement.number == from) {
            bool roadFound = false;
            for (const auto& road : settlement.roads) {
                if (road.to == to) {
                    roadFound = true;
                    break;
                }
            }

            if (!roadFound) {
                Road road;
                road.to = to;
                road.maxWeight = weight;
                settlement.roads.push_back(road);
            }
            break;
        }
    }
}

void LoadRoads(std::ifstream& input, std::vector<Settlement>& settlements) {
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        int from, to, weight;

        if (iss >> from >> to >> weight) {
            bool fromFound = false;
            bool toFound = false;
            for (const auto& settlement : settlements) {
                if (settlement.number == from) fromFound = true;
                if (settlement.number == to) toFound = true;
            }

            if (fromFound && toFound) {
                AddRoad(settlements, from, to, weight);
                AddRoad(settlements, to, from, weight);
            }
        }
    }
}

const Settlement& GetSettlement(const std::vector<Settlement>& settlements, int number) {
    for (const auto& settlement : settlements) {
        if (settlement.number == number) {
            return settlement;
        }
    }
    Error("Населенный пункт не найден: " + std::to_string(number));
}

bool IsFoundSettlement(const std::vector<Settlement>& settlements, int number) {
    for (const auto& settlement : settlements) {
        if (settlement.number == number) {
            return true;
        }
    }
    return false;
}

PathResult FindMaxWeightPath(const std::vector<Settlement>& settlements, int start, int end, int alreadyVisited = 0) {
    if (start == end) {
        Error("Начальная и конечные точки совпадают");
    }

    int maxNumber = 0;
    for (const auto& settlement : settlements) {
        if (settlement.number > maxNumber) {
            maxNumber = settlement.number;
        }
    }

    std::vector<int> maxWeights(maxNumber + 1, 0);
    std::vector<int> previous(maxNumber + 1, -1);
    std::vector<bool> visited(maxNumber + 1, false);

    maxWeights[start] = std::numeric_limits<int>::max();

    for (size_t i = 0; i < settlements.size(); i++) {
        int current = -1;
        int currentMaxWeight = -1;

        for (const auto& settlement : settlements) {
            int num = settlement.number;
            if (!visited[num] && maxWeights[num] > currentMaxWeight) {
                currentMaxWeight = maxWeights[num];
                current = num;
            }
        }

        if (current == -1) break;
        if (current == end) break;

        visited[current] = true;

        const Settlement& currentSettlement = GetSettlement(settlements, current);
        for (const auto& road : currentSettlement.roads) {
            if (!visited[road.to] && (alreadyVisited == 0 || alreadyVisited != road.to)) {
                int possibleWeight = std::min(maxWeights[current], road.maxWeight);
                if (possibleWeight > maxWeights[road.to]) {
                    maxWeights[road.to] = possibleWeight;
                    previous[road.to] = current;
                }
            }
        }
    }

    std::vector<int> path;
    if (maxWeights[end] > 0) {
        for (int from = end; from != -1; from = previous[from]) {
            path.push_back(from);
        }
        std::reverse(path.begin(), path.end());
    }

    return { maxWeights[end], path };
}

PathResult FindMaxWeightPathViaIntermediate(const std::vector<Settlement>& settlements,
    int start, int intermediate, int end) {
    auto firstLeg = FindMaxWeightPath(settlements, start, intermediate, end);
    if (firstLeg.maxWeight == 0) {
        return { 0, {} };
    }

    auto secondLeg = FindMaxWeightPath(settlements, intermediate, end, start);
    if (secondLeg.maxWeight == 0) {
        return { 0, {} };
    }

    int totalMaxWeight = std::min(firstLeg.maxWeight, secondLeg.maxWeight);

    std::vector<int> fullPath = firstLeg.path;
    fullPath.insert(fullPath.end(), secondLeg.path.begin() + 1, secondLeg.path.end());

    return { totalMaxWeight, fullPath };
}

void PrintSettlements(const std::vector<Settlement>& settlements) {
    std::cout << "Доступные населенные пункты:\n";
    for (const auto& settlement : settlements) {
        std::cout << settlement.number << " - " << settlement.name << "\n";
    }
    std::cout << std::endl;
}

void PrintPath(const std::vector<Settlement>& settlements, const PathResult& result) {
    if (result.maxWeight == 0) {
        std::cout << "Путь не найден!\n";
        return;
    }

    std::cout << "Максимальный вес: " << result.maxWeight << "\n";
    std::cout << "Маршрут: ";
    for (size_t i = 0; i < result.path.size(); ++i) {
        if (i > 0) std::cout << " -> ";
        int num = result.path[i];
        std::cout << num << "(" << GetSettlement(settlements, num).name << ")";
    }
    std::cout << std::endl;
}

int main() {
    setlocale(LC_ALL, "");
    std::vector<Settlement> settlements;

    std::string roadsFile, settlementsFile;
    std::cout << "Введите файл с информацией о дорогах: ";
    std::cin >> roadsFile;
    std::cout << "Введите файл с информацией о населенных пунктах: ";
    std::cin >> settlementsFile;

    std::ifstream roadsInput(roadsFile);
    std::ifstream settlementsInput(settlementsFile);

    if (!roadsInput.is_open()) {
        Error("Не удалось открыть файл с дорогами: " + roadsFile);
    }
    if (!settlementsInput.is_open()) {
        Error("Не удалось открыть файл с населенными пунктами: " + settlementsFile);
    }

    try {
        LoadSettlements(settlementsInput, settlements);
        LoadRoads(roadsInput, settlements);
        std::cout << "Данные успешно загружены!\n\n";
    }
    catch (const std::exception& e) {
        Error("Ошибка при загрузке данных: " + std::string(e.what()));
    }

    bool continueWorking = true;
    while (continueWorking) {
        std::cout << "Выберите действие:\n";
        std::cout << "1. Найти максимальный груз между двумя городами\n";
        std::cout << "2. Найти максимальный груз через промежуточный город\n";
        std::cout << "3. Показать список населенных пунктов\n";
        std::cout << "4. Выход\n";

        int choice = SafeInputChoice("Ваш выбор: ");

        switch (choice) {
        case 1: {
            PrintSettlements(settlements);
            int start = SafeInputInt("Введите номер начального города: ");
            int end = SafeInputInt("Введите номер конечного города: ");

            if (!IsFoundSettlement(settlements, start) || !IsFoundSettlement(settlements, end)) {
                std::cout << "Ошибка: Указан несуществующий город!\n";
                break;
            }

            PathResult result = FindMaxWeightPath(settlements, start, end);
            PrintPath(settlements, result);
            break;
        }
        case 2: {
            PrintSettlements(settlements);
            int start = SafeInputInt("Введите номер начального города: ");
            int intermediate = SafeInputInt("Введите номер промежуточного города: ");
            int end = SafeInputInt("Введите номер конечного города: ");

            if (!IsFoundSettlement(settlements, start) ||
                !IsFoundSettlement(settlements, intermediate) ||
                !IsFoundSettlement(settlements, end)) {
                std::cout << "Ошибка: Указан несуществующий город!\n";
                break;
            }

            PathResult result = FindMaxWeightPathViaIntermediate(settlements, start, intermediate, end);
            PrintPath(settlements, result);
            break;
        }
        case 3:
            PrintSettlements(settlements);
            break;
        case 4:
            continueWorking = false;
            break;
        }

        std::cout << std::endl;
    }

    return 0;
}