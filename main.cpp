#include <iostream>

#include "CFileChecker.h"
#include "CMagicSetHelper.h"

using namespace limo_ns;
using namespace std;

int main() {
    list<string> file_list = { "magic.mgc" };

    auto ptr = CMagicSetHelper::create().load(file_list).release();
    auto p_magic_set = shared_ptr<CMagicSet>(ptr);
    CFileChecker checker(p_magic_set);
    auto type = checker.check_file("file name");
    cout << type.to_string() << endl;

    return 0;
}