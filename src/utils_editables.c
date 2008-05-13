/* ************************************************************************** */
/*                                  utils_editables.c                    */
/*                                                                            */
/*     Copyright (C)	2000-2003 CÃ©dric Auger (cedric@grisbi.org)	      */
/*			2003-2004 Benjamin Drieu (bdrieu@april.org)	      */
/*			2003-2004 Alain Portal (aportal@univ-montp2.fr)	      */
/* 			http://www.grisbi.org				      */
/*                                                                            */
/*  This program is free software; you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation; either version 2 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program; if not, write to the Free Software               */
/*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */
/*                                                                            */
/* ************************************************************************** */

#include "include.h"


/*START_INCLUDE*/
#include "utils_editables.h"
#include "./gsb_real.h"
#include "./utils_str.h"
#include "./gsb_real.h"
/*END_INCLUDE*/

/*START_STATIC*/
/*END_STATIC*/

/*START_EXTERN*/
extern gsb_real null_real ;
/*END_EXTERN*/




/**
 * create a normalised text view for grisbi and add the value inside
 *
 * \param value a pointer to a string
 *
 * \return a GtkWidget which is a text_view with the string inside
 * */
GtkWidget *gsb_editable_text_view_new ( const gchar *value )
{
    GtkWidget * text_view;
    GtkTextBuffer *buffer;

    text_view = gtk_text_view_new ();
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));
    gtk_text_view_set_pixels_above_lines (GTK_TEXT_VIEW (text_view), 3);
    gtk_text_view_set_pixels_below_lines (GTK_TEXT_VIEW (text_view), 3);
    gtk_text_view_set_left_margin (GTK_TEXT_VIEW (text_view), 3);
    gtk_text_view_set_right_margin (GTK_TEXT_VIEW (text_view), 3);
    gtk_text_view_set_wrap_mode ( GTK_TEXT_VIEW (text_view), GTK_WRAP_WORD );

    if (value)
	gtk_text_buffer_set_text (buffer, value, -1);

    return text_view;
}

/**
 * get the entire content of a text view
 *
 * \param the text_view
 *
 * \return a gchar which is the content of the text view 
 * */
gchar *gsb_editable_text_view_get_content ( GtkWidget *text_view )
{
    GtkTextIter start, end;
    GtkTextBuffer *buffer;

    g_return_val_if_fail (text_view,
			  NULL);
    g_return_val_if_fail (GTK_IS_TEXT_VIEW (text_view),
			  NULL);
    
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));
    gtk_text_buffer_get_iter_at_offset ( buffer, &start, 0 );
    gtk_text_buffer_get_iter_at_offset ( buffer, &end, -1 );

    return (gtk_text_buffer_get_text (buffer, &start, &end, 0));
}



/**
 *  Increment or decrement the value of a GtkEntry.
 *
 * \param entry Entry to change the value of.
 * \param increment Value to add or substract from the numerical value of entry.
 */
void increment_decrement_champ ( GtkWidget *entry, gint increment )
{
    gdouble number;

    number = utils_str_atoi (gtk_entry_get_text ( GTK_ENTRY ( entry )));
    number += increment;

    gchar* tmpstr = itoa ( number );
    gtk_entry_set_text ( GTK_ENTRY ( entry ), tmpstr );
    g_free ( tmpstr );
}


/**
 * calcule le total du contenu de l'entrÃ©e donnÃ©e en argument 
 * accepte les + et les - 
 * 
 * \param entry entrÃ©e d'un montant 
 * 
 * \return gsb_real total de ce qui est dans l'entrÃ©e 
 */
gsb_real gsb_utils_edit_calculate_entry ( GtkWidget *entry )
{
    gchar *string;
    gchar *pointeur;
    gsb_real total = null_real;
	
    string = my_strdup (gtk_entry_get_text ( GTK_ENTRY (entry)));
    pointeur = string + strlen (string);

    while ( pointeur != string )
    {
	if ( pointeur[0] == '+'
	     ||
	     pointeur[0] == '-' )
	{
	    total = gsb_real_add ( total,
				   gsb_real_get_from_string (pointeur));
	    pointeur[0] = 0;
	}
	pointeur--;
    }
    total = gsb_real_add ( total,
			   gsb_real_get_from_string (pointeur));
    g_free (string);
    return total;
}


/* Local Variables: */
/* c-basic-offset: 4 */
/* End: */
