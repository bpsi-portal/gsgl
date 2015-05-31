
#ifndef __gui_items_h__
#define __gui_items_h__

struct _gsglGuiitems
{
	gsglboolean External_tool_kit;
	gsgllist *itemlist;
	gsgllist *itemlist2;
}; 

typedef struct _itemdata {
	float x;
	float y;
	float w;
	float h;
	int fnt;
	float chh;
        float chw;
        float chsp;
        float angl;
        float tilt;
	int type;
	gsglDisplaylist *displaylist;
	gsglPage *p;
} itemdata;

typedef struct _guialldata {
	itemdata *common;
	void *specific;
} guialldata;

typedef struct _labeldata {
	char *text;
	int ijust;
} labeldata;

typedef struct _buttondata {
	char *text;
	void (*callback)(void);
} buttondata;

typedef struct _checkbuttondata {
	char *text;
	int init;
	void (*callback)(int *);
} checkbuttondata;

typedef struct _comboboxdata {
	int items;
	char **text;
	int current;
	void (*callback)(int *);
	int fortran;
} comboboxdata;

typedef struct _lineeditdata {
	int max;
	char *text;
	void (*callback)(const char *, int *);
} lineeditdata;

typedef struct _spinbuttondataf {
	float init;
	float lower;
	float upper;
	int digits;
	void (*callback)(float *);
} spinbuttondataf;

typedef struct _spinbuttondatai {
	int init;
	int lower;
	int upper;
	void (*callback)(int *);
} spinbuttondatai;

typedef struct _itemnode
{
	int toolkit;
	unsigned int id;
	guialldata *gdata;
	void *widget;
	void *other;
} itemnode;

#define TK_GSGL  1
#define TK_TCPIP 2
#define TK_QT    3
#define TK_GTK   4

#define ITEM_BUTTON   1
#define ITEM_COMBO    2
#define ITEM_SPINF    3
#define ITEM_CHECK    4
#define ITEM_EDIT     5
#define ITEM_LABEL    6
#define ITEM_SPINI    7

#define JUST_LEFT     0
#define JUST_RIGHT    1
#define JUST_CENTER   2

#ifdef __cplusplus
extern "C"{
#endif

gsglboolean gsgl_inq_guiitems_external (gsglPage *);
void gsgl_set_guiitems_external (gsglboolean b, gsglPage *);
void gsgl_to_x11 (float x, float y, float w, float h, int *x11, int *y11, int *width, int *height, gsglPage *p);
void gsgl_init_guiitems_enery_page (gsglPage *);
void gsgl_finish_guiitems_every_page (gsglPage *);
void gsgl_guiitems_configure (gsglPage *);

void gsgl_add_label_external (guialldata *);
void gsgl_inq_label_size (const char *text, float *w, float *h, gsglPage *p);
void gsgl_add_button_external (guialldata *);
void gsgl_add_combobox_external (guialldata *);
void gsgl_add_spinbuttonf_external (guialldata *);
void gsgl_add_spinbuttoni_external (guialldata *);
void gsgl_add_checkbutton_external (guialldata *);
void gsgl_add_lineedit_external (guialldata *);
void gsgl_guiitems_update_memlist (gsglPage *p);
void gsgl_guiitem_append_item (int toolkit, guialldata *data, void *widget, void *other);
unsigned int find_id_from_widget (void *w, gsglPage *);
gsgllist * find_list_from_id (unsigned int id, gsglPage *);
gsgllist * gsgl_guiitem_inq_list (gsglPage *);
void gsgl_guiitems_button_clicked (guialldata *alldata, void *widget);
void gsgl_guiitems_checkbutton_toggled (guialldata *alldata, void *widget);
void gsgl_guiitems_combobox_selected (guialldata *alldata, void *widget);
void gsgl_guiitems_lineedit_changed (guialldata *alldata, void *widget);
void gsgl_guiitems_spinbuttonf_changed (guialldata *alldata, void *widget);
void gsgl_guiitems_spinbuttoni_changed (guialldata *alldata, void *widget);

#ifdef __cplusplus
}
#endif

#endif
