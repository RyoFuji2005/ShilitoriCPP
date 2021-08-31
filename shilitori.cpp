
// (c)2021 Ryo Fujinami.

#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <unistd.h>

#include "converters.hpp"

// 乱数初期化
std::mt19937 initialize_rand() {
    std::srand(std::time(NULL));
    std::mt19937 mt(std::rand());
    return mt;
}
std::mt19937 mt = initialize_rand();

// ランダム削除
std::vector<std::string> remove_random(std::vector<std::string> list) {
    std::uniform_int_distribution<> rand(0, list.size() - 1);
    list.erase(list.begin() + rand(mt));
    return list;
}

// 要素削除
std::vector<std::string> remove_element(std::vector<std::string> list, std::string text) {
    size_t length = list.size();
    for (int i = 0; i < length; i++) {
        if (list[i] == text) {
            list.erase(list.begin() + i);
        }
    }
    return list;
}

// 辞書を縮小
std::map<std::string, std::vector<std::string>> cut_word(std::map<std::string, std::vector<std::string>> dict) {
    std::vector<std::string> list = {};
    for (const auto& item : dict) {
        list.push_back(item.first);
    }
    std::uniform_int_distribution<> rand(0, list.size() - 1);
    for (int i = 0; i < 520; i++) {
        int num = rand(mt);
        if (dict[list[num]].size() != 0) {
            dict[list[num]] = remove_random(dict[list[num]]);
        }
    }
    return dict;
}
std::map<std::string, std::vector<std::string>> light_word_dict = cut_word(full_word_dict);

// 要素数をカウント
unsigned int count_words(std::map<std::string, std::vector<std::string>> dict) {
    unsigned int count = 0;
    for (const auto& item : dict) {
        count += item.second.size();
    }
    return count;
}

// 最小をカウント
unsigned int minor_words(std::map<std::string, std::vector<std::string>> dict) {
    unsigned int minor = 30;
    for (const auto& item : dict) {
        if (minor > item.second.size()) {
            minor = item.second.size();
        }
    }
    return minor;
}

// 要素存在確認
bool find_element(std::vector<std::string> list, std::string text) {
    for (const auto& item : list) {
        if (item == text) {
            return true;
        }
    }
    return false;
}

// 文字列スライス
std::vector<std::string> slice_text(std::string str) {
    int pos;
    unsigned char lead;
    int char_size;
    std::vector<std::string> list = {};
    for (pos = 0; pos < str.size(); pos += char_size) {
        lead = str[pos];
        if (lead < 0x80) {
            char_size = 1;
        }
        else if (lead < 0xE0) {
            char_size = 2;
        }
        else if (lead < 0xF0) {
            char_size = 3;
        }
        else {
            char_size = 4;
        }
        list.push_back(str.substr(pos, char_size));
    }
    return list;
}

// エラー判定
bool judge_error(std::string pword, std::string pwf, std::string cwe,
    std::vector<std::string> occur) {
    std::vector<std::string> sliced_p = slice_text(pword);
    if (pwf != cwe) {
        std::cout << "初めの文字を確認してください" << std::endl;
    }
    else if (sliced_p.size() <= 1) {
        std::cout << "１文字の単語は禁止です" << std::endl;
    }
    else if (sliced_p.size() > 10) {
        std::cout << "10文字より多い単語は禁止です" << std::endl;
    }
    else if (find_element(occur, pword)) {
        std::cout << "その言葉はもう出ています" << std::endl;
    }
    else {
        return false;
    }
    return true;
}

// 終了判定
bool judge_end(std::map<std::string, std::vector<std::string>> dict, std::string pword, std::string pwe, std::string cwe) {
    if (pwe == "ン") {
        std::cout << "「ン」が付きました" << std::endl << "あなたの負けです" << std::endl
            << "お疲れ様でした" << std::endl << std::endl;
    }
    else if (dict[pwe].size() == 0) {
        std::cout << "もう言葉を思いつきません" << std::endl << "私の負けです" << std::endl
            << "お疲れ様でした" << std::endl << std::endl;
    }
    else {
        return false;
    }
    return true;
}

// ランダム選択
std::string choice_random(std::vector<std::string> list) {
    std::uniform_int_distribution<> rand(0, list.size() - 1);
    return list[rand(mt)];
}

