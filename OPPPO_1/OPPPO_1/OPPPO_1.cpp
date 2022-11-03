// OPPPO_1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
// Сделал: Желамский Егор Алексеевич
// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

/*
nvar = 7;
ntask = 7;
ncout = 1; (Однонаправленный кольцевой список)

Обобщённый атефакт : Шифрованные тексты;

Базовые альтернативы(уникальные параметры, задающие отличительные признаки альтернатив) :
    Шифрование заменой символов(исходный и замещающий алфавиты - строки),
    Шифрование циклическим сдвигом кода каждого символа на n(целое число);

Параметры, общие для всех альтернатив : Текст - строка символов(любых), Имя владельца текста.
*/

#include <iostream>
//#include <variant>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <regex>
#include <fstream>
#include <ctype.h>

using namespace std;

enum Type_cipher {
    subst_cipher_type,
    rotat_cipher_type,
    word_cipher_type
};

struct Major
{
    string text;
    string author;
};

struct Subst_cipher
{
    Major inform;
    //const string orig_alph = "abcdefghijklmnopqrstuvwxyz"; //ПРОСТО создал константу глобальную
    string repl_alph;
};

struct Rotat_cipher
{
    Major inform;
    int num;
};

struct Word_cipher
{
    Major inform;
    string key_word;
};

struct Node {
    Node* next;
    Type_cipher type;
    Subst_cipher* subst;
    Rotat_cipher* rotat;
    Word_cipher* word;
};

struct List {
    Node* first = nullptr;
    Node* last = nullptr;
};

const string orig_alph = "abcdefghijklmnopqrstuvwxyz0123456789"; // порядок для работы алгоритма не важен, поэтому если надо можно и добавить как вписываемый параметр

const regex reg_add_subst("^ *ADD (subst)_cipher\\( ?\"([A-Za-z0-9 [:punct:]]{1,500})\" ?, ?\"([A-Za-z ]{1,255})\" ?, ?\"([a-z]{26}[0-9]{10})\" ?\\) *;$");
const regex reg_add_rotat("^ *ADD (rotat)_cipher\\( ?\"([A-Za-z0-9 [:punct:]]{1,500})\" ?, ?\"([A-Za-z ]{1,255})\" ?, ?([1-9]|[1-9][1-9]|[1-9]0|100) ?\\) *;$");
const regex reg_add_word("^ *ADD (word)_cipher\\( ?\"([A-Za-z0-9 [:punct:]]{1,500})\" ?, ?\"([A-Za-z ]{1,255})\" ?, ?\"([a-z0-9]{5})\" ?\\) *;$");
const regex reg_print("^ *PRINT\\(\\) *;$");
const regex reg_sort("^ *SORT\\( ?(asc|desc) ?\\) *;$");
const regex reg_cipher("^ *CIPHER\\( ?\"([A-Za-z ]{1,255})\" ?\\) *;$");
const regex reg_remove_all("^ *REMOVE\\(\\) *;$");
const regex reg_remove("^ *REMOVE\\(( ?(author|num|repl_alph) ?(=|>|<|>=|>=){1,2} ?\"? ?[A-Za-z0-9 ]{1,255}\"? ?)?\\) *;$");
const regex reg_remove_author("^ *REMOVE\\( ?author ?= ?\"([A-Za-z ]{1,255})\" ?\\) *;$");
const regex reg_remove_repl_alph("^ *REMOVE\\( ?repl_alph ?= ?\"([a-z]{26}[0-9]{10})\" ?\\) *;$");
const regex reg_remove_num("^ *REMOVE\\( ?num ?(=|>|<|>=|<=) ?([1-9]|[1-9][1-9]|[1-9]0|100) ?\\) *;$");
const regex reg_remove_key("^ *REMOVE\\( ?key ?= ?\"([a-z0-9]{5})\" ?\\) *;$");

void add_subst(Node* node, List& list, string text, string author, string repl_alph) {
    node->type = subst_cipher_type;
    Subst_cipher* subst_chiper = new Subst_cipher{ text,author,repl_alph };
    node->subst = subst_chiper;
}

