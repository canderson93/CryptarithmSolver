#include <iostream>
#include <sstream>
#include <map>

std::string read_input() {

    std::string input, output;

    //Reads input from IO
    std::cout << "Enter Cryptarithm" << std::endl;
    getline(std::cin, input);

    //Convert the string to uppercase and remove spaces
    int len = 0;
    for (char c : input) {

        if (isspace(c)) { continue; }
        else if ((c == '+' || c == '=')){
            if (len == 0) { continue; }
            len = 0;
        }
        else {
            ++len;
        }

        output += (char)toupper(c);
    }

    return output;
}

void print_map(const std::map<char, int>* tokens) {
    std::map<char, int> map = *tokens;

    auto final_iter = map.end();
    final_iter--;

    std::cout << "{";
    for (auto kv = map.begin(); kv != map.end(); kv++) {
        std::cout << '"' << kv->first << "\"->" << kv->second;
        if (kv != final_iter) {
            std::cout << ", ";
        }
    }

    std::cout << "}" << std::endl;

}

bool is_valid(const std::string cryptarithm, std::stringstream& reason) {
    bool valid = true;
    int len = 0;

    //TODO: Maybe check count before and after non-alpha tokens

    for (char c : cryptarithm) {

        if (!(isalpha(c))) {
            if (c != '+' && c != '=') {
                valid = false;
                reason << "Cryparithms can only contain letters" << std::endl;
            }
        } else {
            ++len;
        }
    }

    if (len == 0) {
        valid = false;
        reason << "Cryptarithms cannot be empty" << std::endl;
    }

    return valid;
}

std::map<char, int> create_map(const std::string cryptaritm) {
    std::map<char, int> out;

    for (char c : cryptaritm) {
        auto it = out.find(c);

        if (isalpha(c) && it == out.end()) {
            out.insert(std::pair<char,int>(c, 0));
        }
    }

    return out;
};

int parse_number(const std::string *cryptarithm, const std::map<char, int> *tokens, int *index) {
    int value = 0;
    int i = *index;

    while (i < cryptarithm->length()) {
        char c = cryptarithm->at(i);
        if (!isalpha(c)) {
            ++i; //move past the non-alpha char
            break;
        }

        value *= 10;
        int val = tokens->at(c);
        value += val;
        ++i;
    }

    *index = i;
    return value;
}

bool evaluate_cryptarithm(const std::string* cryptarithm, const std::map<char, int> *tokens, const int divisions) {

    //Check that the proposed solution is valid
    //i.e no leading zeroes, and no double ups
    int i = 0;
    bool numbers[10] = {0};
    bool is_first_letter = true;

    //Check for duplicate numbers
    for (auto kv = tokens->begin(); kv != tokens->end(); kv++) {
        if (numbers[kv->second]) {
            std::cout << "Rejected: Duplicate Numbers ";
            print_map(tokens);
            return false;
        }
        numbers[kv->second] = true;
    }

    //Check for leading zeroes
    while (i < cryptarithm->length()) {
        char c = cryptarithm->at(i);
        ++i;

        if (!isalpha(c)) {
            is_first_letter = true;
            continue;
        }

        int val = tokens->at(c);
        if (is_first_letter && val == 0) {
            std::cout << "Rejected: Leading Zero ";
            print_map(tokens);
            return false;
        }

        is_first_letter = false;
    }

    //Check that the proposed solution is correct
    int values [divisions];
    int running_total = 0;
    int index = 0;

    for (i = 0; i < divisions; i++) {
        values[i] = parse_number(cryptarithm, tokens, &index);

        if (i != divisions-1) {
            running_total += values[i];
        }
    }

    if (running_total != values[i-1]) {
        std::cout << "Rejected: Incorrect Solution ";
        print_map(tokens);
    }
    return running_total == values[i-1];
}

bool solve_cryptarithm(const std::string *cryptarithm, const int division, std::map<char, int> *tokens,
                       std::map<char, int>::iterator it) {

    if (it == tokens->end()) {
        return evaluate_cryptarithm(cryptarithm, tokens, division);
    }

    const int num_array[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    const int len = sizeof(num_array) / sizeof(num_array[0]);
    auto next = it;
    next++;

    for (int i = 0; i < len; i++) {

        it->second = num_array[i];
        bool solved = solve_cryptarithm(cryptarithm, division, tokens, next);

        if (solved) {
            return true;
        }
    }

    return false;
}

int main() {
    std::cout << "Welcome to the Cryptarithm Solver" << std::endl;

    std::string cryptarithm = read_input();
    std::stringstream err;

    if (!is_valid(cryptarithm, err)) {
        std::cerr << "Cryparithm is not valid:" << std::endl;
        std::cerr << err.str();
        return 1;
    }

    //calculate the number of divisions in the cryptarithm
    int divisions = 1;
    int len = 0;
    for (char c : cryptarithm) {
        if ((c == '+' || c == '=')) {
            if (len != 0) {
                ++divisions;
            }
        } else {
            ++len;
        }
    }

    std::map<char, int> token_map = create_map(cryptarithm);
    if (solve_cryptarithm(&cryptarithm, divisions, &token_map, token_map.begin())) {
        std::cout << "Answer:" << std::endl;
        print_map(&token_map);
    } else {
        std::cout << "An answer could not be found" << std::endl;
    }

    return 0;
}