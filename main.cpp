#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <variant>
#include <regex>
#include <cstdlib>
#include <ctime>
#include <vector>

class Genova {
public:
    void start() {
        std::cout << "Welcome to Genova - please complete the CAPTCHA to continue.\n";

        while (!captchaChallenge()) {
            std::cout << "Incorrect. Please try again.\n";
        }

        std::cout << "CAPTCHA passed! You may now use Genova.\n";
        std::cout << "Type 'help' for a list of commands.\n";

        std::string input;
        while (true) {
            std::cout << "\n> ";
            std::getline(std::cin, input);
            if (input == "exit") {
                std::cout << "Goodbye!\n";
                break;
            }
            input = substituteVariables(input);
            handleCommand(input);
        }
    }

private:
    using VarValue = std::variant<int, double, std::string>;
    std::map<std::string, VarValue> variables;

    bool captchaChallenge() {
        std::srand(std::time(nullptr));
        int type = std::rand() % 3;

        if (type == 0) {
            return mathCaptcha();
        } else if (type == 1) {
            return textCaptcha();
        } else {
            return sequenceCaptcha();
        }
    }

    bool mathCaptcha() {
        int a = std::rand() % 10 + 1;
        int b = std::rand() % 10 + 1;
        char ops[] = {'+', '-', '*'};
        char op = ops[std::rand() % 3];

        int correctAnswer = 0;
        switch (op) {
            case '+': correctAnswer = a + b; break;
            case '-': correctAnswer = a - b; break;
            case '*': correctAnswer = a * b; break;
        }

        std::cout << "Solve this: " << a << " " << op << " " << b << " = ?\n";

        std::string answerStr;
        std::getline(std::cin, answerStr);

        try {
            int userAnswer = std::stoi(answerStr);
            return userAnswer == correctAnswer;
        } catch (...) {
            return false;
        }
    }

    bool textCaptcha() {
        std::vector<std::string> words = {"genova", "assistant", "shell", "command", "captcha"};
        std::string word = words[std::rand() % words.size()];
        std::string reversed = std::string(word.rbegin(), word.rend());

        std::cout << "Type this word backwards: " << word << "\n";
        std::string answer;
        std::getline(std::cin, answer);

        return answer == reversed;
    }

    bool sequenceCaptcha() {
        struct Seq {
            std::vector<int> nums;
            int next;
        };

        std::vector<Seq> sequences = {
            {{1, 2, 3, 4}, 5},
            {{2, 4, 6, 8}, 10},
            {{5, 10, 15, 20}, 25},
            {{3, 6, 9, 12}, 15}
        };

        Seq chosen = sequences[std::rand() % sequences.size()];

        std::cout << "What is the next number in this sequence?\n";
        for (int n : chosen.nums) std::cout << n << " ";
        std::cout << "\n";

        std::string answerStr;
        std::getline(std::cin, answerStr);

        try {
            int userAnswer = std::stoi(answerStr);
            return userAnswer == chosen.next;
        } catch (...) {
            return false;
        }
    }

    std::string substituteVariables(const std::string& input) {
        std::string result = input;
        std::regex varRegex(R"(\$([a-zA-Z_][a-zA-Z0-9_]*))");

        std::smatch match;
        std::string::const_iterator searchStart(result.cbegin());
        while (std::regex_search(searchStart, result.cend(), match, varRegex)) {
            std::string varName = match[1].str();
            std::string valueStr = varToString(varName);
            result.replace(match.position(0), match.length(0), valueStr);
            searchStart = result.cbegin() + match.position(0) + valueStr.length();
        }

        return result;
    }

    std::string varToString(const std::string& varName) {
        if (variables.find(varName) == variables.end()) return "";

        VarValue& val = variables[varName];
        if (std::holds_alternative<int>(val))
            return std::to_string(std::get<int>(val));
        else if (std::holds_alternative<double>(val))
            return std::to_string(std::get<double>(val));
        else
            return std::get<std::string>(val);
    }

