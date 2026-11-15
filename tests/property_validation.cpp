#include "../src/Properties.hpp"
#include <cassert>
int main() {
    using namespace builder;
    assert(allowedKey(21)); assert(allowedKey(23)); assert(allowedKey(57));
    assert(!allowedKey(1)); assert(!allowedKey(145)); assert(!allowedKey(999999));
    assert(numeric(7, 255)); assert(!numeric(7, 256)); assert(!numeric(7, 1.5));
    assert(numeric(21, 999)); assert(!numeric(21, -1)); assert(!numeric(21, 1.5));
    assert(!numeric(57, 123)); assert(!numeric(32, -1));
    assert(text(57, "1.2.9999")); assert(!text(57, "1;1,1")); assert(!text(57, "1."));
    assert(text(43, "30a1a1a0a0")); assert(!text(43, "30a1a1a0a0;"));
    assert(!text(43, "30a1a1a2a0"));
    assert(text(31, "SGVsbG8=")); assert(!text(31, "SGVsbG8=;1,1"));
}
