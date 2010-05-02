/*
    Copyright 2010 by BoscoWitch

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
using GLib;
using Gtk;

extern void lookup(char * str);

public class WadokuNotify : GLib.Object {
    public static const string version = "0.2";
    public static Clipboard clip;
    public static string text;

    public static void clipboard_changed() {
    	text = clip.wait_for_text();
    	if(text != null) {
    		lookup(text);
    	}
    }

    public static int main(string[] args) {
    	Gtk.init(ref args);
    	Notify.init("wadoku-notify");
    	clip = Clipboard.get(Gdk.Atom.intern ("PRIMARY", false));
    	Signal.connect(clip, "owner_change", clipboard_changed , null);
    	
        Gtk.main();
	    return 0;
    }
    
}

