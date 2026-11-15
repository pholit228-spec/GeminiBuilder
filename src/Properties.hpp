#pragma once
#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <iterator>
#include <string>
#include <string_view>

// Documented GD property keys. Restrict keys rather than accepting arbitrary
// indices into the game's object deserializer. Complex 2.2 structures, such as
// particle data, are deliberately not passed through without a typed parser.
namespace builder {
inline constexpr int kKeys[] = {
  4,5,6,7,8,9,10,11,12,13,14,15,16,17,19,20,21,22,23,24,25,28,29,30,31,32,33,34,35,
  41,42,43,44,45,46,47,48,49,50,51,52,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,
  70,71,72,73,75,76,77,78,79,80,81,82,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,
  100,101,102,103,104,105,106,107,108,110,111,112,113,114,115,116,117,120,121,128,129,
  131,132,133,135,138,139,141,142,143,144,146,147,148,150,151,153,154,159,160,161,162,
  163,164,165,169,171,174,175,176,177,179,180,181,182,183,184,188,189,190,191,192,
  194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,
  214,215,216,218,222,243,249,263,264,274,276,287,290,291,370,371,373,374,376,378,
  389,392,393,394,395,399,400,404,405,406,407,408,409,410,411,412,413,414,415,416,
  417,418,419,420,421,422,423,424,425,426,428,430,432,433,434,436,437,438,439,447,
  449,452,453,454,455,456,458,460,461,465,466,467,468,469,470,471,472,473,474,476,
  477,478,479,480,481,482,483,484,485,486,487,489,490,491,502,503,512,515,520,521,
  522,523,524,525,532,540,541,542,543,544,545,546,573,574,575,576,577,578,579,580
};
inline bool allowedKey(int key) {
    return std::find(std::begin(kKeys), std::end(kKeys), key) != std::end(kKeys);
}
inline std::string keyCatalog() {
    std::string out;
    for (int key : kKeys) { if (!out.empty()) out += ','; out += std::to_string(key); }
    return out;
}
inline bool numeric(int key, double x) {
    if (!allowedKey(key) || key == 31 || key == 43 || key == 44 || key == 49 || key == 57 || key == 274) return false;
    if (!std::isfinite(x) || std::abs(x) > 1000000) return false;
    if (key >= 7 && key <= 9 && (x < 0 || x > 255 || std::floor(x) != x)) return false;
    if ((key == 21 || key == 22 || key == 23 || key == 33 || key == 50 || key == 51 || key == 71 || key == 108) &&
        (x < 0 || x > 9999 || std::floor(x) != x)) return false;
    if ((key == 32 || key == 128 || key == 129) && (x <= 0 || x > 100)) return false;
    return true;
}
inline bool decimal(std::string const& s) {
    if (s.empty() || s.size() > 24) return false;
    // No separators, exponent, whitespace, nan or infinity.
    int dots = 0, digits = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char c = s[i];
        if (c >= '0' && c <= '9') ++digits;
        else if (c == '.' && ++dots == 1) {}
        else if ((c == '-' || c == '+') && i == 0) {}
        else return false;
    }
    if (!digits) return false;
    char* end = nullptr;
    double n = std::strtod(s.c_str(), &end);
    return end && *end == '\0' && std::isfinite(n) && std::abs(n) <= 1000000;
}
inline bool groups(std::string const& s) {
    if (s.empty() || s.size() > 49) return false;
    int count = 0;
    size_t pos = 0;
    while (pos < s.size()) {
        auto end = s.find('.', pos);
        if (end == std::string::npos) end = s.size();
        auto item = s.substr(pos, end - pos);
        if (item.empty() || item.size() > 4) return false;
        for (unsigned char c : item) if (c < '0' || c > '9') return false;
        if (++count > 10 || std::stoi(item) > 9999) return false;
        if (end == s.size()) return true;
        pos = end + 1;
        if (pos == s.size()) return false;
    }
    return false;
}
inline bool hsv(std::string const& s) {
    if (s.empty() || s.size() > 90) return false;
    size_t pos = 0;
    for (int i = 0; i < 5; ++i) {
        auto end = s.find('a', pos);
        if (i == 4 ? end != std::string::npos : end == std::string::npos) return false;
        if (end == std::string::npos) end = s.size();
        auto part = s.substr(pos, end - pos);
        if (!decimal(part)) return false;
        if (i >= 3 && part != "0" && part != "1") return false;
        pos = end + 1;
    }
    return true;
}
inline bool textEncoded(std::string const& s) {
    if (s.empty() || s.size() > 512) return false;
    for (unsigned char c : s)
        if (!std::isalnum(c) && c != '+' && c != '/' && c != '-' && c != '_' && c != '=') return false;
    return true;
}
inline bool text(int key, std::string const& s) {
    if (key == 31) return textEncoded(s);
    if (key == 43 || key == 44 || key == 49) return hsv(s);
    if (key == 57 || key == 274) return groups(s);
    return false;
}
} // namespace builder
