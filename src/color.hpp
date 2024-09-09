#include <map>
#include <string>

class Color {
public:
    enum class ColorName {
        black,
        red,
        yellow,
        green,
        cyan,
        blue,
        magenta,
        white
    };
    inline static int GetHex(ColorName color) { return hex_table.at(color); }
    inline static std::string GetAnsiCode(ColorName color) { return ansi_table.at(color); }

private:
    inline static const std::map<ColorName, int> hex_table = {
        {ColorName::black,   0x00000000},
        {ColorName::red,     0x00FF0000},
        {ColorName::yellow,  0x00FFFF00},
        {ColorName::green,   0x0000FF00},
        {ColorName::cyan,    0x0000FFFF},
        {ColorName::blue,    0x000000FF},
        {ColorName::magenta, 0x00FF00FF},
        {ColorName::white,   0x00FFFFFF}
    };

    inline static const std::map<ColorName, std::string> ansi_table = {
        {ColorName::black,   "\x1b[0m"},
        {ColorName::red,     "\x1b[31m"},
        {ColorName::yellow,  "\x1b[33m"},
        {ColorName::green,   "\x1b[32m"},
        {ColorName::cyan,    "\x1b[36m"},
        {ColorName::blue,    "\x1b[34m"},
        {ColorName::magenta, "\x1b[35m"},
        {ColorName::white,   "\x1b[0m"}
    };
};