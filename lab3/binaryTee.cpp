//Общими требованиями к лабораторной работе являются :
//1) вводить   исходное   дерево  из  файла  в  понятной  для
//пользователя форме, а не с клавиатуры;
//2) по требованию прользователя показывать дерево на экране;
//3) обрабатывать  дерево  в  оперативной памяти, а не путем
//многократного обращения к файлу;
//4) обеспечить   возможность   многократных   запросов   без
//повторного запуска программы.

//18. Задано бинарное  дерево.Построить  прошитое   дерево,
//соответствующее обходу сверху вниз.Составить программу удале -
//ния поддерева с  корнем  в  заданной  вершине  без  повторного
//построения дерева.Конечное дерево должно оставаться прошитым.
//Выдать информацию о нитях исходного и конечного деревьев(11).

#include <iostream>
#include <fstream>
#include <string>
#include <regex>

struct Tree
{
    std::string name;
    Tree* fath;
    int level = 0;
    Tree* left;
    Tree* right;
    bool rightThread;
};

void Error(std::string e) {
    std::cout << std::endl;
    std::cout << "Ошибка: " << e << std::endl;
    exit(1);
}

bool IsRootTree(std::string& root) {
    return !root.empty() && root[0] != '.';
}

int CounteLevel(std::string& branchTree) {
    int countDot = 0;
    while (countDot < branchTree.length() && branchTree[countDot] == '.') {
        countDot++;
    }
    branchTree = std::regex_replace(branchTree, std::regex("^\\.+"), "");
    branchTree = std::regex_replace(branchTree, std::regex("\\s+$"), "");
    return countDot;
}

void InitializationTree(Tree* tree, std::string& nameRootTree) {
    if (tree == nullptr) return;
    nameRootTree = std::regex_replace(nameRootTree, std::regex("^\\s+"), "");
    if (nameRootTree == "") Error("Корень не имеет названия");
    tree->name = nameRootTree;
    tree->fath = nullptr;
    tree->level = 0;
    tree->left = nullptr;
    tree->right = nullptr;
    tree->rightThread = false;
}

Tree* FillTree(Tree* tree, std::string& nameRootTree, int& nextLevel, bool& flagLeftSon) {
    if (tree == nullptr) return nullptr;
    if (nameRootTree.empty()) {
        flagLeftSon = true;
        return tree;
    }
    if (tree->left == nullptr && !flagLeftSon) {
        tree->left = new Tree;
        tree->left->name = nameRootTree;
        tree->left->fath = tree;
        tree->left->level = nextLevel;
        tree->left->left = nullptr;
        tree->left->right = nullptr;
        tree->left->rightThread = false;
    }
    else {
        Tree* current = tree;
        while (current->left != nullptr || current->right != nullptr) {
            while (current->left != nullptr) {
                current = current->left;
            }
            while (current->right != nullptr) {
                current = current->right;
            }
        }
        Tree* nextNode;
        if (nextLevel > current->level && !flagLeftSon) {
            current->left = new Tree;
            nextNode = current->left;
        }
        else {
            current->right = new Tree;
            nextNode = current->right;
        }


        nextNode->name = nameRootTree; 
        if (current->level == nextLevel) {
            nextNode->fath = current->fath;
            current->rightThread = true;
        }
        else if (current->level > nextLevel) {
            Tree* fathFind = current->fath;
            while (fathFind != nullptr && fathFind->level != nextLevel) {
                fathFind = fathFind->fath;
            }
            if (fathFind != nullptr && nextNode != nullptr) {
                nextNode->fath = fathFind->fath;
            }
            current->rightThread = true;
        }
        else {
            nextNode->fath = current;

        }
        nextNode->level = nextLevel;
        nextNode->left = nullptr;
        nextNode->right = nullptr;
        nextNode->rightThread = false;

        flagLeftSon = false;
        return nextNode;
    }
}

void BuildStitchedTree(Tree*& tree, std::ifstream& input, std::string& nameRootTree) {
    tree = new Tree;
    InitializationTree(tree, nameRootTree);
    std::string branchTree;
    bool flagLeftSon = false;
    while (std::getline(input, branchTree)) {
        if (!branchTree.empty()) {
            int nextLevel = CounteLevel(branchTree);
            if (nextLevel != 0) {
                FillTree(tree, branchTree, nextLevel, flagLeftSon);
            }
            else {
                Error("В дереве найден второй нулевой корень");
            }
        }
        else {
            break;
        }
    }
}

void PrintDown(Tree* node) {
    if (node == nullptr) return;

    for (int i = 0; i < node->level; i++) std::cout << '.';
    std::cout << node->name; //<< (node->left != nullptr) << (node->right != nullptr)

    if (node->right != nullptr && node->rightThread == true) {
        std::cout << " [Нить от " << node->name << " до " << node->right->name << "]";
    }

    std::cout << std::endl;

    if (node->left != nullptr) {
        PrintDown(node->left);
    }
    if (node->right != nullptr) {
        PrintDown(node->right);
    }
}

