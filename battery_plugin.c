/*
MIT License

Copyright (c) 2016 apalo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include <sys/stat.h>
#include <fcntl.h>
#include <libxfce4panel/xfce-panel-plugin.h>

#define MALLOC(type) ( (type*)malloc(sizeof(type)) )

// Needed to inform xfce4 about the plugin constructor
static void construct_plugin(XfcePanelPlugin *plugin);
XFCE_PANEL_PLUGIN_REGISTER(construct_plugin);


// The struct containing plugin data
typedef struct _battery_plugin {
	GtkWidget *label;
	int fd;
} battery_plugin;


// Create a new battery_plugin struct on the heap
static battery_plugin *new_battery_plugin(void) {
	battery_plugin *bp = MALLOC(battery_plugin);
	if (bp == NULL) {
		return NULL;
	}
	
	bp->label = gtk_label_new(NULL);
	if (bp->label == NULL) {
		free(bp);
		return NULL;
	}
	
	bp->fd = open("/sys/class/power_supply/BAT0/capacity", O_RDONLY);
	if (bp->fd == -1) {
		gtk_widget_destroy(bp->label);
		free(bp);
		return NULL;
	}
	
	gtk_label_set_width_chars((GtkLabel*)bp->label, 5);
	gtk_widget_show_all(bp->label);
	return bp;
}


// The function which updates the battery capacity label
static gboolean update(gpointer data) {
	battery_plugin *bp = (battery_plugin*)data;
	char buf[5]; // Must have room for 3 digits + '%' + '\0'
	int num = read(bp->fd, buf, 3);
	lseek(bp->fd, 0, SEEK_SET);
	buf[num] = '%';
	buf[num+1] = 0; // Null terminate the string
	gtk_label_set_text((GtkLabel*)bp->label, buf);
	return TRUE;
}


// Destroy the plugin
static void free_data(XfcePanelPlugin *plugin, gpointer data) {
    battery_plugin *bp = (battery_plugin*)data;
    gtk_widget_destroy(bp->label);
    close(bp->fd);
    free(bp);
}


// Entry point for the plugin, create all widgets here
static void construct_plugin(XfcePanelPlugin *plugin) {

	battery_plugin *bp = new_battery_plugin();
	if (bp == NULL) {
		return;
	}
	
    gtk_container_add(GTK_CONTAINER(plugin), bp->label);

    // Xfce4-panel right click menu on plugin
    xfce_panel_plugin_add_action_widget(plugin, bp->label);

    // Signal for freeing memory
    g_signal_connect(G_OBJECT(plugin), "free-data", G_CALLBACK(free_data), bp);

	// Initial battery display
    update(bp);

    // Update battery print out once every minute
    g_timeout_add_seconds(60, update, bp);
}
