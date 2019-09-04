#ifndef DBUSHANDLER_H
#define DBUSHANDLER_H

#include <string>

namespace s28 {
class HisPixelApp;

class DbusHandler {
public:
    DbusHandler(HisPixelApp &hispixel);

    std::string focus(std::string s);
    std::string feed(std::string s);
    std::string opentab(std::string s);
    std::string rename(std::string s);
    std::string dump(std::string s);

private:
    HisPixelApp &hispixel;
};

} // namespace s28
#endif /* DBUSHANDLER_H */
