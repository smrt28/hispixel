#ifndef DBUSHANDLER_H
#define DBUSHANDLER_H

#include <string>

namespace s28 {
class HisPixelApp_t;

class DbusHandler {
public:
    DbusHandler(HisPixelApp_t &hispixel);
    void focus(std::string s);
    void feed(std::string s);
    void opentab(std::string s);
    void rename(std::string s);


private:
    HisPixelApp_t &hispixel;
};

} // namespace s28
#endif /* DBUSHANDLER_H */
