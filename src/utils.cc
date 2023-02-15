#include <gtk/gtk.h>

namespace s28 {
namespace utils {


void apply_gama(gdouble &color, int gama) {
        if (gama == 0) return;

        gdouble diff = (color * gama) / 100.0;
        color += diff;

        if (color > 1) color = 1;
        if (color < 0) color = 0;
}


void apply_gama(GdkRGBA &color, int gama) {
	if (gama == 0) return;
	apply_gama(color.red, gama);
	apply_gama(color.green, gama);
	apply_gama(color.blue, gama);
}

}
}
 
