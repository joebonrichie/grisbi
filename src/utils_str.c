/* ************************************************************************** */
/*                                  utils_str.c                               */
/*                                                                            */
/*     Copyright (C)	2000-2008 Cedric Auger (cedric@grisbi.org)	      */
/*			2003-2008 Benjamin Drieu (bdrieu@april.org)	      */
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
#include <stdlib.h>


/*START_INCLUDE*/
#include "utils_str.h"
#include "./include.h"
/*END_INCLUDE*/

/*START_STATIC*/
/*END_STATIC*/


/*START_EXTERN*/
extern gint max;
/*END_EXTERN*/



/*!
 * @brief convert an integer into a gchar based string
 * 
 * 
 * @param integer   integer to convert
 * 
 * @return  a newly allocated string
 * 
 * @caveats You have to unallocate the returned string when you no more use it to save memory
 *
 * @todo: check usage of this function which a cause of memory leak
 * 
 */
gchar *itoa ( gint integer )
{
    div_t result_div;
    gchar *chaine;
    gint i = 0;
    gint num;

    chaine = g_malloc0 ( 11*sizeof (gchar) );
    
    num = abs(integer);

    // Construct the result in the reverse order from right to left, then reverse it.
    do
    {
	result_div = div ( num, 10 );
	chaine[i] = result_div.rem + '0'; 
	i++;
    }
    while ( ( num = result_div.quot ));

    // Add the sign at the end of the string just before to reverse it to avoid
    // to have to insert it at the begin just after...
    if (integer < 0)
    {
        chaine[i++] = '-';
    }
    
    chaine[i] = 0;

    g_strreverse ( chaine );

    return ( chaine );
}


/**
 * used for now only while loading a file before the 0.6.0 version
 * reduce the exponant IN THE STRING of the amount because before the 0.6.0
 * all the gdouble where saved with an exponent very big
 * ie : in the file we have "12.340000"
 * and that fonction return "12.34" wich will be nicely imported
 * with gsb_data_transaction_set_amount_from_string
 *
 * \param amount_string
 * \param exponent the exponent we want at the end (normally always 2, but if ever...)
 *
 * \return a newly allocated string with only 'exponent' digits after the separator (need to be freed). This function returns NULL if the amount_string parameter is NULL.
 * */
gchar *utils_str_reduce_exponant_from_string ( const gchar *amount_string,
					       gint exponent )
{
    gint i=0;
    gchar *return_string;

    if (!amount_string)
	return NULL;

    return_string = my_strdup (amount_string);

    while (i != -1
	   &&
	   return_string[i])
    {
	if ( return_string[i] == '.'
	     ||
	     return_string[i] == ',' )
	{
	    return_string[i+exponent+1] = 0;
	    i = -1;
	}
	else
	    i++;
    }
    return return_string;
}




/***********************************************************************************************************/
/* cette fonction protÃšge atoi qui plante quand on lui envoie un null */
/***********************************************************************************************************/
/*!
 * @brief Secured version of atoi
 * 
 * Encapsulated call of atoi which may crash when it is call with a NULL pointer.
 * 
 * @param chaine   pointer to the buffer containing the string to convert
 * 
 * @return  the converted string as interger
 * @retval  0 when the pointer is NULL or the string empty.
 * 
 */
gint utils_str_atoi ( const gchar *chaine )
{
    if ((chaine )&&(*chaine))
    {
        return ( atoi ( chaine ));
    }
    else
    {
        return ( 0 );
    }
}
/***********************************************************************************************************/