    void handleCommand(const std::string& input) {
        if (input.empty()) {
            return;
        }
        if (input == "help") {
            std::cout << "Available commands:\n";
            std::cout << "  help                  - Show this help\n";
            std::cout << "  convert               - Convert units (e.g. convert 10 km to mi)\n";
            std::cout << "  calc                  - Calculate simple math (e.g. calc 3 + 4)\n";
            std::cout << "  set <var> <value>     - Set a variable (auto type detection)\n";
            std::cout << "  get <var>             - Get variable value\n";
            std::cout << "  exit                  - Exit Genova\n";
        } else if (input.starts_with("convert ")) {
            convertUnits(input);
        } else if (input.starts_with("calc ")) {
            calculate(input);
        } else if (input.starts_with("set ")) {
            setVariable(input);
        } else if (input.starts_with("get ")) {
            getVariable(input);
        } else {
            std::cout << "Unknown command: '" << input << "'\n";
            std::cout << "Type 'help' for a list of available commands.\n";
        }
    }

    void convertUnits(const std::string& input) {
        std::stringstream ss(input);
        std::string cmd, fromUnit, toUnit, tmp;
        double value;
        ss >> cmd >> value >> fromUnit >> tmp >> toUnit;

        std::map<std::string, double> toMeters = {
            {"km", 1000.0}, {"m", 1.0}, {"cm", 0.01}, {"mm", 0.001},
            {"mi", 1609.34}, {"ft", 0.3048}, {"in", 0.0254}
        };

        if (toMeters.count(fromUnit) && toMeters.count(toUnit)) {
            double meters = value * toMeters[fromUnit];
            double result = meters / toMeters[toUnit];
            std::cout << value << " " << fromUnit << " = " << result << " " << toUnit << "\n";
        } else {
            std::cout << "Unsupported units.\n";
        }
    }

    void calculate(const std::string& input) {
        std::stringstream ss(input.substr(5));
        double a, b;
        char op;
        ss >> a >> op >> b;

        double result;
        switch (op) {
            case '+': result = a + b; break;
            case '-': result = a - b; break;
            case '*': result = a * b; break;
            case '/': result = b != 0 ? a / b : NAN; break;
            default:
                std::cout << "Unsupported operator.\n";
                return;
        }
        std::cout << "Result: " << result << "\n";
    }

    void setVariable(const std::string& input) {
        std::stringstream ss(input);
        std::string cmd, varName;
        ss >> cmd >> varName;

        std::string valueStr;
        std::getline(ss, valueStr);
        if (!valueStr.empty() && valueStr[0] == ' ') valueStr.erase(0,1);

        if (varName.empty() || valueStr.empty()) {
            std::cout << "Usage: set <variable> <value>\n";
            return;
        }

        try {
            size_t pos;
            int intVal = std::stoi(valueStr, &pos);
            if (pos == valueStr.size()) {
                variables[varName] = intVal;
                std::cout << "Variable '" << varName << "' set to int: " << intVal << "\n";
                return;
            }
        } catch (...) {}

        try {
            size_t pos;
            double doubleVal = std::stod(valueStr, &pos);
            if (pos == valueStr.size()) {
                variables[varName] = doubleVal;
                std::cout << "Variable '" << varName << "' set to double: " << doubleVal << "\n";
                return;
            }
        } catch (...) {}

        variables[varName] = valueStr;
        std::cout << "Variable '" << varName << "' set to string: " << valueStr << "\n";
    }

    void getVariable(const std::string& input) {
        std::stringstream ss(input);
        std::string cmd, varName;
        ss >> cmd >> varName;

        if (varName.empty()) {
            std::cout << "Usage: get <variable>\n";
            return;
        }
        if (variables.find(varName) != variables.end()) {
            VarValue& val = variables[varName];
            std::cout << varName << " = ";
            if (std::holds_alternative<int>(val))
                std::cout << "(int) " << std::get<int>(val);
            else if (std::holds_alternative<double>(val))
                std::cout << "(double) " << std::get<double>(val);
            else
                std::cout << "(string) " << std::get<std::string>(val);
            std::cout << "\n";
        } else {
            std::cout << "Variable '" << varName << "' not found.\n";
        }
    }
};

int main() {
    Genova genova;
    genova.start();
    return 0;
}
