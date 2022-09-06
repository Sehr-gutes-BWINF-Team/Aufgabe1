#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <locale>
#include <codecvt>
#include <io.h>
#include <fcntl.h>
#include <vector>
#include <regex>

using namespace std;

#define RESOURCES      "C:\\workspace\\private\\bwinf-2022\\Aufgabe1\\resources\\"
#define ENCODING_ASCII      0
#define ENCODING_UTF8       1
#define ENCODING_UTF16LE    2
#define ENCODING_UTF16BE    3

wstring removeExcessiveSpaces(wstring &in);

wstring removeLineBreaks(wstring &in);

// https://gist.github.com/VeryCrazyDog/c20b2cb83896e9975d22
std::string readFile(std::string path) {
    std::string result;
    std::ifstream ifs(path.c_str(), std::ios::binary);
    std::stringstream ss;
    int encoding = ENCODING_ASCII;

    if (!ifs.is_open()) {
        // Unable to read file
        result.clear();
        return result;
    } else if (ifs.eof()) {
        result.clear();
    } else {
        int ch1 = ifs.get();
        int ch2 = ifs.get();
        if (ch1 == 0xff && ch2 == 0xfe) {
            // The file contains UTF-16LE BOM
            encoding = ENCODING_UTF16LE;
        } else if (ch1 == 0xfe && ch2 == 0xff) {
            // The file contains UTF-16BE BOM
            encoding = ENCODING_UTF16BE;
        } else {
            int ch3 = ifs.get();
            if (ch1 == 0xef && ch2 == 0xbb && ch3 == 0xbf) {
                // The file contains UTF-8 BOM
                encoding = ENCODING_UTF8;
            } else {
                // The file does not have BOM
                encoding = ENCODING_ASCII;
                ifs.seekg(0);
            }
        }
    }
    ss << ifs.rdbuf() << '\0';
    if (encoding == ENCODING_UTF16LE) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> utfconv;
        result = utfconv.to_bytes(std::wstring((wchar_t *) ss.str().c_str()));
    } else if (encoding == ENCODING_UTF16BE) {
        std::string src = ss.str();
        std::string dst = src;
        // Using Windows API
        _swab(&src[0u], &dst[0u], src.size() + 1);
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> utfconv;
        result = utfconv.to_bytes(std::wstring((wchar_t *) dst.c_str()));
    } else {
        result = ss.str();
    }
    return result;
}

std::wstring get_content(string name) {

    std::string path = RESOURCES + name;
    std::string utf8Content = readFile(path);
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> utfconv;
    std::wstring utf16LeContent = utfconv.from_bytes(utf8Content);
    return utf16LeContent;
}

wstring clean(wstring &in) {
    wstring clean;
    wregex whitespaces(L"\\s{2,}|\\n");
    clean = regex_replace(in, whitespaces, L" ", std::regex_constants::match_any);
    wregex anyotherchars(L"[[:punct:]]");
    clean = regex_replace(clean, anyotherchars, L"", std::regex_constants::match_any);
    return clean;
}

template<typename Out>
void split(const std::wstring &s, wchar_t delim, Out result) {
    std::wistringstream iss(s);
    std::wstring item;
    while (getline(iss, item, delim)) {
        *result++ = item;
    }
}

std::vector<std::wstring> split(const std::wstring &s, char delim) {
    std::vector<std::wstring> elems;
    split(s, delim, std::back_inserter(elems));


    return elems;
}

wstring generateRegex(wstring &in) {
    in = in.substr(0, in.length() - 1); // Weil end of file char

    const wstring wordprefix = L"(";
    const wstring wordsuffix = L")\\W+";
    const wstring wildcart = L"([^\\s]+)\\W+";

    vector<wstring> list = split(in, L' ');
    wstring regex = L"";
    for (const auto &item: list) {
        // wcout << item;
        if (item[0] == L'_') {
            regex.append(wildcart);
            continue;
        }
        regex.append(wordprefix);
        regex.append(item);
        regex.append(wordsuffix);
    }
    // wcout << regex << endl;
    return regex;
}

int main() {
    _setmode(_fileno(stdout), _O_U8TEXT);
    wstring book = get_content("Alice_im_Wunderland.txt");
    book = clean(book);
    // wcout << book;
    wstring stoerung = get_content("stoerung5.txt");
    wstring reg = generateRegex(stoerung);
    // wcout << reg;
    wregex word_regex(reg, std::regex_constants::icase);

    auto words_begin = std::wsregex_iterator(book.begin(), book.end(), word_regex, std::regex_constants::match_any);
    auto words_end = std::wsregex_iterator();

    for (std::wsregex_iterator i = words_begin; i != words_end; ++i) {
        std::wsmatch match = *i;
        std::wstring match_str = match.str();
        wcout << match_str << endl;
    }

    return 0;
}


