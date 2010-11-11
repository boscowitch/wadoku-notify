using Notify;
using GLib;
using Gtk;

Notification not = null;

Gtk.Window window;
Gtk.Label label;
uint Timer;
bool gtk;

bool HideTimer() {
	window.hide_all();
	return false;
}


void notify(char * ctitle, char * ctext) {
	if(gtk) {
		string temp = (string) ctext;
		temp = temp.replace("<","&#60;");
		temp = temp.replace(">","&#62;");
		temp = temp.replace("&","&amp");
	
		label.set_markup("<span font=\"Meiryo 14\">%s</span>\n%s".printf((string)ctitle,(string)ctext));
		window.resize(1,1);
		window.show_all();
		Source.remove(Timer);
		Timer = Timeout.add(5000,HideTimer);
		return;
	}

	if(not!=null)
        	not.update((string)ctitle, (string)ctext, "%s/wadoku.png".printf(workingPath) );
    	else
		not = new Notification((string)ctitle, (string)ctext, "%s/wadoku.png".printf(workingPath), null);
		
	not.set_timeout(5000);
	not.set_urgency(Notify.Urgency.CRITICAL);
	
	try {
		not.show();
	} catch(GLib.Error err) {
		GLib.stderr.printf("Error in notify!\n");
	}
}


