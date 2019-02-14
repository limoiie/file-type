//
// Created by limo on 2/14/2019.
//

#ifndef FILE_TYPE_LOGGER_H
#define FILE_TYPE_LOGGER_H


#include <string>

#include "Singleton.h"
#include "spdlog/spdlog.h"

namespace limo_ns {

    namespace log {

        using log_level_e = spdlog::level::level_enum;

        std::shared_ptr<spdlog::logger> logger();

#define DEF_LOG(LEVEL, FMT, ...) limo_ns::log::logger()->LEVEL(FMT, __VA_ARGS__);

    }

}

#endif //FILE_TYPE_LOGGER_H