void PrintStitchedTree(Tree* stitchedTree) {
    if (stitchedTree == nullptr) std::cout << "Дерево пустое\n";
    PrintDown(stitchedTree);
}

void CleanTree(Tree*& tree) {
    if (tree == nullptr) return;
    if (tree->left != nullptr) CleanTree(tree->left);
    if (tree->right != nullptr) CleanTree(tree->right);
    delete tree;
    tree = nullptr;
}

void DeleteNode(Tree*& tree, Tree*& saveNode) {
    if (tree == nullptr) return;
    if (tree->left != nullptr && tree->left->level > saveNode->level) {
        DeleteNode(tree->left, saveNode);
    }
    else if (tree->right != nullptr && tree->right->level > saveNode->level) {
        DeleteNode(tree->right, saveNode);
    }
    else if (tree->right != nullptr && tree->right->level <= saveNode->level) {
        saveNode = tree->right;
    }
    else {
        Tree* emptyNode = nullptr;
        saveNode = emptyNode;
    }
    delete tree;
    tree = nullptr;
}

bool FindSubtree(Tree*& tree, std::string nameRootTree) {
    if (tree == nullptr) return false;

    if (tree->name == nameRootTree) {
        if (tree->fath != nullptr) {
            bool flagExtreme = false;
            Tree* savePrev = tree->fath;
            while ((savePrev->left != nullptr && savePrev->left->name != nameRootTree) ||
                (savePrev->right != nullptr && savePrev->right->name != nameRootTree)) {
                while (savePrev->left != nullptr && tree->name != savePrev->left->name) {
                    savePrev = savePrev->left;
                    flagExtreme = true;
                }
                while (savePrev->right != nullptr && tree->name != savePrev->right->name) {
                    savePrev = savePrev->right;
                    flagExtreme = true;
                }
            }
            Tree* saveNode = tree;
            DeleteNode(tree, saveNode);
            if (saveNode != nullptr) {
                savePrev->right = saveNode;
                if (savePrev->level >= saveNode->level) {
                    savePrev->rightThread = true;
                }
                else {
                    savePrev->rightThread = false;
                }
            }
            else {
                savePrev->rightThread = false;
            }
        }
        else {
            CleanTree(tree);
        }
        return true;
    }

    if (tree->left != nullptr) {
        if (FindSubtree(tree->left, nameRootTree)) {
            return true;
        }
    }
    if (tree->right != nullptr) {
        if (FindSubtree(tree->right, nameRootTree)) {
            return true;
        }
    }
    return false;
}

Tree* DeleteTree(Tree*& tree, std::string nameRootTree) {
    bool flagGoodWord = FindSubtree(tree, nameRootTree);

    if (!flagGoodWord) {
        std::cout << "Узел '" << nameRootTree << "' не найден." << std::endl;
    }
    else {
        std::cout << "Поддерево с корнем '" << nameRootTree << "' удалено." << std::endl;
    }

    return tree;
}

void OutInformationThead(Tree* root) {
    if (root == nullptr) {
        std::cout << "Деверо пустое или для удаления выбран корень дерева\n";
        return;
    }
    if (root->right != nullptr && root->rightThread == true) {
        std::cout << " [Нить от " << root->name << " до " << root->right->name << "]\n";
    }
    if (root->left != nullptr) {
        OutInformationThead(root->left);
    }
    if (root->right != nullptr) {
        OutInformationThead(root->right);
    }
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
        Tree* root = nullptr;

        while (flagWork && !in.eof()) {
            std::string rootTree;

            if (std::getline(in, rootTree) && IsRootTree(rootTree)) {
                BuildStitchedTree(root, in, rootTree);

                if (root != nullptr) {
                    bool processingTree = true;
                    while (processingTree) {
                        std::cout << "\nЧто вы хотите сделать?(ответ - цифра пункта): \n";
                        std::cout << "1. показать дерево \n";
                        std::cout << "2. удалить поддерево \n";
                        std::cout << "3. завершить \n";

                        int answer = 0;
                        std::cin >> answer;
                        std::cout << std::endl;
                        if (answer == 1) {
                            PrintStitchedTree(root);

                        }
                        else if (answer == 2) {
                            std::string nameRootTree;
                            std::cout << "Введите имя корня, поддерево которого Вы хотите удалить: ";
                            std::cin >> nameRootTree;
                            std::cout << "Нити у предыдущего:\n";
                            OutInformationThead(root);
                            root = DeleteTree(root, nameRootTree);
                            std::cout << "Нити у следующего:\n";
                            OutInformationThead(root);
                        }
                        else if (answer == 3) {
                            processingTree = false;
                            flagWork = false;
                        }
                        else {
                            Error("Неверный выбор!");;
                        }
                    }

                    CleanTree(root);
                    root = nullptr;
                }
            }
        }

        if (root != nullptr) {
            CleanTree(root);
        }

        std::cout << "Достигнут конец файла или завершено пользователем!" << std::endl;
    }
    else {
        Error("Ошибка открытия файла!");
    }
    in.close();

    return 0;
}