// ルール表示
void print_rule() {
    std::cout << "   ルール" << std::endl;
    std::cout << "＊ １文字だけの単語は禁止です" << std::endl;
    std::cout << "＊ 10文字以上の単語は禁止です" << std::endl;
    std::cout << "＊ 名詞を入力して\"Enter\"を押してください" << std::endl;
    std::cout << "＊ 最後に\"ン\"がついた時あなたの負けです" << std::endl;
    std::cout << "＊ 一度使われた単語は使えません" << std::endl;
    std::cout << "＊ 通常音と濁音・破裂音は区別しません" << std::endl;
    std::cout << "＊ カタカナ又はひらがなで入力してください" << std::endl;
}

// オプション表示
void print_option() {
    std::cout << "   オプション" << std::endl;
    std::cout << "-f すべての単語を利用します" << std::endl;
    std::cout << "-w 終了時に入力しません" << std::endl;
    std::cout << "-h ヘルプを表示します" << std::endl;
    std::cout << "-r ルールを表示しません" << std::endl;
    std::cout << "-c 単語数を表示しません" << std::endl;
}

// メイン
int main(int argc, char* argv[]) {
    bool opt_f = false;
    bool opt_w = false;
    bool opt_r = false;
    bool opt_c = false;
    int opt;
    while ((opt = getopt(argc, argv, "fwhrc")) != -1) {
        switch (opt) {
        case 'f':
            opt_f = true;
            break;
        case 'w':
            opt_w = true;
            break;
        case 'r':
            opt_r = true;
            break;
        case 'c':
            opt_c = true;
            break;
        case 'h':
            print_option();
            return 0;
        default:
            std::cout << "error: オプション \"" << opt << "\" は存在しません";
            return 1;
        }
    }

    std::map<std::string, std::vector<std::string>> word_dict;
    if (opt_f) {
        word_dict = full_word_dict;
    }
    else {
        word_dict = light_word_dict;
    }

    if (!opt_r) {
        print_rule();
    }
    if (!opt_c) {
        std::cout << "（ 合計単語数：" << count_words(word_dict) << "、最小単語数：" << minor_words(word_dict) << " ）" << std::endl;
    }
    if (!opt_r || !opt_c) {
        std::cout << std::endl;
    }

    std::string cwe = "リ";
    std::vector<std::string> occur = {};  // 出現リスト
    int num = 1;  // 現在の回数
    std::string cword = "シリトリ";
    std::string pwe = "始";
    occur.push_back(cword);
    std::cout << " " << num << " 回目" << std::endl;
    std::cout << pwe << "：" << cword << std::endl;
    std::string pword, pwf, copy, element;
    std::string long_character = "ー";
    std::vector<std::string> sliced_p, sliced_c;

    while (true) {
        std::cout << cwe << "：";
        std::cin >> pword;
        std::cout << std::endl;
        sliced_p = slice_text(pword);
        if (pword == "") {
            std::cout << "入力してください" << std::endl;
            continue;
        }
        copy = "";
        size_t length = sliced_p.size();
        bool error = false;
        for (int i = 0; i < length; i++) {
            element = sliced_p[i];
            if (!find_element(kana_list, element)) {
                error = true;
            }
            copy += kana_dict[element];
        }
        if (error) {
            std::cout << "カタカナ又はひらがなで入力してください" << std::endl;
            continue;
        }
        pword = copy;
        sliced_p = slice_text(pword);
        element = sliced_p.back();
        if (element == long_character) {
            element = sliced_p.rbegin()[1];
            pwe = long_dict[element];
            pwf = sliced_p[0];
        }
        else {
            pwe = sliced_p.back();
            pwf = sliced_p[0];
            pwe = voice_dict[pwe];
            pwf = voice_dict[pwf];
        }
        word_dict[cwe] = remove_element(word_dict[cwe], pword);
        if (judge_error(pword, pwf, cwe, occur)) {
            continue;
        }
        else if (judge_end(word_dict, pword, pwe, cwe)) {
            break;
        }
        occur.push_back(pword);
        std::string cword = choice_random(word_dict[pwe]);
        word_dict[pwe] = remove_element(word_dict[pwe], cword);
        occur.push_back(cword);
        sliced_c = slice_text(cword);
        element = sliced_c.back();
        if (element == long_character) {
            element = sliced_c.rbegin()[1];
            cwe = long_dict[element];
        }
        else {
            element = sliced_c.back();
            cwe = voice_dict[element];
        }
        num += 1;
        std::cout << "\n " << num << " 回目" << std::endl;
        std::cout << pwe << "：" << cword << std::endl;
    }

    if (!opt_w) {
        std::cin >> element;
    }

    return 0;
}