void add_rotat(Node* node, List& list, string text, string author, string number) {
    node->type = rotat_cipher_type;
    Rotat_cipher* rotat_chiper = new Rotat_cipher{ text,author,stoi(number.c_str()) };
    node->rotat = rotat_chiper;
}

void add_word(Node* node, List& list, string text, string author, string key_word) {
    node->type = word_cipher_type;
    Word_cipher* word_chiper = new Word_cipher{ text,author,key_word };
    node->word = word_chiper;
}

void add_node(string typenode, List& list, string text, string author, string cipher) {
    Node* new_node = new Node;
    if (typenode == "subst")
    {
        add_subst(new_node, list, text, author, cipher);
    }
    if (typenode == "rotat")
    {
        add_rotat(new_node, list, text, author, cipher);
    }
    if (typenode == "word")
    {
        add_word(new_node, list, text, author, cipher);
    }
    if (list.first == nullptr) {
        list.first = new_node;
    }
    else {
        list.last->next = new_node;
    }
    new_node->next = list.first;
    list.last = new_node;
}

bool check_author(string author, Node* remove_node) {
    bool del_bool = false;
    if (remove_node->type == subst_cipher_type) {
        if (remove_node->subst->inform.author == author) {
            del_bool = true;
        }
        else {
            del_bool = false;
        }
    }
    if (remove_node->type == rotat_cipher_type) {
        if (remove_node->rotat->inform.author == author) {
            del_bool = true;
        }
        else {
            del_bool = false;
        }
    }
    if (remove_node->type == word_cipher_type) {
        if (remove_node->word->inform.author == author) {
            del_bool = true;
        }
        else {
            del_bool = false;
        }
    }
    return del_bool;
}

bool check_repl_alph(string repl_alph, Node* remove_node) {
    bool del_bool = false;
    if (remove_node->type == subst_cipher_type) {
        if (remove_node->subst->repl_alph == repl_alph) {
            del_bool = true;
        }
        else {
            del_bool = false;
        }
    }
    else {
        del_bool = false;
    }
    return del_bool;
}

bool check_num_not_equally(string num, Node* remove_node) {
    bool del_bool = false;
    if (remove_node->type == rotat_cipher_type) {
        if (remove_node->rotat->num != stoi(num)) {
            del_bool = true;
        }
        else {
            del_bool = false;
        }
    }
    else {
        del_bool = false;
    }
    return del_bool;
}

bool check_num_equally(string num, Node* remove_node) {
    bool del_bool = false;
    if (remove_node->type == rotat_cipher_type) {
        if (remove_node->rotat->num == stoi(num)) {
            del_bool = true;
        }
        else {
            del_bool = false;
        }
    }
    else {
        del_bool = false;
    }
    return del_bool;
}

bool check_num_more(string num, Node* remove_node) {
    bool del_bool = false;
    if (remove_node->type == rotat_cipher_type) {
        if (remove_node->rotat->num > stoi(num)) {
            del_bool = true;
        }
        else {
            del_bool = false;
        }
    }
    else {
        del_bool = false;
    }
    return del_bool;
}

bool check_num_less(string num, Node* remove_node) {
    bool del_bool = false;
    if (remove_node->type == rotat_cipher_type) {
        if (remove_node->rotat->num < stoi(num)) {
            del_bool = true;
        }
        else {
            del_bool = false;
        }
    }
    else {
        del_bool = false;
    }
    return del_bool;
}

bool check_num_equally_more(string num, Node* remove_node) {
    bool del_bool = false;
    if (remove_node->type == rotat_cipher_type) {
        if (remove_node->rotat->num >= stoi(num)) {
            del_bool = true;
        }
        else {
            del_bool = false;
        }
    }
    else {
        del_bool = false;
    }
    return del_bool;
}

bool check_num_equally_less(string num, Node* remove_node) {
    bool del_bool = false;
    if (remove_node->type == rotat_cipher_type) {
        if (remove_node->rotat->num <= stoi(num)) {
            del_bool = true;
        }
        else {
            del_bool = false;
        }
    }
    else {
        del_bool = false;
    }
    return del_bool;
}

