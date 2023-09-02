/**
 * 具有层级的uri
 * */

#ifndef ESP32VUE_LEVELURI_H
#define ESP32VUE_LEVELURI_H

#include "Uri.h"

class LevelUri : public Uri{
public:
    explicit LevelUri(const char *uri);

    explicit LevelUri(const String &uri);

    bool canHandle(const String &requestUri, std::vector<String> &pathArgs) override;

Uri * clone() const override;
};


#endif //ESP32VUE_LEVELURI_H
