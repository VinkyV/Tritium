#include "Utils.h"

namespace Utils {
    std::string formatDataUnit(double data) {
        std::string prefix = "B";
        if (data > 1024) {
            data = data / 1024;
            prefix = "KB";
            if (data > 1024) {
                data = data / 1024;
                prefix = "MB";
                if (data > 1024) {
                    data = data / 1024;
                    prefix = "GB";
                }
            }
        }
        std::stringstream stream;
        stream << std::fixed << std::setprecision(2) << data;
        return stream.str() + prefix;
    }
}