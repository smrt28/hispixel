import gi
gi.require_version("Gtk", "3.0")
from gi.repository import Gtk


class Example(Gtk.Window):
    """Using Pango to get system fonts names"""

    def list_system_fonts(self):
        """Yield system fonts families names using Pango"""
        context = self.create_pango_context()
        for fam in context.list_families():
            yield fam.get_name()


a = Example()
system_fonts = list(a.list_system_fonts())
for sf in system_fonts:
    print(sf)
