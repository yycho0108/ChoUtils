set(CHO_VERSION_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/cho_util/core/include/cho_util/core/version.hpp"
)
file(STRINGS "${CHO_VERSION_FILE}" CHO_VERSION_PARTS
     REGEX "#define CHO_VERSION_[A-Z]+[ ]+")

string(REGEX REPLACE ".+CHO_VERSION_MAJOR[ ]+([0-9]+).*" "\\1"
                     CHO_VERSION_MAJOR "${CHO_VERSION_PARTS}")
string(REGEX REPLACE ".+CHO_VERSION_MINOR[ ]+([0-9]+).*" "\\1"
                     CHO_VERSION_MINOR "${CHO_VERSION_PARTS}")
string(REGEX REPLACE ".+CHO_VERSION_PATCH[ ]+([0-9]+).*" "\\1"
                     CHO_VERSION_PATCH "${CHO_VERSION_PARTS}")
