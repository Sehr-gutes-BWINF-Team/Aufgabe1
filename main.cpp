#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <sstream>
#include <locale>
#include <codecvt>
#include <io.h>
#include <fcntl.h>
#include <vector>

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

wstring removeSpecialCharacters(wstring &in) {
    wstring formatted;
    for (const auto &item: in) {
        if (item == L'»') {
            continue;
        }
        if (item == L'«') {
            continue;
        }
        if (item == L'[') {
            continue;
        }
        if (item == L']') {
            continue;
        }
        if (item == L'(') {
            continue;
        }
        if (item == L')') {
            continue;
        }
        if (item == L',') {
            continue;
        }
        if (item == L'.') {
            continue;
        }
        if (item == L':') {
            continue;
        }
        if (item == L';') {
            continue;
        }
        if (item == L'?') {
            continue;
        }
        if (item == L'!') {
            continue;
        }
        if (item == L'-') {
            continue;
        }
        if (item == L'_') {
            continue;
        }
        if (item == L'\'') {
            continue;
        }
        formatted.push_back(item);
    }
    return formatted;
}

wstring format(wstring in) {
    wstring formatted = removeLineBreaks(in);
    formatted = removeExcessiveSpaces(formatted);
    formatted = removeSpecialCharacters(formatted);

    return formatted;

}


wstring removeLineBreaks(wstring &in) {
    wstring formatted;
    for (const auto &item: in) {
        if (item == '\n') {
            formatted.push_back(' ');
            continue;
        }
        if (item == '\r') continue;
        formatted.push_back(item);
    }
    return formatted;
}

wstring removeExcessiveSpaces(wstring &in) {
    wstring formatted;
    wchar_t c = 'x';
    for (const auto &item: in) {
        if (item == ' ' && c == ' ') {
            c = item;
            continue;
        }
        c = item;
        formatted.push_back(item);
    }
    return formatted;
}

wstring removeAllSpaces(const wstring &in) {
    wstring formatted;
    for (wchar_t i: in) {
        if (i == ' ') continue;
        formatted.push_back(i);
    }
    return formatted;
}

// https://stackoverflow.com/a/37454181
vector<wstring> split(const wstring &str, const wstring &delim) {
    vector<wstring> tokens;
    size_t prev = 0, pos;
    do {
        pos = str.find(delim, prev);
        wstring token = str.substr(prev, pos - prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}

bool equals(wstring &a, wstring &b) {
    a = removeAllSpaces(a);
    b = removeAllSpaces(b);
    if (a.length() != b.length()) return false;
    for (int i = 0; i < a.length(); ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

wstring to_lower(wstring s) {
    wstring out;
    for (const auto &item: s) {
        out.push_back(tolower(item));
    }
    return out;
}

bool valid(vector<wstring> target, vector<wstring> current) {
    int i = 0;
    for (int k = 0; k < target.size(); ++k) {
        wstring a = target[k];
        if (a[0] == L'_') continue;
        wstring b = current[k];
        if (!equals(a, b)) {
            if (i > 1) {
                wcout << "|" << a << "!=" << b << "|";
            }
            return false;
        }
        i++;


        if (i >= 2) {
            wcout << "\n";
            // wcout << a << " != " << b << " ";
            for (const auto &item: current) {
                wcout << item << " ";
            }
            wcout << "\n";
        }

    }
    return true;
}

int main() {
    _setmode(_fileno(stdout), _O_U8TEXT);
    wstring inputBook = get_content("Alice_im_Wunderland.txt");
    wstring book = format(inputBook);
    wstring inputStoerung = get_content("stoerung0.txt");
    vector<wstring> inputSplit = split(inputStoerung, L" ");
    vector<wstring> bookSplit = split(book, L" ");
    vector<wstring> cleanInputSplit;
    for (const auto &item: inputSplit) {
        cleanInputSplit.push_back(removeAllSpaces(item));
    }
    int size = inputSplit.size();

    vector<vector<wstring>> out;

    for (int i = 0; i < bookSplit.size() - size; ++i) {
        vector<wstring> list;
        for (int j = 0; j < size; ++j) {
            list.push_back(to_lower(bookSplit[i + j]));
        }

        if (valid(cleanInputSplit, list)) {
            out.push_back(list);
        }
    }
    wcout << "Input: ";
    for (const auto &item: cleanInputSplit) {
        wcout << item << " ";
    }
    wcout << "\n";
    wcout << "Results: ";

    for (const auto &item: out) {
        for (const auto &i: item) {
            wcout << i << " ";
        }
        wcout << "\n";
    }


    return 0;
}


