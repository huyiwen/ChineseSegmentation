#include <string>
#include <fstream>
#include <set>
#include <unordered_map>
#include <cmath>
#include "utils.hpp"
using std::wstring;


class Seg {

private:

std::unordered_map<wstring, float> _dict;
std::vector<std::pair<float, size_t> > _buffer;
static inline std::wregex _punctuation = std::wregex(\
        L"[，。；？！“”《》、]", std::wregex::extended);
float _log_N;
float _penalties[100];

float _penalty(const size_t length, const float count) {
    if (length > 99) {
        return -100;
    }
    // std::wcout << L"[" << length << L" " << count << L"]";
    return exp(-count) * this->_penalties[length];
}

void seg_str(const wstring &sent) {

    // initalize DP buffer
    size_t S = sent.length();
    this->_buffer.clear();
    this->_buffer.resize(S);

    // first token (case npos_sent = npos_old = 1)
    this->_buffer[0] = std::make_pair(this->_dict[sent.substr(0, 1)], 0);

    // enumerating the length of maximum sentence
    for (std::wstring::size_type npos_sent = 2; npos_sent <= sent.size(); npos_sent++) {

        // inital score: case npos_old = 0
        float count = this->_dict[sent.substr(0, npos_sent)];
        float mx_score = count + this->_penalty(npos_sent, count);
        std::wstring::size_type mx_pos = 0;
        // std::wcout << sent.substr(0, npos_sent) << L" " << mx_score << L"=" << count << L"+" << this->_penalty(npos_sent, count) << std::endl;

        // state transfer: enumerating new word
        for (std::wstring::size_type npos_old = 1; npos_old < npos_sent; npos_old++) {
            std::wstring new_word = sent.substr(npos_old, npos_sent - npos_old);
            count = this->_dict[new_word];
            float score = count + this->_penalty(new_word.length(), count) - this->_log_N + this->_buffer[npos_old-1].first;

            // std::wcout << score << L" " << this->_buffer[npos_old-1].first << L" " << sent.substr(0, npos_old) << L" " << this->_penalty(new_word.length(), count) - this->_log_N << L" " << count << L" " << new_word << std::endl;
            if (score > mx_score) { mx_score = score;
                mx_pos = npos_old;
            }
        }

        this->_buffer[npos_sent-1] = std::make_pair(mx_score, mx_pos);
        // std::wcout << L"   final: " << sent.substr(mx_pos, npos_sent-mx_pos) << L"    " << npos_sent << L" " << mx_score << L" " << mx_pos << L"\n" << std::endl;
        
    }

    for (size_t i = 0; i < S - 1; i++) {
        std::wcout << sent[i];
        if (this->_buffer[i].second != this->_buffer[i+1].second) {
            std::wcout << L"/";
        }
    }
    std::wcout << sent[S-1];

}


public:
    
Seg(std::string dict_name = "dict.txt") {
    
    std::ifstream ifs (dict_name);
    this->_load(ifs);

}

void segmentation(wstring input) {

    // segmentation with punctuations
    std::vector<wstring> sents = std::vector<wstring>(\
            std::wsregex_token_iterator(\
                input.begin(), input.end(), this->_punctuation, -1), \
            std::wsregex_token_iterator());

    // initalize buffer
    std::wstring::size_type mx_len = 0;
    for (const auto& sent: sents) {
        mx_len = std::max(mx_len, sent.size());
    }
    this->_buffer.reserve(mx_len);

    size_t s_num = sents.size();
    for (size_t i = 0; i < s_num; i++) {
        this->seg_str(sents[i]);
        std::wcout << (i != s_num-1 ? L'/' : L'\n');
        // std::wcout << L"sent: " << sent << L" " << this->_dict[sent.substr(0, 1)] << std::endl;
        
    }

}

private:

void _load(std::ifstream &ifs) {
    std::string word, pos;
    int count;
    std::wstring tmp_word;

    int tmp_counts[100];
    memset(tmp_counts, 0, sizeof(tmp_counts));

    while (ifs >> word >> count >> pos) {
        tmp_word = to_wstring(word);
        this->_dict[tmp_word] = log10((float)count);
        tmp_counts[tmp_word.length()]++;
    }
    this->_log_N = log10(this->_dict.size());
    // std::wcout << L" logN = " << this->_log_N << L"  N = " << this->_dict.size() << std::endl;

    for (int i = 1; i < 100; i++) {
        this->_penalties[i] = log2(1.0 * tmp_counts[i] / this->_dict.size());
    }
}

};