/******************************************************************************/
/* Fonction my_strtod (string to decimal)                                    */
/* Convertie une chaine de caractÃšres en un nombre                            */
/* ParamÃštres d'entrÃ©e :                                                      */
/*   - nptr : pointeur sur la chaine de caractÃšres Ã  convertir                */
/*   - endptr : n'est pas utilisÃ©, alors Ã  quoi peut-il bien servir ?         */
/* Valeur de retour :                                                         */
/*   - resultat : le rÃ©sultat de la conversion                                */
/* Variables locales :                                                        */
/*   - entier : la partie entiÃšre du rÃ©sultat                                 */
/*   - mantisse : la partie dÃ©cimale du rÃ©sultat                              */
/*   - invert : le signe du rÃ©sultat (0 -> positif, 1 -> nÃ©gatif)             */
/*   - p, m : pointeurs locaux sur la chaine de caractÃšres Ã  convertir        */
/******************************************************************************/
double my_strtod ( const char *nptr, const char **endptr )
{
    double entier=0, mantisse=0, resultat=0;
    int invert = 0;
    const char * p;

    if (!nptr)
	return 0;


    for ( p = nptr; p < nptr + strlen(nptr); p++ )
    {
	if (g_ascii_isspace(*p) || *p == '+' )
	    continue;

	if (*p == '-')
	{
	    invert = 1;
	    continue;
	}

	if ( *p == ',' || *p == '.' )
	{
	    const char * m;
	    for ( m = p+1; m <= nptr+strlen(nptr) && 
		  (g_ascii_isdigit(*m) || g_ascii_isspace(*m)); m++)
		/* Nothing, just loop */ ;
	    for ( --m; m > p; m-- )
	    {
		if (isdigit(*m))
		{
		    mantisse /= 10;
		    mantisse += (*m - '0');
		}
	    }
	    mantisse /= 10;
	}

	if (isdigit(*p))
	{
	    entier = entier * 10;
	    entier += (*p - '0');
	}
	else
	{
	    break;
	}
    }

    resultat = entier + mantisse;
    if ( invert )
	resultat = - resultat;

    return resultat;
}



gchar * latin2utf8 ( const gchar * inchar)
{
    return g_locale_from_utf8 ( inchar, -1, NULL, NULL, NULL );
}



/* 
 * do the same as g_strdelimit but new_delimiters can containes several characters or none
 * ex	my_strdelimit ("a-b", "-", "123") returns a123b
 * 	my_strdelimit ("a-b", "-", "") returns ab
 *
 * \param string the string we want to modify
 * \param delimiters the characters we need to change to new_delimiters
 * \param new_delimiters the replacements characters for delimiters
 *
 * \return a newly allocated string or NULL
 * */
gchar *my_strdelimit ( const gchar *string,
		       const gchar *delimiters,
		       const gchar *new_delimiters )
{
    gchar **tab_str;
    gchar *retour;

    if ( !( string
	    &&
	    delimiters
	    &&
	    new_delimiters ))
	return my_strdup (string);

    tab_str = g_strsplit_set ( string,
			       delimiters,
			       0 );
    retour = g_strjoinv ( new_delimiters,
			  tab_str );
    g_strfreev ( tab_str );

    return ( retour );
}


/* ******************************************************************************* */
/* my_strcasecmp : compare 2 chaines sensitive que ce soit utf8 ou ascii */
/* ******************************************************************************* */
/* TODO dOm : this function seems not to be used. Is it possible to remove it 
gint my_strcmp ( gchar *string_1,
		 gchar *string_2 )
{
    if ( string_1
	 &&
	 string_2 )
    {
	if ( g_utf8_validate ( string_1, -1, NULL )
	     &&
	     g_utf8_validate ( string_2, -1, NULL ))
	{
	    gint retour;
 	    gchar *new_1, *new_2;
	    
	    new_1 = g_utf8_collate_key ( string_1,
					 -1 );
	    new_2 = g_utf8_collate_key ( string_2,
					 -1 );
	    retour = strcmp ( new_1,
			      new_2 );
	    g_free ( new_1 );
	    g_free ( new_2 );
	    return ( retour );
	}
	else
	    return ( strcmp ( string_1,
			      string_2 ));
    }

    return 0;
}
*/
/* ******************************************************************************* */




/**
 * do the same as g_strcasecmp but works alse with the accents on the words
 * 
 * \param string_1 the first string to cmp
 * \param string_2 the second string to cmp
 * 
 * \return -1 if string_1 berfore string_2
 * */
gint gsb_strcasecmp ( gchar *string_1,
		      gchar *string_2 )
{
    string_1 = my_strdup ( string_1 );
    string_1 = g_strdelimit ( string_1, "éÉèÈêÊ", 'e' );
    string_1 = g_strdelimit ( string_1, "çÇ", 'c' );
    string_1 = g_strdelimit ( string_1, "àÀ", 'a' );
    string_1 = g_strdelimit ( string_1, "ùûÙÛ", 'u' );
    string_1 = g_strdelimit ( string_1, "ôÔ", 'o' );
    string_1 = g_strdelimit ( string_1, "îÎ", 'i' );

    string_2 = my_strdup ( string_2 );
    string_2 = g_strdelimit ( string_2, "éÉèÈêÊ", 'e' );
    string_2 = g_strdelimit ( string_2, "çÇ", 'c' );
    string_2 = g_strdelimit ( string_2, "àÀ", 'a' );
    string_2 = g_strdelimit ( string_2, "ùûÙÛ", 'u' );
    string_2 = g_strdelimit ( string_2, "ôÔ", 'o' );
    string_2 = g_strdelimit ( string_2, "îÎ", 'i' );

    gint result = g_strcasecmp ( string_1, string_2 );
    g_free(string_1);
    g_free(string_2);
    return result; 
}
/* ************************************************************************** */



