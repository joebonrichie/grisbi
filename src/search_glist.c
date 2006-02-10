/* ************************************************************************** */
/*   									      */
/*  					                                      */
/*                                                                            */
/*                                  search_glist.c                            */
/*                                                                            */
/*     Copyright (C)	2000-2003 Cédric Auger (cedric@grisbi.org)	      */
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
#include "search_glist.h"
#include "gsb_data_transaction.h"
#include "utils_str.h"
#include "structures.h"
/*END_INCLUDE*/

/*START_STATIC*/
static gint recherche_exercice_par_nom ( struct struct_exercice *exercice,
				  gchar *nom );
/*END_STATIC*/




/*START_EXTERN*/
extern GtkWidget *nom_exercice;
/*END_EXTERN*/





/* ************************************************************************************************************ */
/* Fonction recherche_banque_par_no */
/* appelée par g_slist_find_custom */
/* ************************************************************************************************************ */

gint recherche_banque_par_no ( struct struct_banque *banque,
			       gint *no_banque )
{
    return ( !( banque -> no_banque == GPOINTER_TO_INT ( no_banque )) );
}
/* ************************************************************************************************************ */





/******************************************************************************/
gint recherche_rapprochement_par_nom ( struct struct_no_rapprochement *rapprochement,
				       gchar *no_rap )
{
    return ( strcmp ( rapprochement -> nom_rapprochement,
		      no_rap ));
}
/******************************************************************************/

/******************************************************************************/
gint recherche_rapprochement_par_no ( struct struct_no_rapprochement *rapprochement,
				      gint *no_rap )
{
    return ( !(rapprochement -> no_rapprochement == GPOINTER_TO_INT ( no_rap )));
}
/******************************************************************************/




/* ************************************************************************************************************ */
gint recherche_exercice_par_nom ( struct struct_exercice *exercice,
				  gchar *nom )
{
    return ( g_strcasecmp ( g_strstrip ( exercice -> nom_exercice ),
			    nom ) );
}
/* ************************************************************************************************************ */



/***********************************************************************************************************/
/* Fonction recherche_exercice_par_no */
/* appelée par un g_slist_find_custom */
/***********************************************************************************************************/

gint recherche_exercice_par_no ( struct struct_exercice *exercice,
				 gint *no_exercice )
{
    return ( exercice -> no_exercice != GPOINTER_TO_INT ( no_exercice ));
}
/***********************************************************************************************************/






/******************************************************************************/
/* Fonction recherche_operation_par_cheque					      */
/* appelée par un slist_find_custom					      */
/* recherche une opé par son numéro de cheque dans la liste des opérations	      */
/******************************************************************************/
gint recherche_operation_par_cheque ( gpointer operation,
				      gint *no_chq )
{
  if ( gsb_data_transaction_get_method_of_payment_content ( gsb_data_transaction_get_transaction_number (operation )))
    return ( ! ( utils_str_atoi (gsb_data_transaction_get_method_of_payment_content ( gsb_data_transaction_get_transaction_number (operation ))) == GPOINTER_TO_INT ( no_chq ) ));
  return -1;
}
/******************************************************************************/





/* *******************************************************************************/
gint recherche_operation_par_id ( gpointer operation,
				  gchar *id_recherchee )
{
    gint transaction_number;
    
    /* FIXME : à mettre dans gsb_transaction_data.c */
    
    transaction_number = gsb_data_transaction_get_transaction_number (operation);

    if ( gsb_data_transaction_get_transaction_id ( transaction_number ))
	return ( strcmp ( id_recherchee,
			  gsb_data_transaction_get_transaction_id ( transaction_number)));
    else
	return -1;
}
/* *******************************************************************************/



/* ************************************************************************************************************** */
gint recherche_type_ope_par_no ( struct struct_type_ope *type_ope,
				 gint *no_type )
{
    return ( !(type_ope->no_type == GPOINTER_TO_INT(no_type)) );
}
/* ************************************************************************************************************** */


/* ************************************************************************************************************** */
gint cherche_string_equivalente_dans_slist ( gchar *string_list,
					     gchar *string_cmp )
{
    return ( strcmp ( string_list,
		      string_cmp ));
}
/* ************************************************************************************************************** */


/* Local Variables: */
/* c-basic-offset: 4 */
/* End: */
