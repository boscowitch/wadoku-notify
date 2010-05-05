using Notify;
using GLib;

Notification not = null;

public void notify(char * ctitle, char * ctext) {
	if(not!=null)
        	not.update((string)ctitle, (string)ctext, "./wadoku.png");
    	else
		not = new Notification((string)ctitle, (string)ctext, "./wadoku.png", null);
	not.set_timeout(5000);
	not.set_urgency(Notify.Urgency.CRITICAL);
	try {
		not.show();
	} catch(GLib.Error err) {
		GLib.stderr.printf("Error in notify!\n");
	}
}
