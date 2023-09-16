

#include "LevelUri.h"

LevelUri::LevelUri(const char *uri) : Uri(uri) {}
LevelUri::LevelUri(const String &uri) : Uri(uri) {}

bool LevelUri::canHandle(const String &requestUri, std::vector <String> &pathArgs) {
    return requestUri.startsWith(_uri) || _uri == requestUri  ;
}

Uri *LevelUri::clone() const {
    return new LevelUri(_uri);
}




