#ifndef _CATEGORIES_ONGLET_H
#define _CATEGORIES_ONGLET_H (1)

#include <gtk/gtk.h>

/* START_INCLUDE_H */
/* END_INCLUDE_H */


/* START_DECLARATION */
GtkTreePath *category_hold_position_get_path ( void );
gboolean category_hold_position_set_expand ( gboolean expand );
gboolean category_hold_position_set_path ( GtkTreePath *path );
GtkTreeStore *category_list_get_tree_store ( void );
GtkWidget *category_list_get_tree_view ( void );
void category_list_exporter_categ ( void );
void category_list_importer_categ ( void );
void category_list_new_category ( void );
GtkWidget *onglet_categories ( void );
void gsb_gui_update_category_toolbar ( void );
void remplit_arbre_categ ( void );
/* END_DECLARATION */
#endif
