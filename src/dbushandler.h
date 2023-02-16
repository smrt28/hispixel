#ifndef DBUSHANDLER_H
#define DBUSHANDLER_H

#include <string>

namespace s28 {
class HisPixelApp;

class DbusHandler {
public:
    DbusHandler(HisPixelApp &hispixel);
    std::string dump(std::string s);

private:
    HisPixelApp &hispixel;
};

} // namespace s28
#endif /* DBUSHANDLER_H */