/**
 * compare 2 strings unsensitive
 * if a string is NULL, it will go after the non NULL
 *
 * \param string_1
 * \param string_2
 *
 * \return -1 string_1 before string_2 (or string_2 NULL) ; 0 if same or NULL everyone ; +1 if string_1 after string_2 (or string_1 NULL)
 * */
gint my_strcasecmp ( const gchar *string_1,
		     const gchar *string_2 )
{
    if (!string_1 && string_2)
	return 1;
    if (string_1 && !string_2)
	return -1;
    if ( string_1
	 &&
	 string_2 )
    {
	if ( g_utf8_validate ( string_1, -1, NULL )
	     &&
	     g_utf8_validate ( string_2, -1, NULL ))
	{
	    gint retour;
 	    gchar *new_1, *new_2;
	    
	    new_1 = g_utf8_collate_key ( g_utf8_casefold ( string_1,-1 ),
					 -1 );
	    new_2 = g_utf8_collate_key ( g_utf8_casefold (  string_2,-1 ),
					 -1 );
	    retour = strcmp ( new_1,
			      new_2 );
	    g_free ( new_1 );
	    g_free ( new_2 );
	    return ( retour );
	}
	else
	    return ( g_ascii_strcasecmp ( string_1,
					  string_2 ));
    }

    return 0;
}
/* ******************************************************************************* */




/* ******************************************************************************* */
/* my_strncasecmp : compare 2 chaines case-insensitive que ce soit utf8 ou ascii */
/* ******************************************************************************* */
gint my_strncasecmp ( gchar *string_1,
		      gchar *string_2,
		      gint longueur )
{
    if ( string_1
	 &&
	 string_2 )
    {
	if ( g_utf8_validate ( string_1, -1, NULL )
	     &&
	     g_utf8_validate ( string_2, -1, NULL ))
	{
	    gint retour;
 	    gchar *new_1, *new_2;
	    
	    new_1 = g_utf8_collate_key ( g_utf8_casefold ( string_1,longueur ),
					 longueur );
	    new_2 = g_utf8_collate_key ( g_utf8_casefold (  string_2,longueur ),
					 longueur );
	    retour = strcasecmp ( new_1, new_2);
	    g_free ( new_1 );
	    g_free ( new_2 );
	    return ( retour );
	}
	else
	    return ( g_ascii_strncasecmp ( string_1,
					   string_2,
					   longueur ));
    }

    return 0;
}
/* ******************************************************************************* */

/**
 * Protect the my_strdup function if the string is NULL
 * 
 * If the length of string is 0 (ie ""), return NULL.  That is just
 * nonsense, but it has been done that way and disabling it would
 * certainly cause side effects. [benj]
 *
 * \param string the string to be dupped
 *
 * \return a newly allocated string (which is a copy of that string) 
 * or NULL if the parameter is NULL or an empty string. 
 * */
gchar *my_strdup ( const gchar *string )
{
    if ( string && strlen (string) )
	return g_strdup (string);
    else
	return NULL;
}



/**
 * check if the string is maximum to the length
 * if bigger, limit it and set ... at the end
 *
 * \param string the string to check
 * \param length the limit length we want
 *
 * \return a dupplicate version of the string with max length character (must to be freed)
 * */
gchar *limit_string ( gchar *string,
		      gint length )
{
    gchar *string_return;

    if ( !string )
	return NULL;

    if ( strlen ( string ) <= length )
	return my_strdup (string);

    string_return = my_strdup ( string );
    string_return[length-3] = '.';
    string_return[length-2] = '.';
    string_return[length-1] = '.';
    string_return[length] = 0;

    return string_return;
}
/* ******************************************************************************* */



/* ******************************************************************************* */
/* fonction qui rÃ©cupÃšre une ligne de charactÃšre dans une chaine */
/* elle alloue la mÃ©moire nÃ©cessaire et n'incorpore pas le \n final */
/* renvoie NULL en cas de pb */
/* ******************************************************************************* */
gchar *get_line_from_string ( gchar *string )
{
    gchar *pointeur_char;

    if ( !string )
	return NULL;
	    
    pointeur_char = my_strdup ( string );

    pointeur_char = g_strdelimit ( pointeur_char,
				   "\n\r",
				   0 );
    return pointeur_char;
}
/* ******************************************************************************* */


