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
using Posix;

extern void init_db(char * path,bool mm);
extern void lookup(char * str);

string workingPath;

public class WadokuNotify : GLib.Object {
    public static const string version = "0.2";
    public static Clipboard clip;
    public static string text;
    public static bool memory_mode;

    public static void clipboard_changed() {
    	text = clip.wait_for_text();
    	if(text != null) {
    		if( (text.get_char() >=  0x4E00 && text.get_char() <= 0x9FAF) ||  (text.get_char() >=  0x30A0  && text.get_char() <= 0x30FF) || (text.get_char() >= 0x3040 && text.get_char() <= 0x309F) )
    			lookup(text);
    	}
    }

    public static string getWorkingDirectory() {
    	string exelink;
    	string Path;
	char exepath[1024];
	exelink = "/proc/%d/exe".printf(Posix.getpid());

	Posix.readlink(exelink,exepath);
	Path = (string)exepath;

	Path = Path.slice(0,Path.pointer_to_offset(Path.rchr(-1,'/')));
	return Path;
    }

    public static int main(string[] args) {
    	workingPath = getWorkingDirectory();
    	Gtk.init(ref args);
    	gtk = false;
    	memory_mode = false;
    	for(int i=1; i < args.length; i++) {
    		if (args[i] == "-gtk" ) {
			gtk = true;
			window = new Window (WindowType.POPUP);
			window.skip_taskbar_hint = true;
			window.skip_pager_hint = true;
			window.can_focus = false;
			window.move(Gdk.Screen.width() - 310, Gdk.Screen.height() - 250);
			Gdk.Color color;
			Gdk.Color.parse("black", out color);
			window.modify_bg( Gtk.StateType.NORMAL,color);
			window.border_width = 2;
			window.opacity = 0.75f;
			window.set_default_size(300,-1);
			window.destroy.connect (Gtk.main_quit);
			label = new Label("");
			label.selectable = false;
			label.set_line_wrap(true);
			label.set_size_request(300,-1);
			Gdk.Color fg_color;
			Gdk.Color.parse("white",out fg_color);
			label.modify_fg(StateType.NORMAL,fg_color);
			window.add(label);
			Timer = Timeout.add(5500,HideTimer);
		}
		else if (args[i] == "-memory-cache") {
			memory_mode = true;
		}
		else {
			GLib.stdout.printf("Usage:\n -gtk\tadd this for gtk notify window output instead of standart libnotify\n -memory-cache cache whole databae into memory (needs ca 80MB ram), mitght be faster for very slow harddrive\n");
			return 0;
		}

	}

	if(!gtk) {
		Notify.init("wadoku-notify");
	}

    	init_db(workingPath,memory_mode);
    	clip = Clipboard.get(Gdk.Atom.intern ("PRIMARY", false));
    	Signal.connect(clip, "owner_change", clipboard_changed , null);


        Gtk.main();
	    return 0;
    }

}