bool check_key_word(string key, Node* remove_node) {
    bool del_bool = false;
    if (remove_node->type == word_cipher_type) {
        if (remove_node->word->key_word == key) {
            del_bool = true;
        }
        else {
            del_bool = false;
        }
    }
    else {
        del_bool = false;
    }
    return del_bool;
}

/*
void delete_all_node(List &list) {
    list.first = nullptr;
    list.last = nullptr;
}
*/

void delete_node(string searchstr, List& list, int check) {

    Node* rem_node = list.first;
    Node* lrem_node = list.first;
    Node* buff;
    bool del = false, end = false;
    if (rem_node != nullptr) {
        while (!end) {
            switch (check)
            {
            case 0:
                del = true;
                break;
            case 1:
                del = check_author(searchstr, rem_node);
                break;
            case 2:
                del = check_repl_alph(searchstr, rem_node);
                break;
            case 3:
                del = check_num_equally(searchstr, rem_node);
                break;
            case 4:
                del = check_num_more(searchstr, rem_node);
                break;
            case 5:
                del = check_num_less(searchstr, rem_node);
                break;
            case 6:
                del = check_num_equally_more(searchstr, rem_node);
                break;
            case 7:
                del = check_num_equally_less(searchstr, rem_node);
                break;
            case 8:
                del = check_num_not_equally(searchstr, rem_node);
                break;
            case 9:
                del = check_key_word(searchstr, rem_node);
                break;
            default:
                del = false;
                break;
            }

            if (del) {
                if (list.first == list.last) {
                    end = true;
                    if (rem_node != nullptr) {
                        delete rem_node;
                        list.first = nullptr;
                        list.last = nullptr;
                    }
                }
                else {
                    if (rem_node == list.last) {
                        end = true;
                        list.last = lrem_node;
                        lrem_node->next = list.first;
                        delete rem_node;

                    }
                    else {
                        if (rem_node == list.first) {
                            list.first = rem_node->next;
                            list.last->next = list.first;
                            buff = rem_node;
                            rem_node = rem_node->next;
                            lrem_node = rem_node;
                            delete buff;
                        }
                        else {
                            lrem_node->next = rem_node->next;
                            buff = rem_node;
                            rem_node = rem_node->next;
                            delete buff;
                        }
                    }
                }
            }
            else {
                if (rem_node == list.last) {
                    end = true;
                }
                else {
                    if (rem_node == list.first) {
                        rem_node = rem_node->next;
                    }
                    else {
                        lrem_node = rem_node;
                        rem_node = rem_node->next;
                    }
                }
            }
        }
    }else {
        cout << "Nothing to delete! List is empty!" << endl << endl;
    }
}

void print(List& list) {
    Node* node = list.first;
    bool end = false;
    cout << "\nPRINT:\n";
    if (node != nullptr) {
        while (!end)
        {
            if (node == list.last) {
                end = true;
            }
            if (node->type == subst_cipher_type) {
                cout << "Text: " << node->subst->inform.text << endl;
                cout << "Author: " << node->subst->inform.author << endl;
                //cout << "Orig_alph: " << node->subst.orig_alph << endl;
                cout << "Repl_alph: " << node->subst->repl_alph << endl << endl;
            }
            if (node->type == rotat_cipher_type) {
                cout << "Text: " << node->rotat->inform.text << endl;
                cout << "Author: " << node->rotat->inform.author << endl;
                cout << "Num: " << node->rotat->num << endl << endl;
            }
            if (node->type == word_cipher_type) {
                cout << "Text: " << node->word->inform.text << endl;
                cout << "Author: " << node->word->inform.author << endl;
                cout << "Key: " << node->word->key_word << endl << endl;
            }
            node = node->next;
        }
    }else {
        cout << "List is empty!" << endl << endl;
    }
}

char shift_symbol(int lnum_1, int rnum, int size_num, int num_rotat, char symbol) {
    int sym = (int)symbol + (num_rotat % size_num);
    if (sym > rnum) {
        sym = lnum_1 + (sym % rnum);
    }
    return (char)sym;
}