/**
 * return a gslist of integer from a string which the elements are separated
 * by the separator
 *
 * \param string the string we want to change to a list
 * \param delimiter the string which is the separator in the list
 *
 * \return a g_slist or NULL
 * */
GSList *gsb_string_get_int_list_from_string ( const gchar *string,
					      gchar *delimiter )
{
    GSList *list_tmp;
    gchar **tab;
    gint i=0;

    if ( !string
	 ||
	 !delimiter
	 ||
	 !strlen (string)
	 ||
	 !strlen (delimiter))
	return NULL;

    tab = g_strsplit ( string,
		       delimiter,
		       0 );

    list_tmp = NULL;

    while ( tab[i] )
    {
	list_tmp = g_slist_append ( list_tmp,
				    GINT_TO_POINTER ( atoi (tab[i])));
	i++;
    }

    g_strfreev ( tab );

    return list_tmp;
}

/**
 * return a gslist of strings from a string which the elements are separated
 * by the separator
 * (same as gsb_string_get_int_list_from_string but with strings)
 *
 * \param string the string we want to change to a list
 * \param delimiter the string which is the separator in the list
 *
 * \return a g_slist or NULL
 * */
GSList *gsb_string_get_string_list_from_string ( const gchar *string,
						 gchar *delimiter )
{
    GSList *list_tmp;
    gchar **tab;
    gint i=0;

    if ( !string
	 ||
	 !delimiter
	 ||
	 !strlen (string)
	 ||
	 !strlen (delimiter))
	return NULL;

    tab = g_strsplit ( string,
		       delimiter,
		       0 );

    list_tmp = NULL;

    while ( tab[i] )
    {
	list_tmp = g_slist_append ( list_tmp,
				    my_strdup  (tab[i]));
	i++;
    }

    g_strfreev ( tab );

    return list_tmp;
}




/**
 * Create and return a string where underscores are escaped so that
 * gtk_item_factory_get_item won't be confused.
 *
 * \param orig		Original string.
 *
 * \return		A newly-allocated and escaped string.
 */
/* TODO dOm : this function seems not to be used. Is it possible to remove it 
gchar * gsb_string_escape_underscores ( gchar * orig )
{
    gchar new[1024], *iter;

    for ( iter = new; *orig; orig++ )
    {
	if ( *orig == '_' )
	{
	    *iter++ = '_';
	}
	*iter++ = *orig;
    }
    *iter = 0;

    return my_strdup (new);
}
*/



/**
 * Return a newly created strings, truncating original.  It should be
 * truncated at the end of the word containing the 20th letter.
 *
 * \param string	String to truncate.
 *
 * \return A newly-created string.
 */
gchar * gsb_string_truncate ( gchar * string )
{
    return gsb_string_truncate_n ( string, 20, FALSE );
}



/**
 * Return a newly created strings, truncating original.  It should be
 * truncated at the end of the word containing the nth letter.
 *
 * \param string	String to truncate
 * \param n		Max lenght to truncate at.
 * \param hard_trunc	Cut in the middle of a word if needed.
 *
 * \return A newly-created string.
 */
gchar * gsb_string_truncate_n ( gchar * string, int n, gboolean hard_trunc )
{
    gchar * tmp = string, * trunc;

    if ( ! string )
	return NULL;

    if ( strlen(string) < n )
	return my_strdup ( string );
    
    tmp = string + n;
    if ( ! hard_trunc && ! ( tmp = strchr ( tmp, ' ' ) ) )
    {
	/* We do not risk splitting the string in the middle of a
	   UTF-8 accent ... the end is probably near btw. */
	return my_strdup ( string );
    }
    else 
    {
	while ( ! isascii(*tmp) && *tmp )
	    tmp++;

	trunc = g_strndup ( string, ( tmp - string ) );
	gchar* result = g_strconcat ( trunc, "...", NULL );
	g_free(trunc);
	return result;
    }
}

/**
 * all the gtk_entry_set_text in grisbi should be my_gtk_entry_set_text which just
 * check if the text is not NULL, and if NULL, erase the entry
 * the original gtk function returns a warning in that case and do nothing
 *
 * \param entry a GtkEntry
 * \param string the text to set in the entry (it can be NULL)
 *
 * \return
 * */
void my_gtk_entry_set_text ( GtkEntry *entry,
			     const gchar *string )
{
    if (string)
	gtk_entry_set_text (entry, string);
    else
	gtk_entry_set_text (entry, "");
    return;
}


/* Local Variables: */
/* c-basic-offset: 4 */
/* End: */
