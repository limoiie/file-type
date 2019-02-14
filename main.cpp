#include <iostream>

#include "CFileChecker.h"
#include "CMagicSetHelper.h"
#include "logger.h"

using namespace limo_ns;
using namespace std;

void init_log() {
    auto const logger_name = "logger";
    log::create_logger(logger_name, true, false, "log/local_log", spdlog::level::trace);
    log::set_default_logger(logger_name);
}


int main() {
    init_log();

    list<string> file_list = { "magic.mgc" };

    auto ptr = CMagicSetHelper::create().load(file_list).release();
    auto p_magic_set = shared_ptr<CMagicSet>(ptr);
    CFileChecker checker(p_magic_set);
    auto type = checker.check_file("file name");
    cout << type.to_string() << endl;

    return 0;
}