void cipher(string author, List& list) {
    Node* node = list.first;
    bool end = false, find = false;
    if (list.first != nullptr) {
        while (!end)
        {
            if (node == list.last) {
                end = true;
            }
            if (check_author(author, node)) {
                find = true;
                break;
            }
            node = node->next;
        }
        string text;
        if (find) {
            if (node->type == subst_cipher_type) {
                text = node->subst->inform.text;
                //string replace_alph = node->subst.repl_alph;
                cout << "\nText(origin): " << text << endl;
                for (int i = 0; i < text.length(); i++) {
                    if (isupper(text[i])) {
                        text[i] = toupper(node->subst->repl_alph[orig_alph.find(tolower(text[i]))]);
                    }
                    if (islower(text[i])) {
                        text[i] = node->subst->repl_alph[orig_alph.find(text[i])];
                    }
                    if (isdigit(text[i])) {
                        text[i] = node->subst->repl_alph[orig_alph.find(text[i])];
                    }
                    /*
                    // НЕ ДОДЕЛАНА
                    // без использования оригинального алфавита!
                    if (isupper(text[i])) {
                        sym = 65 - int(text[i]);
                        text[i] = toupper(node->data.subst.repl_alph[sym]);
                    }
                    if (islower(text[i])) {
                        sym = 97 - int(text[i]);
                        text[i] = node->data.subst.repl_alph[sym];
                    }

                    if (isdigit(symbol)) {
                        ????
                    }
                    */
                }
            }
            if (node->type == rotat_cipher_type) {
                text = node->rotat->inform.text;
                int num = node->rotat->num;
                cout << "\nText(origin): " << text << endl;
                for (int i = 0; i < text.length(); i++) {
                    if (isalpha(text[i]) || isdigit(text[i])) {

                        if (isupper(text[i])) { // (int)symbol >= 65 && (int)symbol <= 90
                            text[i] = shift_symbol(64, 90, 26, num, text[i]);
                        }
                        if (islower(text[i])) { // (int)symbol >= 97 && (int)symbol <= 122
                            text[i] = shift_symbol(96, 122, 26, num, text[i]);
                        }
                        if (isdigit(text[i])) { // (int)symbol >= 48 && (int)symbol <= 57
                            text[i] = shift_symbol(47, 57, 10, num, text[i]);
                        }
                    }
                }
            }
            if (node->type == word_cipher_type) {
                text = node->word->inform.text;
                string key_word = node->word->key_word;
                cout << "\nText(origin): " << text << endl;
                for (int i = 0, j = 0; i < text.length(); i++, j++) {
                    if (j == 6) {
                        j = 0;
                    }
                    if (isalpha(text[i]) || isdigit(text[i])) {
                        if (isupper(text[i])) {
                            text[i] = shift_symbol(64, 90, 26, (int)key_word[j], text[i]);
                        }
                        if (islower(text[i])) {
                            text[i] = shift_symbol(96, 122, 26, (int)key_word[j], text[i]);
                        }
                        if (isdigit(text[i])) {
                            text[i] = shift_symbol(47, 57, 10, (int)key_word[j], text[i]);
                        }
                    }
                }
            }
            cout << "New Text: " << text << endl << endl;
        }
        else {
            cout << "\nDon't found node!";
        }
    }else {
        cout << "List is empty!" << endl << endl;
    }
}

string get_author(Node* node) {
    string author;
    if (node->type == subst_cipher_type) {
        author = node->subst->inform.author;
    }
    if (node->type == rotat_cipher_type) {
        author = node->rotat->inform.author;
    }
    if (node->type == word_cipher_type) {
        author = node->word->inform.author;
    }
    return author;
}

bool gosort(List& list, Node* &lnode, Node* &node, Node* &rnode) {
    bool end = false;
    if (rnode == list.last) {
        end = true;
        list.last = node;
        node->next = list.first;
        rnode->next = node;
        lnode->next = rnode;
    }
    else {
        if (node == list.first) {
            list.first = rnode;
            list.last->next = list.first;
        }
        else {
            lnode->next = rnode;
        }
        node->next = rnode->next;
        rnode->next = node;

        lnode = rnode;
        rnode = node->next;
    }
    return end;
}

bool nosort(List& list, Node* &lnode, Node* &node, Node* &rnode) {
    bool end = false;
    if (rnode == list.last) {
        end = true;
    }
    else {
        lnode = node;
        node = rnode;
        rnode = rnode->next;
    }
    return end;
}

