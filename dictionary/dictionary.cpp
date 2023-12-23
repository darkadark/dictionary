// dictionary.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_set>

#include <locale>

std::vector<std::string> split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}


void parse(const std::string& filename, const std::string& delimiter, std::vector<std::vector<std::string>>& words) {
    std::string s;
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    if (file.peek() == EOF) {
        throw std::runtime_error("File is empty: " + filename);
    }

    
    std::vector<std::string> str;
    while (getline(file, s)) {
        str = split(s, delimiter);
        words.push_back(str);
    }

    file.close();
}

bool miss_char(const std::string& word_longer, const std::string& word_shorter) {
    int diff_chars = 0;
    int l_idx = 0;
    int s_idx = 0;
    for (; l_idx < word_longer.size() && s_idx < word_shorter.size(); ) {
        if (word_longer[l_idx] != word_shorter[s_idx]) {
            ++diff_chars;
            if (diff_chars > 1) {
                break;
            }
            ++l_idx;
        }
        else {
            ++l_idx;
            ++s_idx;
        }

    }

    return diff_chars <= 1;
}

bool change_char(const std::string& word1, const std::string& word2) {
    int diff_chars = 0;
    for (int idx = 0; idx < word1.size();  ++idx ) {
        if (word1[idx] != word2[idx]) {
            ++diff_chars;
            if (diff_chars > 1) {
                break;
            }

        }
    }

    return diff_chars <= 1;
}

bool is_sim(const std::string& word, const std::string& word_dict) {
    switch(word.size() - word_dict.size()) {
    case -1:
        return miss_char(word_dict, word);
    case 0:
        return change_char(word_dict, word);
    case 1:
        return miss_char(word, word_dict);
    }

    return false;
}

std::string correct(const std::string& word, std::unordered_set<std::string>& dictionary) {
    if (dictionary.find(word) != dictionary.end()) {
        return word;
    }

    std::vector<std::string> sim_word;
    for (auto it = dictionary.begin(); it != dictionary.end(); ++it) {
        if (is_sim(word, *it)) {
            sim_word.push_back(*it);
        }
    }

    std::cout << "Unknown word: " << word << std::endl;
    std::cout << "1) Skip" << std::endl;
    std::cout << "2) Add to dictionary" << std::endl;
    for (int i = 0; i < sim_word.size(); ++i) {
        std::cout << i+3 << ") Replace: " << sim_word[i] << std::endl;
    }
    int var = 0;
    while (true) {
        std::cin >> var;
        if (var < 1 || var > 2 + sim_word.size()) {
            std::cout << "Error number" << std::endl;
            continue;
        }
        switch (var) {
        case 1:
            return word;
        case 2:
            dictionary.insert(word);
            return word;
        default:
            return sim_word[var - 3];
        }

    }
}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "russian_russia.1251");
    const std::string delimiter = " ";

    std::vector<std::vector<std::string>> words;

    std::string filename = argc > 1 ? argv[1] : "dict.txt";
    parse(filename, delimiter, words);

    std::unordered_set<std::string> dictionary;
    for (int i = 0; i < words.size(); ++i) {
        const std::vector<std::string>& vi = words[i];
        for (int j = 0; j < vi.size(); ++j) {
            dictionary.insert(vi[j]);
        }
    }

    words.clear();

    filename = argc > 2 ? argv[2] : "input.txt";
    parse(filename, delimiter, words);

    filename = argc > 3 ? argv[3] : "output.txt";
    std::ofstream file(filename);

    for (int i = 0; i < words.size(); ++i) {
        const std::vector<std::string>& vi = words[i];
        for (int j = 0; j < vi.size(); ++j) {
            if (j > 0)
                file  << delimiter;

            file << correct(vi[j], dictionary);
        }
        file << std::endl;
    }
    
    file.close();
    std::ofstream file1("new_dict.txt");
    for (auto it = dictionary.begin(); it != dictionary.end(); ++it) {
        file1 << *it << std::endl;
    }
    file1.close();

    return 0;
}