void sort(List& list, string sort_by) {
    Node* node, *lnode, *rnode;
    string author, r_author;
    bool end = false, fend = false;
    int sort_result = 0;
    if (list.first != nullptr) {
        while (!fend)
        {
            end = false;
            node = list.first;
            lnode = list.first;
            rnode = node->next;
            while (!end)
            {

                author = get_author(node);

                r_author = get_author(rnode);

                sort_result = strcmp(author.c_str(), r_author.c_str());
                switch (sort_result)
                {
                case 0:
                    end = nosort(list, lnode, node, rnode);
                    break;
                case 1:
                    if (sort_by == "asc") {
                        end = gosort(list, lnode, node, rnode);
                        fend = true;
                    }
                    else {
                        end = nosort(list, lnode, node, rnode);
                    }
                    break;
                case -1:
                    if (sort_by == "desc") {
                        end = gosort(list, lnode, node, rnode);
                        fend = true;
                    }
                    else {
                        end = nosort(list, lnode, node, rnode);
                    }
                    break;
                default:
                    break;
                }
            }
            if (fend == true) {
                fend = false;
            }
            else {
                fend = true;
            }
        }
    }else {
        cout << "Nothing to sort!" << endl << endl;
    }
}

int main()
{
    string strfile;
    cmatch result;
    List list;

    ifstream file("C:\\Example.txt");
    if (!file) {
        cout << "File dont open!" << endl;
        return -1;
    }
    while (getline(file, strfile)) {
        cout << strfile.c_str() << endl;
        if (regex_match(strfile.c_str(), result, reg_add_subst)) {
            string type_node = result[1];
            string text = result[2];
            string author = result[3];
            string repl_alph = result[4];
            add_node(type_node, list, text, author, repl_alph);
            continue;
        }
        if (regex_match(strfile.c_str(), result, reg_add_rotat))
        {
            string type_node = result[1];
            string text = result[2];
            string author = result[3];
            string num = result[4];
            add_node(type_node, list, text, author, num);
            continue;
        }
        if (regex_match(strfile.c_str(), result, reg_add_word))
        {
            string type_node = result[1];
            string text = result[2];
            string author = result[3];
            string key = result[4];
            add_node(type_node, list, text, author, key);
            continue;
        }
        if (regex_match(strfile.c_str(), result, reg_remove))
        {
			if (regex_match(strfile.c_str(), reg_remove_all))
			{
				//delete_all_node(list);
				delete_node("", list, 0);
				continue;
			}
            if (regex_match(strfile.c_str(), result, reg_remove_author))
            {
                string author = result[1];

                delete_node(author, list, 1);
                continue;

            }
            if (regex_match(strfile.c_str(), result, reg_remove_repl_alph))
            {
                string replace_alph = result[1];
                delete_node(replace_alph, list, 2);
                continue;
            }
            if (regex_match(strfile.c_str(), result, reg_remove_num))
            {
                string numif = result[1];
                string num = result[2];
                if (numif == "=") {
                    delete_node(num, list, 3);
                    continue;
                }
                if (numif == ">") {
                    delete_node(num, list, 4);
                    continue;
                }
                if (numif == "<") {
                    delete_node(num, list, 5);
                    continue;
                }
                if (numif == ">=") {
                    delete_node(num, list, 6);
                    continue;
                }
                if (numif == "<=") {
                    delete_node(num, list, 7);
                    continue;
                }
                if (numif == "!=") {
                    delete_node(num, list, 8);
                    continue;
                }
            }
            if (regex_match(strfile.c_str(), result, reg_remove_key))
            {
                string key = result[1];
                delete_node(key, list, 9);
                continue;

            }
        }
        if (regex_match(strfile.c_str(), result, reg_cipher))
        {
            string author = result[1];
            cipher(author, list);
            continue;

        }
        if (regex_match(strfile.c_str(), result, reg_sort))
        {
            string sort_by = result[1];
            sort(list, sort_by);
            continue;
        }
        if (regex_match(strfile.c_str(), reg_print))
        {
            print(list);
            continue;
        }
    }

    file.close();
    return 0;
}
