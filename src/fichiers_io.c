/* Fichier fichiers_io.c */
/* Contient toutes les proc�dures relatives � l'acc�s au disque */

/*     Copyright (C) 2000-2002  C�dric Auger */
/* 			cedric@grisbi.org */
/* 			http://www.grisbi.org */

/*     This program is free software; you can redistribute it and/or modify */
/*     it under the terms of the GNU General Public License as published by */
/*     the Free Software Foundation; either version 2 of the License, or */
/*     (at your option) any later version. */

/*     This program is distributed in the hope that it will be useful, */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/*     GNU General Public License for more details. */

/*     You should have received a copy of the GNU General Public License */
/*     along with this program; if not, write to the Free Software */
/*     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */


#include "include.h"
#include "structures.h"
#include "variables-extern.c"
#include "en_tete.h"



/****************************************************************************/
/** Proc�dure qui charge les op�rations en m�moire sous forme de structures**/
/** elle rend la main en ayant initialis�e la variable p_tab_nom_de_compte,**/
/** tableau de pointeurs vers chaque compte :                              **/
/****************************************************************************/

gboolean charge_operations ( void )
{
  struct stat buffer_stat;
  xmlDocPtr doc;

  /* v�rification de la permission du fichier */

  stat ( nom_fichier_comptes,
	 &buffer_stat);
  if ( buffer_stat.st_mode != 33152 && etat.alerte_permission )
    dialogue ( _("Attention, les permissions du fichier ne sont pas � \
600\n(Ce message peut �tre d�sactiv� dans les param�tres)") );
 
  /* on commence par ouvrir le fichier en xml */
  /*   si marche pas, essaye d'ouvrir la version 0.3.1 */

  doc = xmlParseFile ( nom_fichier_comptes );

  if ( doc )
    {
      /* v�rifications d'usage */

      if ( !doc->root
	   ||
	   !doc->root->name
	   ||
	   g_strcasecmp ( doc->root->name,
			  _("Grisbi") ))
	{
	  dialogue ( _("Fichier de compte invalide.") );
	  xmlFreeDoc ( doc );
	  return ( FALSE );
	}

      /* r�cup�re la version de fichier */

        if (( !strcmp (  xmlNodeGetContent ( doc->root->childs->childs ),
			 "0.3.2" )))
	  return ( charge_operations_version_0_3_2 ( doc ));

        if (( !strcmp (  xmlNodeGetContent ( doc->root->childs->childs ),
			 "0.3.3" )))
	  return ( charge_operations_version_0_3_2 ( doc ));

        if (( !strcmp (  xmlNodeGetContent ( doc->root->childs->childs ),
			 "0.4.0" )))
	  return ( charge_operations_version_0_4_0 ( doc ));

	/* 	� ce niveau, c'est que que la version n'est pas connue de grisbi, on donne alors */
	/* la version n�cessaire pour l'ouvrir */


	dialogue ( g_strdup_printf ( _("Pour ouvrir ce fichier, il vous faut la version %s de Grisbi "),
				     xmlNodeGetContent ( doc->root->childs->childs->next )));

	xmlFreeDoc ( doc );

	return ( FALSE );
    }
  else
    {
      dialogue ( _("Fichier de compte invalide.") );
      return ( FALSE );
    }
    
  dialogue ( _("Fichier de compte invalide ou inf�rieur � la version 0.3.1.") );

  return ( FALSE );

}
/***********************************************************************************************************/





/*****************************************************************************/
/* ajout de la 0.3.2 et 0.3.3 */
/*****************************************************************************/

gboolean charge_operations_version_0_3_2 ( xmlDocPtr doc )
{
  xmlNodePtr node_1;
  gint i;

  /* message d'avertissement */

  dialogue ( _("Attention, le format de donn�es a chang� ; si vous voulez conserver l'ancien format\nfaire une copie du fichier AVANT de refermer Grisbi ou de sauvegarder le fichier") );

  /* on place node_1 sur les g�n�ralit�s */

  node_1 = doc -> root -> childs;

  /*   on met en place la boucle de node_1, qui va successivement passer */
  /*     par les g�n�ralit�s, les comptes, les �ch�ances ... */

  while ( node_1 )
    {

      /* on r�cup�re ici les g�n�ralit�s */

      if ( !strcmp ( node_1 -> name,
		     _("G�n�ralit�s") ) )
	{
	  xmlNodePtr node_generalites;

	  /* node_generalites va faire le tour des g�n�ralit�s */

	  node_generalites = node_1 -> childs;

	  while ( node_generalites )
	    {
	      if ( !strcmp ( node_generalites -> name,
			     _("Fichier_ouvert") ))
		if ( (etat.fichier_deja_ouvert  = atoi ( xmlNodeGetContent ( node_generalites ))))
		  {
		    if ( etat.force_enregistrement )
		      dialogue ( _("Attention, le fichier semble d�j� ouvert par un autre \
utilisateur ou n'a pas �t� ferm� correctement (plantage ?).\nCependant Grisbi va \
forcer l'enregistrement ; cette option est d�conseill�e\n sauf si vous �tes s�r \
que personne d'autre n'utilise le fichier pour le moment.\nLa d�sactivation de \
cette option s'effectue dans le menu de configuration") );
		    else
		      dialogue ( _("Attention, le fichier semble d�j� ouvert pas un autre \
utilisateur ou n'a pas �t� ferm� correctement (plantage ?).\nVous ne pourrez \
enregistrer vos modification qu'en activant l'option \"Forcer l'enregistrement\" \
des param�tres.") );
		  }
	    

	      if ( !strcmp ( node_generalites -> name,
			     "Backup" ))
		nom_fichier_backup = xmlNodeGetContent ( node_generalites );

	      if ( !strcmp ( node_generalites -> name,
			     "Titre" ))
		titre_fichier = xmlNodeGetContent ( node_generalites );

	      if ( !strcmp ( node_generalites -> name,
			     "Adresse_commune" ))
		adresse_commune	= xmlNodeGetContent ( node_generalites );

	      if ( !strcmp ( node_generalites -> name,
			     "Utilise_exercices" ))
		etat.utilise_exercice = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Utilise_IB" ))
		etat.utilise_imputation_budgetaire = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Utilise_PC" ))
		etat.utilise_piece_comptable = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Utilise_info_BG" ))
		etat.utilise_info_banque_guichet = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Num�ro_devise_totaux_tiers" ))
		no_devise_totaux_tiers = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Type_affichage_des_�ch�ances" ))
		affichage_echeances = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Affichage_echeances_perso_nb_libre" ))
		affichage_echeances_perso_nb_libre = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Type_affichage_perso_echeances" ))
		affichage_echeances_perso_j_m_a = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Num�ro_derniere_operation" ))
		no_derniere_operation= atoi ( xmlNodeGetContent ( node_generalites ));

	      node_generalites = node_generalites -> next;
	    }
	}

      /* on r�cup�re ici les comptes et op�rations */

      if ( !strcmp ( node_1 -> name,
		     "Comptes" ))
	{
	  xmlNodePtr node_comptes;

	  /* node_comptes va faire le tour de l'arborescence des comptes */

	  node_comptes = node_1 -> childs;

	  while ( node_comptes )
	    {

	      /* on va r�cup�rer ici les g�n�ralit�s des comptes */
	      
	      if ( !strcmp ( node_comptes -> name,
			     "G�n�ralit�s" ))
		{
		xmlNodePtr node_generalites;

		node_generalites = node_comptes -> childs;

		while ( node_generalites )
		  {
		    /* r�cup�re l'ordre des comptes */

		    if ( !strcmp ( node_generalites -> name,
				   "Ordre_des_comptes" ))
		      {
			gchar **pointeur_char;

			pointeur_char = g_strsplit ( xmlNodeGetContent ( node_generalites ),
						     "-",
						     0 );

			i = 0;
			ordre_comptes = NULL;

			while ( pointeur_char[i] )
			  {
			    ordre_comptes = g_slist_append ( ordre_comptes,
							     GINT_TO_POINTER ( atoi ( pointeur_char[i] )));
			    i++;
			  }
			g_strfreev ( pointeur_char );

			/* calcule le nb de comptes */

			nb_comptes = g_slist_length ( ordre_comptes );

			/* Cr�ation du tableau de pointeur vers les structures de comptes */

			p_tab_nom_de_compte = malloc ( nb_comptes * sizeof ( gpointer ));
			p_tab_nom_de_compte_variable = p_tab_nom_de_compte;
		      }

		    /* r�cup�re le compte courant */

		    if ( !strcmp ( node_generalites -> name,
				   "Compte_courant" ))
		      {
			compte_courant = atoi ( xmlNodeGetContent ( node_generalites ));
			p_tab_nom_de_compte_courant = p_tab_nom_de_compte + compte_courant;
		      }

		    node_generalites = node_generalites -> next;
		  }
	      }
	    
		/* on r�cup�re les d�tails de chaque compte */

	      if ( !strcmp ( node_comptes -> name,
			     "Compte" ))
	      {
		xmlNodePtr node_nom_comptes;

		/* normalement p_tab_nom_de_compte_variable est d�j� plac� */

		/* on cr�e la structure du compte */

		*p_tab_nom_de_compte_variable = calloc ( 1,
							 sizeof (struct donnees_compte));

		/* les valeurs AFFICHAGE_R et NB_LIGNES_OPE par d�faut */

		AFFICHAGE_R = 0;
		NB_LIGNES_OPE = 3;

		/* on fait le tour dans l'arbre nom, cad : les d�tails, d�tails de type et d�tails des op�rations */

		node_nom_comptes = node_comptes -> childs;

		while ( node_nom_comptes )
		  {
		    /* on r�cup�re les d�tails du compte */

		    if ( !strcmp ( node_nom_comptes -> name,
				   "D�tails" ))
		    {
		      xmlNodePtr node_detail;

		      node_detail = node_nom_comptes -> childs;

		      while ( node_detail )
			{

			  if ( !strcmp ( node_detail -> name,
					 "Nom" ))
			       NOM_DU_COMPTE = xmlNodeGetContent ( node_detail );

			  if ( !strcmp ( node_detail -> name,
					 "No_de_compte" ))
			    NO_COMPTE = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Titulaire" ))
			   TITULAIRE = xmlNodeGetContent ( node_detail );

			  if ( !strcmp ( node_detail -> name,
					 "Type_de_compte" ))
			   TYPE_DE_COMPTE = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Nb_op�rations" ))
			   NB_OPE_COMPTE = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Devise" ))
			   DEVISE = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Banque" ))
			   BANQUE = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Guichet" ))
			   NO_GUICHET = xmlNodeGetContent ( node_detail );

			  if ( !strcmp ( node_detail -> name,
					 "No_compte_banque" ))
			   NO_COMPTE_BANQUE = xmlNodeGetContent ( node_detail );

			  if ( !strcmp ( node_detail -> name,
					 "Cl�_du_compte" ))
			   CLE_COMPTE = xmlNodeGetContent ( node_detail );

			  if ( !strcmp ( node_detail -> name,
					 "Solde_initial" ))
			   SOLDE_INIT = g_strtod ( xmlNodeGetContent ( node_detail ),
						   NULL );

			  if ( !strcmp ( node_detail -> name,
					 "Solde_mini_voulu" ))
			   SOLDE_MINI_VOULU = g_strtod ( xmlNodeGetContent ( node_detail ),
							 NULL );

			  if ( !strcmp ( node_detail -> name,
					 "Solde_mini_autoris�" ))
			   SOLDE_MINI = g_strtod ( xmlNodeGetContent ( node_detail ),
						   NULL );

			  if ( !strcmp ( node_detail -> name,
					 "Solde_courant" ))
			   SOLDE_COURANT = g_strtod ( xmlNodeGetContent ( node_detail ),
						      NULL );

			  if ( !strcmp ( node_detail -> name,
					 "Date_dernier_relev�" ))
			    {
			      gchar **pointeur_char;

			      if ( xmlNodeGetContent ( node_detail ))
				{
				  pointeur_char = g_strsplit ( xmlNodeGetContent ( node_detail ),
							       "/",
							       3 );
				  DATE_DERNIER_RELEVE = g_date_new_dmy ( atoi ( pointeur_char [0] ),
									 atoi ( pointeur_char [1] ),
									 atoi ( pointeur_char [2] ));
				  g_strfreev ( pointeur_char );
				}
			    }

			  if ( !strcmp ( node_detail -> name,
					 "Solde_dernier_relev�" ))
			   SOLDE_DERNIER_RELEVE = g_strtod ( xmlNodeGetContent ( node_detail ),
							     NULL );

			  if ( !strcmp ( node_detail -> name,
					 "Dernier_no_de_rapprochement" ))
			   DERNIER_NO_RAPPROCHEMENT = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Compte_clotur�" ))
			   COMPTE_CLOTURE = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Commentaires" ))
			   COMMENTAIRE = xmlNodeGetContent ( node_detail );

			  if ( !strcmp ( node_detail -> name,
					 "Adresse_du_titulaire" ))
			   ADRESSE_TITULAIRE = xmlNodeGetContent ( node_detail );

			  if ( !strcmp ( node_detail -> name,
					 "Type_d�faut_d�bit" ))
			   TYPE_DEFAUT_DEBIT = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Type_d�faut_cr�dit" ))
			   TYPE_DEFAUT_CREDIT = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Tri_par_type" ))
			   TRI = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Neutres_inclus" ))
			   NEUTRES_INCLUS = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Ordre_du_tri" ))
			    {
			      LISTE_TRI = NULL;

			      if ( xmlNodeGetContent ( node_detail ))
				{
				  gchar **pointeur_char;

				  pointeur_char = g_strsplit ( xmlNodeGetContent ( node_detail ),
							       "/",
							       0 );

				  i = 0;

				  while ( pointeur_char[i] )
				    {
				      LISTE_TRI = g_slist_append ( LISTE_TRI,
								   GINT_TO_POINTER ( atoi ( pointeur_char[i] )));
				      i++;
				    }
				  g_strfreev ( pointeur_char );
				}
			    }
			  node_detail = node_detail -> next;
			}
		    }
		  
		    /* on r�cup�re ici le d�tail des types */

		    if ( !strcmp ( node_nom_comptes -> name,
				   "D�tail_de_Types" ))
		    {
		      xmlNodePtr node_type;

		      node_type = node_nom_comptes -> childs;
		      TYPES_OPES = NULL;

		      while ( node_type )
			{
			  struct struct_type_ope *type;

			  type = calloc ( 1,
					  sizeof ( struct struct_type_ope ));

			  type -> no_type = atoi ( xmlGetProp ( node_type,
								"No" ));
			  type -> nom_type = xmlGetProp ( node_type,
							  "Nom" );
			  type -> signe_type = atoi ( xmlGetProp ( node_type,
								   "Signe" ));
			  type -> affiche_entree = atoi ( xmlGetProp ( node_type,
								       "Affiche_entree" ));
			  type -> numerotation_auto = atoi ( xmlGetProp ( node_type,
									  "Num�rotation_auto" ));
			  type -> no_en_cours = atoi ( xmlGetProp ( node_type,
								    "No_en_cours" ));

			  type -> no_compte = NO_COMPTE;

			  TYPES_OPES = g_slist_append ( TYPES_OPES,
							type );

			  node_type = node_type -> next;
			}
		    }


		    /* on r�cup�re ici le d�tail des op�s */

		    if ( !strcmp ( node_nom_comptes -> name,
				   "D�tail_des_op�rations" ))
		      {
		      xmlNodePtr node_ope;

		      node_ope = node_nom_comptes -> childs;
		      LISTE_OPERATIONS = NULL;

		      while ( node_ope )
			{
			  struct structure_operation *operation;
			  gchar **pointeur_char;
			  gchar *pointeur;

			  operation = calloc ( 1,
					       sizeof (struct structure_operation ));

			  operation -> no_operation = atoi ( xmlGetProp ( node_ope,
							      "No" ));

			  pointeur_char = g_strsplit ( xmlGetProp ( node_ope ,
								    "D" ),
						       "/",
						       3 );
			  operation -> jour = atoi ( pointeur_char[0] );
			  operation -> mois = atoi ( pointeur_char[1] );
			  operation -> annee = atoi ( pointeur_char[2] );
			  operation -> date = g_date_new_dmy ( operation -> jour,
							       operation -> mois,
							       operation -> annee );
			  g_strfreev ( pointeur_char );

			  /* GDC prise en compte de la lecture de la date bancaire */

			  pointeur = xmlGetProp ( node_ope,
						  "Db" );

			  if ( pointeur )
			    {
			      pointeur_char = g_strsplit ( pointeur,
							   "/",
							   3 );
			      operation -> jour_bancaire = atoi ( pointeur_char[0] );
			      operation -> mois_bancaire = atoi ( pointeur_char[1] );
			      operation -> annee_bancaire = atoi ( pointeur_char[2] );

			      if ( operation -> jour_bancaire )
				operation -> date_bancaire = g_date_new_dmy ( operation -> jour_bancaire,
									      operation -> mois_bancaire,
									      operation -> annee_bancaire );
			      else
				operation -> date_bancaire = NULL;

			      g_strfreev ( pointeur_char );
			    }
			  else
			    {
			      operation -> jour_bancaire = 0;
			      operation -> mois_bancaire = 0;
			      operation -> annee_bancaire = 0;
			      operation -> date_bancaire = NULL;
			    }
			  
			  /* GDCFin */

			  operation -> montant = g_strtod ( xmlGetProp ( node_ope,
									 "M" ),
							    NULL );

			  operation -> devise = atoi ( xmlGetProp ( node_ope,
								    "De" ));

			  operation -> une_devise_compte_egale_x_devise_ope = atoi ( xmlGetProp ( node_ope,
												  "Rdc" ));

			  operation -> taux_change = g_strtod ( xmlGetProp ( node_ope,
									     "Tc" ),
								NULL );

			  operation -> frais_change = g_strtod ( xmlGetProp ( node_ope,
									      "Fc" ),
								 NULL );

			  operation -> tiers = atoi ( xmlGetProp ( node_ope,
								   "T" ));

			  operation -> categorie = atoi ( xmlGetProp ( node_ope,
								       "C" ));

			  operation -> sous_categorie = atoi ( xmlGetProp ( node_ope,
									    "Sc" ));

			  operation -> operation_ventilee = atoi ( xmlGetProp ( node_ope,
										"Ov" ));

			  operation -> notes = xmlGetProp ( node_ope,
							    "N" );
			  if ( !strlen ( operation -> notes ))
			    operation -> notes = NULL;

			  operation -> type_ope = atoi ( xmlGetProp ( node_ope,
								      "Ty" ));

			  operation -> contenu_type = xmlGetProp ( node_ope,
								   "Ct" );
			  if ( !strlen ( operation -> contenu_type ))
			    operation -> contenu_type = NULL;

			  operation -> pointe = atoi ( xmlGetProp ( node_ope,
								    "P" ));

			  operation -> auto_man = atoi ( xmlGetProp ( node_ope,
								      "A" ));

			  operation -> no_rapprochement = atoi ( xmlGetProp ( node_ope,
									      "R" ));

			  operation -> no_exercice = atoi ( xmlGetProp ( node_ope,
									 "E" ));

			  operation -> imputation = atoi ( xmlGetProp ( node_ope,
									"I" ));

			  operation -> sous_imputation = atoi ( xmlGetProp ( node_ope,
									     "Si" ));

			  operation -> no_piece_comptable = xmlGetProp ( node_ope,
									 "Pc" );
			  if ( !strlen ( operation -> no_piece_comptable ))
			    operation -> no_piece_comptable = NULL;

			  operation -> info_banque_guichet = xmlGetProp ( node_ope,
									  "Ibg" );
			  if ( !strlen ( operation -> info_banque_guichet ))
			    operation -> info_banque_guichet = NULL;

			  operation -> relation_no_operation = atoi ( xmlGetProp ( node_ope,
										   "Ro" ));

			  operation -> relation_no_compte = atoi ( xmlGetProp ( node_ope,
										"Rc" ));

			  operation -> no_operation_ventilee_associee = atoi ( xmlGetProp ( node_ope,
											    "Va" ));


			  /* on met le compte associ� */
			  
			  operation -> no_compte = NO_COMPTE;

			  LISTE_OPERATIONS = g_slist_append ( LISTE_OPERATIONS,
							      operation);

			  node_ope = node_ope -> next;
			}
		    }
		    node_nom_comptes = node_nom_comptes -> next;
		  }

		/* 		    le compte est fini, on peut mettre � jour qques variables */


		if ( SOLDE_COURANT < SOLDE_MINI_VOULU )
		  MESSAGE_SOUS_MINI_VOULU = 1;
		else
		  MESSAGE_SOUS_MINI_VOULU = 0;

		if ( SOLDE_COURANT < SOLDE_MINI )
		  MESSAGE_SOUS_MINI = 1;
		else
		  MESSAGE_SOUS_MINI = 0;

		/*       la s�lection au d�part est en bas de la liste */

		OPERATION_SELECTIONNEE = GINT_TO_POINTER ( -1 );


		/* on incr�mente p_tab_nom_de_compte_variable pour le compte suivant */

		p_tab_nom_de_compte_variable++;
	      }
	      
	      node_comptes = node_comptes -> next;
	    }
	}
	
      /* on r�cup�re ici les �ch�ances */

      if ( !strcmp ( node_1 -> name,
		     "�ch�ances" ))
	{
	  xmlNodePtr node_echeances;

	  /* node_echeances va faire le tour de l'arborescence des �ch�ances */

	  node_echeances = node_1 -> childs;

	  while ( node_echeances )
	    {
	      /* on va r�cup�rer ici les g�n�ralit�s des �ch�ances */

	      if ( !strcmp ( node_echeances -> name,
			     "G�n�ralit�s" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_echeances -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "Nb_�ch�ances" ))
			nb_echeances = atoi ( xmlNodeGetContent ( node_generalites ));

		      if ( !strcmp ( node_generalites -> name,
				     "No_derni�re_�ch�ance" ))
			no_derniere_echeance = atoi ( xmlNodeGetContent ( node_generalites ));

			node_generalites = node_generalites -> next;
		    }
		}


	      /* on va r�cup�rer ici les �ch�ances */
	      
	      if ( !strcmp ( node_echeances -> name,
			     "D�tail_des_�ch�ances" ))
		{
		  xmlNodePtr node_detail;

		  gsliste_echeances = NULL;

		  node_detail = node_echeances -> childs;

		  while ( node_detail )
		    {
		      struct operation_echeance *operation_echeance;
		      gchar **pointeur_char;

		      operation_echeance = calloc ( 1,
						    sizeof (struct operation_echeance ));

		      operation_echeance -> no_operation = atoi ( xmlGetProp ( node_detail,
								      "No" ));

		      pointeur_char = g_strsplit ( xmlGetProp ( node_detail ,
								"Date" ),
						   "/",
						   3 );
		      operation_echeance -> jour = atoi ( pointeur_char[0] );
		      operation_echeance -> mois = atoi ( pointeur_char[1] );
		      operation_echeance -> annee = atoi ( pointeur_char[2] );
		      operation_echeance -> date = g_date_new_dmy ( operation_echeance -> jour,
								    operation_echeance -> mois,
								    operation_echeance -> annee );
		      g_strfreev ( pointeur_char );

		      operation_echeance -> compte = atoi ( xmlGetProp ( node_detail,
								      "Compte" ));

		      operation_echeance -> montant = g_strtod ( xmlGetProp ( node_detail,
									      "Montant" ),
								 NULL );

		      operation_echeance -> devise = atoi ( xmlGetProp ( node_detail,
									 "Devise" ));

		      operation_echeance -> tiers = atoi ( xmlGetProp ( node_detail,
									"Tiers" ));

		      operation_echeance -> categorie = atoi ( xmlGetProp ( node_detail,
								      "Cat�gorie" ));

		      operation_echeance -> sous_categorie = atoi ( xmlGetProp ( node_detail,
								      "Sous-cat�gorie" ));

		      operation_echeance -> compte_virement = atoi ( xmlGetProp ( node_detail,
								      "Virement_compte" ));

		      operation_echeance -> type_ope = atoi ( xmlGetProp ( node_detail,
								      "Type" ));

		      operation_echeance -> contenu_type = xmlGetProp ( node_detail,
							    "Contenu_du_type" );
		      if ( !strlen ( operation_echeance -> contenu_type ))
			operation_echeance -> contenu_type = NULL;

		      operation_echeance -> no_exercice = atoi ( xmlGetProp ( node_detail,
								      "Exercice" ));

		      operation_echeance -> imputation = atoi ( xmlGetProp ( node_detail,
								      "Imputation" ));

		      operation_echeance -> sous_imputation = atoi ( xmlGetProp ( node_detail,
								      "Sous-imputation" ));

/* 		      operation_echeance -> no_piece_comptable = xmlGetProp ( node_detail, */
/* 								      "Pi�ce_comptable" ); */
/* 		      if ( !strlen ( operation_echeance -> no_piece_comptable )) */
/* 			operation_echeance -> no_piece_comptable = NULL; */

/* 		      operation_echeance -> info_banque_guichet = xmlGetProp ( node_detail, */
/* 								      "Info_baque_guichet" ); */
/* 		      if ( !strlen ( operation_echeance -> info_banque_guichet )) */
/* 			operation_echeance -> info_banque_guichet = NULL; */

		      operation_echeance -> notes = xmlGetProp ( node_detail,
								      "Notes" );
		      if ( !strlen ( operation_echeance -> notes ))
			operation_echeance -> notes = NULL;

		      operation_echeance -> auto_man = atoi ( xmlGetProp ( node_detail,
								      "Automatique" ));

		      operation_echeance -> periodicite = atoi ( xmlGetProp ( node_detail,
								      "P�riodicit�" ));

		      operation_echeance -> intervalle_periodicite_personnalisee = atoi ( xmlGetProp ( node_detail,
								      "Intervalle_p�riodicit�" ));

		      operation_echeance -> periodicite_personnalisee = atoi ( xmlGetProp ( node_detail,
								      "P�riodicit�_personnalis�e" ));

		      if ( strlen ( xmlGetProp ( node_detail ,
						 "Date_limite" )))
			{
			  pointeur_char = g_strsplit ( xmlGetProp ( node_detail ,
								    "Date_limite" ),
						       "/",
						       3 );

			  operation_echeance -> jour_limite = atoi ( pointeur_char[0] );
			  operation_echeance -> mois_limite = atoi ( pointeur_char[1] );
			  operation_echeance -> annee_limite = atoi ( pointeur_char[2] );
			  operation_echeance -> date_limite = g_date_new_dmy ( operation_echeance -> jour_limite,
									       operation_echeance -> mois_limite,
									       operation_echeance -> annee_limite );
			  g_strfreev ( pointeur_char );
			}
		      else
			{
			  operation_echeance -> jour_limite = 0;
			  operation_echeance -> mois_limite = 0;
			  operation_echeance -> annee_limite = 0;
			  operation_echeance -> date_limite = NULL;
			}


		      gsliste_echeances = g_slist_append ( gsliste_echeances,
							   operation_echeance);

		      node_detail = node_detail -> next;
		    }
		}
	      node_echeances = node_echeances -> next;
	    }
	}
	
      /* on r�cup�re ici les tiers */

      if ( !strcmp ( node_1 -> name,
		     "Tiers" ))
	{
	  xmlNodePtr node_tiers;

	  /* node_tiers va faire le tour de l'arborescence des tiers */

	  node_tiers = node_1 -> childs;

	  while ( node_tiers )
	    {
	      /* on va r�cup�rer ici les g�n�ralit�s des tiers */
	      
	      if ( !strcmp ( node_tiers -> name,
			     "G�n�ralit�s" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_tiers -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "Nb_tiers" ))
			nb_enregistrements_tiers = atoi ( xmlNodeGetContent ( node_generalites ));

		      if ( !strcmp ( node_generalites -> name,
				     "No_dernier_tiers" ))
			no_dernier_tiers = atoi ( xmlNodeGetContent ( node_generalites ));


			node_generalites = node_generalites -> next;
		    }
		}


	      /* on va r�cup�rer ici les tiers */
	      
	      if ( !strcmp ( node_tiers -> name,
			     "D�tail_des_tiers" ))
		{
		  xmlNodePtr node_detail;

		  liste_struct_tiers = NULL;
		  node_detail = node_tiers -> childs;

		  while ( node_detail )
		    {
		      struct struct_tiers *tiers;

		      tiers = calloc ( 1,
				       sizeof ( struct struct_tiers ) );

		      tiers -> no_tiers = atoi ( xmlGetProp ( node_detail,
							      "No" ));
		      tiers -> nom_tiers = xmlGetProp ( node_detail,
							"Nom" );
		      tiers -> texte = xmlGetProp ( node_detail,
						    "Informations" );
		      if ( !strlen ( tiers -> texte ))
			tiers -> texte = NULL;

		      tiers -> liaison = atoi ( xmlGetProp ( node_detail,
							     "Liaison" ));
		      
		      liste_struct_tiers = g_slist_append ( liste_struct_tiers,
							    tiers );


		      node_detail = node_detail -> next;
		    }
		}
	      node_tiers = node_tiers -> next;
	    }
	}
	
      /* on r�cup�re ici les cat�gories */

      if ( !strcmp ( node_1 -> name,
		     "Cat�gories" ))
	{
	  xmlNodePtr node_categories;

	  /* node_categories va faire le tour de l'arborescence des cat�gories */

	  node_categories = node_1 -> childs;

	  while ( node_categories )
	    {
	      /* on va r�cup�rer ici les g�n�ralit�s des cat�gories */
	      
	      if ( !strcmp ( node_categories -> name,
			     "G�n�ralit�s" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_categories -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "Nb_cat�gories" ))
			nb_enregistrements_categories = atoi ( xmlNodeGetContent ( node_generalites ));

		      if ( !strcmp ( node_generalites -> name,
				     "No_derni�re_cat�gorie" ))
			no_derniere_categorie = atoi ( xmlNodeGetContent ( node_generalites ));

			node_generalites = node_generalites -> next;
		    }
		}


	      /* on va r�cup�rer ici les cat�gories */
	      
	      if ( !strcmp ( node_categories -> name,
			     "D�tail_des_cat�gories" ))
		{
		  xmlNodePtr node_detail;

		  liste_struct_categories = NULL;
		  node_detail = node_categories -> childs;

		  while ( node_detail )
		    {
		      struct struct_categ *categorie;
		      xmlNodePtr node_sous_categ;
		      
		      categorie = calloc ( 1,
					   sizeof ( struct struct_categ ) );

		      categorie -> no_categ = atoi ( xmlGetProp ( node_detail,
								  "No" ));
		      categorie -> nom_categ = xmlGetProp ( node_detail,
							    "Nom" );
		      categorie -> type_categ = atoi ( xmlGetProp ( node_detail,
								    "Type" ));
		      categorie -> no_derniere_sous_categ = atoi ( xmlGetProp ( node_detail,
										"No_derni�re_sous_cag�gorie" ));

		      /*  pour chaque cat�gorie, on r�cup�re les sous-cat�gories */

		      categorie -> liste_sous_categ = NULL;
		      node_sous_categ = node_detail -> childs;

		      while ( node_sous_categ )
			{
			  struct struct_sous_categ *sous_categ;

			  sous_categ = calloc ( 1,
						sizeof ( struct struct_sous_categ ) );

			  sous_categ -> no_sous_categ = atoi ( xmlGetProp ( node_sous_categ,
									    "No" ));
			  sous_categ -> nom_sous_categ = xmlGetProp ( node_sous_categ,
									     "Nom" );

			  categorie -> liste_sous_categ = g_slist_append ( categorie -> liste_sous_categ,
									   sous_categ );
			  node_sous_categ = node_sous_categ -> next;
			}

		      liste_struct_categories = g_slist_append ( liste_struct_categories,
								 categorie );

		      node_detail = node_detail -> next;
		    }
		}
	      node_categories = node_categories -> next;
	    }
	  /* cr�ation de la liste des cat�g pour le combofix */

	  creation_liste_categ_combofix ();
	}
	
      /* on r�cup�re ici les imputations */

      if ( !strcmp ( node_1 -> name,
		     "Imputations" ))
	{
	  xmlNodePtr node_imputations;

	  /* node_imputations va faire le tour de l'arborescence des imputations */

	  node_imputations = node_1 -> childs;

	  while ( node_imputations )
	    {
	      /* on va r�cup�rer ici les g�n�ralit�s des imputations */
	      
	      if ( !strcmp ( node_imputations -> name,
			     "G�n�ralit�s" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_imputations -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "Nb_imputations" ))
			nb_enregistrements_imputations = atoi ( xmlNodeGetContent ( node_generalites ));

		      if ( !strcmp ( node_generalites -> name,
				     "No_derni�re_imputation" ))
			no_derniere_imputation = atoi ( xmlNodeGetContent ( node_generalites ));

			node_generalites = node_generalites -> next;
		    }
		}


	      /* on va r�cup�rer ici les imputations */
	      
	      if ( !strcmp ( node_imputations -> name,
			     "D�tail_des_imputations" ))
		{
		  xmlNodePtr node_detail;

		  liste_struct_imputation = NULL;
		  node_detail = node_imputations -> childs;

		  while ( node_detail )
		    {
		      struct struct_imputation *imputation;
		      xmlNodePtr node_sous_imputation;
		      
		      imputation = calloc ( 1,
					    sizeof ( struct struct_imputation ) );

		      imputation -> no_imputation = atoi ( xmlGetProp ( node_detail,
								  "No" ));
		      imputation -> nom_imputation = xmlGetProp ( node_detail,
							    "Nom" );
		      imputation -> type_imputation = atoi ( xmlGetProp ( node_detail,
								    "Type" ));
		      imputation -> no_derniere_sous_imputation = atoi ( xmlGetProp ( node_detail,
										"No_derni�re_sous_imputation" ));

		      /*  pour chaque cat�gorie, on r�cup�re les sous-cat�gories */

		      imputation -> liste_sous_imputation = NULL;
		      node_sous_imputation = node_detail -> childs;

		      while ( node_sous_imputation )
			{
			  struct struct_sous_imputation *sous_imputation;

			  sous_imputation = calloc ( 1,
						     sizeof ( struct struct_sous_imputation ) );

			  sous_imputation -> no_sous_imputation = atoi ( xmlGetProp ( node_sous_imputation,
									    "No" ));
			  sous_imputation -> nom_sous_imputation = xmlGetProp ( node_sous_imputation,
									     "Nom" );

			  imputation -> liste_sous_imputation = g_slist_append ( imputation -> liste_sous_imputation,
									   sous_imputation );
			  node_sous_imputation = node_sous_imputation -> next;
			}

		      liste_struct_imputation = g_slist_append ( liste_struct_imputation,
								 imputation );

		      node_detail = node_detail -> next;
		    }
		}
	      node_imputations = node_imputations -> next;
	    }
	  /* cr�ation de la liste des imputations pour le combofix */

	  creation_liste_imputation_combofix ();
	}

	
      /* on r�cup�re ici les devises */

      if ( !strcmp ( node_1 -> name,
		     "Devises" ))
	{
	  xmlNodePtr node_devises;

	  /* node_devises va faire le tour de l'arborescence des devises */

	  node_devises = node_1 -> childs;

	  while ( node_devises )
	    {
	      /* on va r�cup�rer ici les g�n�ralit�s des devises */
	      
	      if ( !strcmp ( node_devises -> name,
			     "G�n�ralit�s" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_devises -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "Nb_devises" ))
			nb_devises = atoi ( xmlNodeGetContent ( node_generalites ));

		      if ( !strcmp ( node_generalites -> name,
				     "No_derni�re_devise" ))
			no_derniere_devise = atoi ( xmlNodeGetContent ( node_generalites ));

			node_generalites = node_generalites -> next;
		    }
		}


	      /* on va r�cup�rer ici les devises */

	      if ( !strcmp ( node_devises -> name,
			     "D�tail_des_devises" ))
		{
		  xmlNodePtr node_detail;

		  liste_struct_devises = NULL;
		  node_detail = node_devises -> childs;

		  while ( node_detail )
		    {
		      struct struct_devise *devise;
		      
		      devise = calloc ( 1,
						     sizeof ( struct struct_devise ));


		      devise -> no_devise = atoi ( xmlGetProp ( node_detail,
						 "No" ));
		      devise -> nom_devise = xmlGetProp ( node_detail,
						 "Nom" );
		      devise -> code_devise = xmlGetProp ( node_detail,
						 "Code" );
		      if ( !strlen ( devise -> code_devise ))
			devise -> code_devise = NULL;

		      devise -> passage_euro = atoi ( xmlGetProp ( node_detail,
						 "Passage_euro" ));

		      if ( strlen ( xmlGetProp ( node_detail,
						 "Date_dernier_change" )))
			{
			  gchar **pointeur_char;

			  pointeur_char = g_strsplit ( xmlGetProp ( node_detail,
								    "Date_dernier_change" ),
						       "/",
						       3 );

			  devise -> date_dernier_change = g_date_new_dmy ( atoi ( pointeur_char[0] ),
									   atoi ( pointeur_char[1] ),
									   atoi ( pointeur_char[2] ));
			  g_strfreev ( pointeur_char );
			}
		      else
			devise -> date_dernier_change = NULL;

		      devise -> une_devise_1_egale_x_devise_2 = atoi ( xmlGetProp ( node_detail,
						 "Rapport_entre_devises" ));
		      devise -> no_devise_en_rapport = atoi ( xmlGetProp ( node_detail,
						 "Devise_en_rapport" ));
		      devise -> change = g_strtod ( xmlGetProp ( node_detail,
								 "Change" ),
						    NULL );

		      liste_struct_devises = g_slist_append ( liste_struct_devises,
							      devise );

		      node_detail = node_detail -> next;
		    }
		}
	      node_devises = node_devises -> next;
	    }
	}
	
      /* on r�cup�re ici les banques */

      if ( !strcmp ( node_1 -> name,
		     "Banques" ))
	{
	  xmlNodePtr node_banques;

	  /* node_banques va faire le tour de l'arborescence des banques */

	  node_banques = node_1 -> childs;

	  while ( node_banques )
	    {
	      /* on va r�cup�rer ici les g�n�ralit�s des banques */
	      
	      if ( !strcmp ( node_banques -> name,
			     "G�n�ralit�s" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_banques -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "Nb_banques" ))
			nb_banques = atoi ( xmlNodeGetContent ( node_generalites ));

		      if ( !strcmp ( node_generalites -> name,
				     "No_derni�re_banque" ))
			no_derniere_banque = atoi ( xmlNodeGetContent ( node_generalites ));


			node_generalites = node_generalites -> next;
		    }
		}


	      /* on va r�cup�rer ici les banques */
	      
	      if ( !strcmp ( node_banques -> name,
			     "D�tail_des_banques" ))
		{
		  xmlNodePtr node_detail;

		  liste_struct_banques = NULL;
		  node_detail = node_banques -> childs;

		  while ( node_detail )
		    {
		      struct struct_banque *banque;
		      
		      banque = calloc ( 1,
						     sizeof ( struct struct_banque ));

		      banque -> no_banque = atoi ( xmlGetProp ( node_detail,
						 "No" ));
		      banque -> nom_banque = xmlGetProp ( node_detail,
						 "Nom" );
		      banque -> code_banque = xmlGetProp ( node_detail,
						 "Code" );
		      if ( !strlen ( banque -> code_banque ))
			banque -> code_banque = NULL;
		      banque -> adr_banque = xmlGetProp ( node_detail,
						 "Adresse" );
		      if ( !strlen ( banque -> adr_banque ))
			banque -> adr_banque = NULL;
		      banque -> tel_banque = xmlGetProp ( node_detail,
						 "T�l" );
		      if ( !strlen ( banque -> tel_banque ))
			banque -> tel_banque = NULL;
		      banque -> email_banque = xmlGetProp ( node_detail,
						 "Mail" );
		      if ( !strlen ( banque -> email_banque ))
			banque -> email_banque = NULL;
		      banque -> web_banque = xmlGetProp ( node_detail,
						 "Web" );
		      if ( !strlen ( banque -> web_banque ))
			banque -> web_banque = NULL;
		      banque -> nom_correspondant = xmlGetProp ( node_detail,
						 "Nom_correspondant" );
		      if ( !strlen ( banque -> nom_correspondant ))
			banque -> nom_correspondant = NULL;
		      banque -> fax_correspondant = xmlGetProp ( node_detail,
						 "Fax_correspondant" );
		      if ( !strlen ( banque -> fax_correspondant ))
			banque -> fax_correspondant = NULL;
		      banque -> tel_correspondant = xmlGetProp ( node_detail,
						 "T�l_correspondant" );
		      if ( !strlen ( banque -> tel_correspondant ))
			banque -> tel_correspondant = NULL;
		      banque -> email_correspondant = xmlGetProp ( node_detail,
						 "Mail_correspondant" );
		      if ( !strlen ( banque -> email_correspondant ))
			banque -> email_correspondant = NULL;
		      banque -> remarque_banque = xmlGetProp ( node_detail,
						 "Remarques" );
		      if ( !strlen ( banque -> remarque_banque ))
			banque -> remarque_banque = NULL;

		      liste_struct_banques = g_slist_append ( liste_struct_banques,
							      banque );

		      node_detail = node_detail -> next;
		    }
		}
	      node_banques = node_banques -> next;
	    }
	}
	
      /* on r�cup�re ici les exercices */

      if ( !strcmp ( node_1 -> name,
		     "Exercices" ))
	{
	  xmlNodePtr node_exercices;

	  /* node_exercices va faire le tour de l'arborescence des exercices */

	  node_exercices = node_1 -> childs;

	  while ( node_exercices )
	    {
	      /* on va r�cup�rer ici les g�n�ralit�s des exercices */
	      
	      if ( !strcmp ( node_exercices -> name,
			     "G�n�ralit�s" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_exercices -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "Nb_exercices" ))
			nb_exercices = atoi ( xmlNodeGetContent ( node_generalites ));

		      if ( !strcmp ( node_generalites -> name,
				     "No_dernier_exercice" ))
			no_derniere_exercice = atoi ( xmlNodeGetContent ( node_generalites ));

			node_generalites = node_generalites -> next;
		    }
		}

	 
	      /* on va r�cup�rer ici les exercices */
	      
	      if ( !strcmp ( node_exercices -> name,
			     "D�tail_des_exercices" ))
		{
		  xmlNodePtr node_detail;

		  liste_struct_exercices = NULL;
		  node_detail = node_exercices -> childs;

		  while ( node_detail )
		    {
		      struct struct_exercice *exercice;
		      
		      exercice = calloc ( 1,
						     sizeof ( struct struct_exercice ));

		      exercice -> no_exercice = atoi ( xmlGetProp ( node_detail,
						 "No" ));
		      exercice -> nom_exercice = xmlGetProp ( node_detail,
						 "Nom" );

		      if ( strlen ( xmlGetProp ( node_detail,
						 "Date_d�but" )))
			{
			  gchar **pointeur_char;

			  pointeur_char = g_strsplit ( xmlGetProp ( node_detail,
								    "Date_d�but" ),
						       "/",
						       3 );

			  exercice -> date_debut = g_date_new_dmy ( atoi ( pointeur_char[0] ),
								    atoi ( pointeur_char[1] ),
								    atoi ( pointeur_char[2] ));
			  g_strfreev ( pointeur_char );
			}
		      else
			exercice -> date_debut = NULL;

		      if ( strlen ( xmlGetProp ( node_detail,
						 "Date_fin" )))
			{
			  gchar **pointeur_char;

			  pointeur_char = g_strsplit ( xmlGetProp ( node_detail,
								    "Date_fin" ),
						       "/",
						       3 );

			  exercice -> date_fin = g_date_new_dmy ( atoi ( pointeur_char[0] ),
								  atoi ( pointeur_char[1] ),
								  atoi ( pointeur_char[2] ));
			  g_strfreev ( pointeur_char );
			}
		      else
			exercice -> date_fin = NULL;

		      exercice -> affiche_dans_formulaire = atoi ( xmlGetProp ( node_detail,
										"Affich�" ));

		      liste_struct_exercices = g_slist_append ( liste_struct_exercices,
								exercice );

		      node_detail = node_detail -> next;
		    }
		}
	      node_exercices = node_exercices -> next;
	    }
	}
	
      /* on r�cup�re ici les rapprochements */

      if ( !strcmp ( node_1 -> name,
		     "Rapprochements" ))
	{
	  xmlNodePtr node_rapprochements;

	  /* node_rapprochements va faire le tour de l'arborescence des rapprochements */

	  node_rapprochements = node_1 -> childs;

	  while ( node_rapprochements )
	    {
	      /* il n'y a pas de g�n�ralit�s ... */

	      /* on va r�cup�rer ici les rapprochements */
	      
	      if ( !strcmp ( node_rapprochements -> name,
			     "D�tail_des_rapprochements" ))
		{
		  xmlNodePtr node_detail;

		  liste_no_rapprochements = NULL;
		  node_detail = node_rapprochements -> childs;

		  while ( node_detail )
		    {
		      struct struct_no_rapprochement *rapprochement;

		      rapprochement = calloc ( 1,
						     sizeof ( struct struct_no_rapprochement ));

		      rapprochement -> no_rapprochement = atoi ( xmlGetProp ( node_detail,
						 "No" ));
		      rapprochement -> nom_rapprochement = xmlGetProp ( node_detail,
									"Nom" );
		      
		      rapprochement -> nom_rapprochement = g_strstrip ( rapprochement -> nom_rapprochement);

		      liste_no_rapprochements = g_slist_append ( liste_no_rapprochements,
								 rapprochement );

		      node_detail = node_detail -> next;
		    }
		}
	      node_rapprochements = node_rapprochements -> next;
	    }
	}
      node_1 = node_1 -> next;
    }


  /* on lib�re la m�moire */

  xmlFreeDoc ( doc );

  /*   on applique la modif des ventils : si une op� ventil�e est < 0, les op�s de ventil ont le m�me signe */
  /* que l'op� ventil�e */
  /*   pour �a, on fait le tour de toutes les op�s, et si on a une op� de ventil, on v�rifie le signe de la ventil */
  /*     associ�e, si elle est n�gative, on inverse le signe */

  for ( i=0 ; i<nb_comptes ; i++ )
    {
      GSList *pointeur_tmp;
      struct structure_operation *operation_associee;

      p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;
      operation_associee = NULL;

      pointeur_tmp = LISTE_OPERATIONS;

      while ( pointeur_tmp )
	{
	  struct structure_operation *operation;

	  operation = pointeur_tmp -> data;

	  if ( operation -> montant
	       &&
	       operation -> no_operation_ventilee_associee )
	    {
	      if ( !operation_associee
		   ||
		   operation -> no_operation_ventilee_associee != operation_associee -> no_operation )
		{
		  GSList *pointeur_2;

		  pointeur_2 = g_slist_find_custom ( LISTE_OPERATIONS,
						     GINT_TO_POINTER ( operation -> no_operation_ventilee_associee ),
						     (GCompareFunc) recherche_operation_par_no );

		  if ( pointeur_2 )
		    operation_associee = pointeur_2 -> data;
		}

	      if ( operation_associee
		   &&
		   operation_associee -> montant < 0 )
		operation -> montant = -operation -> montant;
	    }
	  pointeur_tmp = pointeur_tmp -> next;
	}
    }

  /*   r�cup�re le chemin du logo, qui est dans le fichier � partir de la 0.4.0 */

  chemin_logo = gnome_config_get_string ( g_strconcat ( "/", FICHIER_CONF, "/Affichage/Chemin_du_logo", NULL ));

  if ( !chemin_logo
       ||
       !strlen ( chemin_logo ))
    chemin_logo = CHEMIN_LOGO;

  /* met l'affichage des op�s comme il l'�tait avant */

  initialise_tab_affichage_ope();

  /*   la taille des colonnes est automatique, on y met les anciens rapports */

  etat.largeur_auto_colonnes = 1;
  rapport_largeur_colonnes[0] = 11;
  rapport_largeur_colonnes[1] = 13;
  rapport_largeur_colonnes[2] = 30;
  rapport_largeur_colonnes[3] = 3;
  rapport_largeur_colonnes[4] = 11;
  rapport_largeur_colonnes[5] = 11;
  rapport_largeur_colonnes[6] = 11;


  /* on marque le fichier comme ouvert */

  fichier_marque_ouvert ( TRUE );

  modification_fichier ( TRUE );

  return ( TRUE );
}
/***********************************************************************************************************/




/*****************************************************************************/
/* version 0.4.0 */
/*****************************************************************************/

gboolean charge_operations_version_0_4_0 ( xmlDocPtr doc )
{
  xmlNodePtr node_1;

  /* on place node_1 sur les generalites */

  node_1 = doc -> root -> childs;

  /*   on met en place la boucle de node_1, qui va successivement passer */
  /*     par les generalites, les comptes, les echeances ... */

  while ( node_1 )
    {

      /* on recup�re ici les generalites */

      if ( !strcmp ( node_1 -> name,
		     "Generalites" ) )
	{
	  xmlNodePtr node_generalites;

	  /* node_generalites va faire le tour des generalites */

	  node_generalites = node_1 -> childs;

	  while ( node_generalites )
	    {
	      if ( !strcmp ( node_generalites -> name,
			     "Fichier_ouvert" ))
		if ( (etat.fichier_deja_ouvert  = atoi ( xmlNodeGetContent ( node_generalites ))))
		  {
		    if ( etat.force_enregistrement )
		      dialogue ( _("Attention, le fichier semble dej� ouvert pas un autre \
utilisateur ou n'a pas ete ferme correctement (plantage ?).\nCependant Grisbi va \
forcer l'enregistrement ; cette option est deconseillee\n sauf si vous �tes s�r \
que personne d'autre n'utilise le fichier pour le moment.\nLa desactivation de \
cette option s'effectue dans le menu de configuration") );
		    else
		      dialogue ( _("Attention, le fichier semble dej� ouvert pas un autre \
utilisateur ou n'a pas ete ferme correctement (plantage ?).\nVous ne pourrez \
enregistrer vos modification qu'en activant l'option \"Forcer l'enregistrement\" \
des param�tres.") );
		  }
	    

	      if ( !strcmp ( node_generalites -> name,
			     "Backup" ))
		nom_fichier_backup = xmlNodeGetContent ( node_generalites );

	      if ( !strcmp ( node_generalites -> name,
			     "Titre" ))
		titre_fichier = xmlNodeGetContent ( node_generalites );

	      if ( !strcmp ( node_generalites -> name,
			     "Adresse_commune" ))
		adresse_commune	= xmlNodeGetContent ( node_generalites );

	      if ( !strcmp ( node_generalites -> name,
			     "Utilise_exercices" ))
		etat.utilise_exercice = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Utilise_IB" ))
		etat.utilise_imputation_budgetaire = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Utilise_PC" ))
		etat.utilise_piece_comptable = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Utilise_info_BG" ))
		etat.utilise_info_banque_guichet = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Numero_devise_totaux_tiers" ))
		no_devise_totaux_tiers = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Type_affichage_des_echeances" ))
		affichage_echeances = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Affichage_echeances_perso_nb_libre" ))
		affichage_echeances_perso_nb_libre = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Type_affichage_perso_echeances" ))
		affichage_echeances_perso_j_m_a = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Numero_derniere_operation" ))
		no_derniere_operation= atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Chemin_logo" ))
		chemin_logo = xmlNodeGetContent ( node_generalites );

	      if ( !strcmp ( node_generalites -> name,
			     "Affichage_opes" ))
		{
		  gchar **pointeur_char;
		  gint i, j;

		  pointeur_char = g_strsplit ( xmlNodeGetContent ( node_generalites ),
					       "-",
					       28 );

		  for ( i=0 ; i<4 ; i++ )
		    for ( j=0 ; j< 7 ; j++ )
		      tab_affichage_ope[i][j] = atoi ( pointeur_char[j + i*7]);

		  g_strfreev ( pointeur_char );
		}

	      if ( !strcmp ( node_generalites -> name,
			     "Rapport_largeur_col" ))
		{
		  gchar **pointeur_char;
		  gint i;

		  pointeur_char = g_strsplit ( xmlNodeGetContent ( node_generalites ),
					       "-",
					       7 );

		  for ( i=0 ; i<7 ; i++ )
		    rapport_largeur_colonnes[i] = atoi ( pointeur_char[i]);

		  g_strfreev ( pointeur_char );
		}


	      node_generalites = node_generalites -> next;
	    }
	}

      if ( !chemin_logo
	   ||
	   !strlen ( chemin_logo ))
	chemin_logo = CHEMIN_LOGO;

      /* on recup�re ici les comptes et operations */

      if ( !strcmp ( node_1 -> name,
		     "Comptes" ))
	{
	  xmlNodePtr node_comptes;

	  /* node_comptes va faire le tour de l'arborescence des comptes */

	  node_comptes = node_1 -> childs;

	  while ( node_comptes )
	    {

	      /* on va recuperer ici les generalites des comptes */
	      
	      if ( !strcmp ( node_comptes -> name,
			     "Generalites" ))
		{
		xmlNodePtr node_generalites;

		node_generalites = node_comptes -> childs;

		while ( node_generalites )
		  {
		    /* recup�re l'ordre des comptes */

		    if ( !strcmp ( node_generalites -> name,
				   "Ordre_des_comptes" ))
		      {
			gchar **pointeur_char;
			gint i;

			pointeur_char = g_strsplit ( xmlNodeGetContent ( node_generalites ),
						     "-",
						     0 );

			i = 0;
			ordre_comptes = NULL;

			while ( pointeur_char[i] )
			  {
			    ordre_comptes = g_slist_append ( ordre_comptes,
							     GINT_TO_POINTER ( atoi ( pointeur_char[i] )));
			    i++;
			  }
			g_strfreev ( pointeur_char );

			/* calcule le nb de comptes */

			nb_comptes = g_slist_length ( ordre_comptes );

			/* Creation du tableau de pointeur vers les structures de comptes */

			p_tab_nom_de_compte = malloc ( nb_comptes * sizeof ( gpointer ));
			p_tab_nom_de_compte_variable = p_tab_nom_de_compte;
		      }

		    /* recup�re le compte courant */

		    if ( !strcmp ( node_generalites -> name,
				   "Compte_courant" ))
		      {
			compte_courant = atoi ( xmlNodeGetContent ( node_generalites ));
			p_tab_nom_de_compte_courant = p_tab_nom_de_compte + compte_courant;
		      }

		    node_generalites = node_generalites -> next;
		  }
	      }
	    
		/* on recup�re les details de chaque compte */

	      if ( !strcmp ( node_comptes -> name,
			     "Compte" ))
	      {
		xmlNodePtr node_nom_comptes;

		/* normalement p_tab_nom_de_compte_variable est dej� place */

		/* on cree la structure du compte */

		*p_tab_nom_de_compte_variable = calloc ( 1,
							 sizeof (struct donnees_compte));

		/* on fait le tour dans l'arbre nom, cad : les details, details de type et details des operations */

		node_nom_comptes = node_comptes -> childs;

		while ( node_nom_comptes )
		  {
		    /* on recup�re les details du compte */

		    if ( !strcmp ( node_nom_comptes -> name,
				   "Details" ))
		    {
		      xmlNodePtr node_detail;

		      node_detail = node_nom_comptes -> childs;

		      while ( node_detail )
			{

			  if ( !strcmp ( node_detail -> name,
					 "Nom" ))
			       NOM_DU_COMPTE = xmlNodeGetContent ( node_detail );

			  if ( !strcmp ( node_detail -> name,
					 "No_de_compte" ))
			    NO_COMPTE = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Titulaire" ))
			   TITULAIRE = xmlNodeGetContent ( node_detail );

			  if ( !strcmp ( node_detail -> name,
					 "Type_de_compte" ))
			   TYPE_DE_COMPTE = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Nb_operations" ))
			   NB_OPE_COMPTE = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Devise" ))
			   DEVISE = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Banque" ))
			   BANQUE = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Guichet" ))
			   NO_GUICHET = xmlNodeGetContent ( node_detail );

			  if ( !strcmp ( node_detail -> name,
					 "No_compte_banque" ))
			   NO_COMPTE_BANQUE = xmlNodeGetContent ( node_detail );

			  if ( !strcmp ( node_detail -> name,
					 "Cle_du_compte" ))
			   CLE_COMPTE = xmlNodeGetContent ( node_detail );

			  if ( !strcmp ( node_detail -> name,
					 "Solde_initial" ))
			   SOLDE_INIT = g_strtod ( xmlNodeGetContent ( node_detail ),
						   NULL );

			  if ( !strcmp ( node_detail -> name,
					 "Solde_mini_voulu" ))
			   SOLDE_MINI_VOULU = g_strtod ( xmlNodeGetContent ( node_detail ),
							 NULL );

			  if ( !strcmp ( node_detail -> name,
					 "Solde_mini_autorise" ))
			   SOLDE_MINI = g_strtod ( xmlNodeGetContent ( node_detail ),
						   NULL );

			  if ( !strcmp ( node_detail -> name,
					 "Solde_courant" ))
			   SOLDE_COURANT = g_strtod ( xmlNodeGetContent ( node_detail ),
						      NULL );

			  if ( !strcmp ( node_detail -> name,
					 "Date_dernier_releve" ))
			    {
			      gchar **pointeur_char;

			      if ( xmlNodeGetContent ( node_detail ))
				{
				  pointeur_char = g_strsplit ( xmlNodeGetContent ( node_detail ),
							       "/",
							       3 );
				  DATE_DERNIER_RELEVE = g_date_new_dmy ( atoi ( pointeur_char [0] ),
									 atoi ( pointeur_char [1] ),
									 atoi ( pointeur_char [2] ));
				  g_strfreev ( pointeur_char );
				}
			    }

			  if ( !strcmp ( node_detail -> name,
					 "Solde_dernier_releve" ))
			   SOLDE_DERNIER_RELEVE = g_strtod ( xmlNodeGetContent ( node_detail ),
							     NULL );

			  if ( !strcmp ( node_detail -> name,
					 "Dernier_no_de_rapprochement" ))
			   DERNIER_NO_RAPPROCHEMENT = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Compte_cloture" ))
			   COMPTE_CLOTURE = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Affichage_r" ))
			    AFFICHAGE_R  = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Nb_lignes_ope" ))
			    NB_LIGNES_OPE = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Commentaires" ))
			   COMMENTAIRE = xmlNodeGetContent ( node_detail );

			  if ( !strcmp ( node_detail -> name,
					 "Adresse_du_titulaire" ))
			   ADRESSE_TITULAIRE = xmlNodeGetContent ( node_detail );

			  if ( !strcmp ( node_detail -> name,
					 "Type_defaut_debit" ))
			   TYPE_DEFAUT_DEBIT = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Type_defaut_credit" ))
			   TYPE_DEFAUT_CREDIT = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Tri_par_type" ))
			   TRI = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Neutres_inclus" ))
			   NEUTRES_INCLUS = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Ordre_du_tri" ))
			    {
			      LISTE_TRI = NULL;

			      if ( xmlNodeGetContent ( node_detail ))
				{
				  gchar **pointeur_char;
				  gint i;

				  pointeur_char = g_strsplit ( xmlNodeGetContent ( node_detail ),
							       "/",
							       0 );

				  i = 0;

				  while ( pointeur_char[i] )
				    {
				      LISTE_TRI = g_slist_append ( LISTE_TRI,
								   GINT_TO_POINTER ( atoi ( pointeur_char[i] )));
				      i++;
				    }
				  g_strfreev ( pointeur_char );
				}
			    }
			  node_detail = node_detail -> next;
			}
		    }
		  
		    /* on recup�re ici le detail des types */

		    if ( !strcmp ( node_nom_comptes -> name,
				   "Detail_de_Types" ))
		    {
		      xmlNodePtr node_type;

		      node_type = node_nom_comptes -> childs;
		      TYPES_OPES = NULL;

		      while ( node_type )
			{
			  struct struct_type_ope *type;

			  type = calloc ( 1,
						     sizeof ( struct struct_type_ope ));

			  type -> no_type = atoi ( xmlGetProp ( node_type,
								"No" ));
			  type -> nom_type = xmlGetProp ( node_type,
							  "Nom" );
			  type -> signe_type = atoi ( xmlGetProp ( node_type,
								   "Signe" ));
			  type -> affiche_entree = atoi ( xmlGetProp ( node_type,
								       "Affiche_entree" ));
			  type -> numerotation_auto = atoi ( xmlGetProp ( node_type,
									  "Numerotation_auto" ));
			  type -> no_en_cours = atoi ( xmlGetProp ( node_type,
								    "No_en_cours" ));

			  type -> no_compte = NO_COMPTE;

			  TYPES_OPES = g_slist_append ( TYPES_OPES,
							type );

			  node_type = node_type -> next;
			}
		    }


		    /* on recup�re ici le detail des opes */

		    if ( !strcmp ( node_nom_comptes -> name,
				   "Detail_des_operations" ))
		      {
		      xmlNodePtr node_ope;

		      node_ope = node_nom_comptes -> childs;
		      LISTE_OPERATIONS = NULL;

		      while ( node_ope )
			{
			  struct structure_operation *operation;
			  gchar **pointeur_char;
			  gchar *pointeur;

			  operation = calloc ( 1,
						     sizeof (struct structure_operation ));

			  operation -> no_operation = atoi ( xmlGetProp ( node_ope,
							      "No" ));

			  pointeur_char = g_strsplit ( xmlGetProp ( node_ope ,
								    "D" ),
						       "/",
						       3 );
			  operation -> jour = atoi ( pointeur_char[0] );
			  operation -> mois = atoi ( pointeur_char[1] );
			  operation -> annee = atoi ( pointeur_char[2] );
			  operation -> date = g_date_new_dmy ( operation -> jour,
							       operation -> mois,
							       operation -> annee );
			  g_strfreev ( pointeur_char );

			  /* GDC prise en compte de la lecture de la date bancaire */

			  pointeur = xmlGetProp ( node_ope,
						  "Db" );

			  if ( pointeur )
			    {
			      pointeur_char = g_strsplit ( pointeur,
							   "/",
							   3 );
			      operation -> jour_bancaire = atoi ( pointeur_char[0] );
			      operation -> mois_bancaire = atoi ( pointeur_char[1] );
			      operation -> annee_bancaire = atoi ( pointeur_char[2] );

			      if ( operation -> jour_bancaire )
				operation -> date_bancaire = g_date_new_dmy ( operation -> jour_bancaire,
									      operation -> mois_bancaire,
									      operation -> annee_bancaire );
			      else
				operation -> date_bancaire = NULL;

			      g_strfreev ( pointeur_char );
			    }
			  else
			    {
			      operation -> jour_bancaire = 0;
			      operation -> mois_bancaire = 0;
			      operation -> annee_bancaire = 0;
			      operation -> date_bancaire = NULL;
			    }
			  
			  /* GDCFin */

			  operation -> montant = g_strtod ( xmlGetProp ( node_ope,
									 "M" ),
							    NULL );

			  operation -> devise = atoi ( xmlGetProp ( node_ope,
								    "De" ));

			  operation -> une_devise_compte_egale_x_devise_ope = atoi ( xmlGetProp ( node_ope,
												  "Rdc" ));

			  operation -> taux_change = g_strtod ( xmlGetProp ( node_ope,
									     "Tc" ),
								NULL );

			  operation -> frais_change = g_strtod ( xmlGetProp ( node_ope,
									      "Fc" ),
								 NULL );

			  operation -> tiers = atoi ( xmlGetProp ( node_ope,
								   "T" ));

			  operation -> categorie = atoi ( xmlGetProp ( node_ope,
								       "C" ));

			  operation -> sous_categorie = atoi ( xmlGetProp ( node_ope,
									    "Sc" ));

			  operation -> operation_ventilee = atoi ( xmlGetProp ( node_ope,
										"Ov" ));

			  operation -> notes = xmlGetProp ( node_ope,
							    "N" );
			  if ( !strlen ( operation -> notes ))
			    operation -> notes = NULL;

			  operation -> type_ope = atoi ( xmlGetProp ( node_ope,
								      "Ty" ));

			  operation -> contenu_type = xmlGetProp ( node_ope,
								   "Ct" );
			  if ( !strlen ( operation -> contenu_type ))
			    operation -> contenu_type = NULL;

			  operation -> pointe = atoi ( xmlGetProp ( node_ope,
								    "P" ));

			  operation -> auto_man = atoi ( xmlGetProp ( node_ope,
								      "A" ));

			  operation -> no_rapprochement = atoi ( xmlGetProp ( node_ope,
									      "R" ));

			  operation -> no_exercice = atoi ( xmlGetProp ( node_ope,
									 "E" ));

			  operation -> imputation = atoi ( xmlGetProp ( node_ope,
									"I" ));

			  operation -> sous_imputation = atoi ( xmlGetProp ( node_ope,
									     "Si" ));

			  operation -> no_piece_comptable = xmlGetProp ( node_ope,
									 "Pc" );
			  if ( !strlen ( operation -> no_piece_comptable ))
			    operation -> no_piece_comptable = NULL;

			  operation -> info_banque_guichet = xmlGetProp ( node_ope,
									  "Ibg" );
			  if ( !strlen ( operation -> info_banque_guichet ))
			    operation -> info_banque_guichet = NULL;

			  operation -> relation_no_operation = atoi ( xmlGetProp ( node_ope,
										   "Ro" ));

			  operation -> relation_no_compte = atoi ( xmlGetProp ( node_ope,
										"Rc" ));

			  operation -> no_operation_ventilee_associee = atoi ( xmlGetProp ( node_ope,
											    "Va" ));


			  /* on met le compte associe */
			  
			  operation -> no_compte = NO_COMPTE;

			  LISTE_OPERATIONS = g_slist_append ( LISTE_OPERATIONS,
							      operation);

			  node_ope = node_ope -> next;
			}
		    }
		    node_nom_comptes = node_nom_comptes -> next;
		  }

		/* 		    le compte est fini, on peut mettre � jour qques variables */


		if ( SOLDE_COURANT < SOLDE_MINI_VOULU )
		  MESSAGE_SOUS_MINI_VOULU = 1;
		else
		  MESSAGE_SOUS_MINI_VOULU = 0;

		if ( SOLDE_COURANT < SOLDE_MINI )
		  MESSAGE_SOUS_MINI = 1;
		else
		  MESSAGE_SOUS_MINI = 0;

		/*       la selection au depart est en bas de la liste */

		OPERATION_SELECTIONNEE = GINT_TO_POINTER ( -1 );


		/* on incremente p_tab_nom_de_compte_variable pour le compte suivant */

		p_tab_nom_de_compte_variable++;
	      }
	      
	      node_comptes = node_comptes -> next;
	    }
	}
	
      /* on recup�re ici les echeances */

      if ( !strcmp ( node_1 -> name,
		     "Echeances" ))
	{
	  xmlNodePtr node_echeances;

	  /* node_echeances va faire le tour de l'arborescence des echeances */

	  node_echeances = node_1 -> childs;

	  while ( node_echeances )
	    {
	      /* on va recuperer ici les generalites des echeances */

	      if ( !strcmp ( node_echeances -> name,
			     "Generalites" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_echeances -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "Nb_echeances" ))
			nb_echeances = atoi ( xmlNodeGetContent ( node_generalites ));

		      if ( !strcmp ( node_generalites -> name,
				     "No_derniere_echeance" ))
			no_derniere_echeance = atoi ( xmlNodeGetContent ( node_generalites ));

			node_generalites = node_generalites -> next;
		    }
		}


	      /* on va recuperer ici les echeances */
	      
	      if ( !strcmp ( node_echeances -> name,
			     "Detail_des_echeances" ))
		{
		  xmlNodePtr node_detail;

		  gsliste_echeances = NULL;

		  node_detail = node_echeances -> childs;

		  while ( node_detail )
		    {
		      struct operation_echeance *operation_echeance;
		      gchar **pointeur_char;

		      operation_echeance = calloc ( 1,
						     sizeof (struct operation_echeance ));

		      operation_echeance -> no_operation = atoi ( xmlGetProp ( node_detail,
								      "No" ));

		      pointeur_char = g_strsplit ( xmlGetProp ( node_detail ,
								"Date" ),
						   "/",
						   3 );
		      operation_echeance -> jour = atoi ( pointeur_char[0] );
		      operation_echeance -> mois = atoi ( pointeur_char[1] );
		      operation_echeance -> annee = atoi ( pointeur_char[2] );
		      operation_echeance -> date = g_date_new_dmy ( operation_echeance -> jour,
								    operation_echeance -> mois,
								    operation_echeance -> annee );
		      g_strfreev ( pointeur_char );

		      operation_echeance -> compte = atoi ( xmlGetProp ( node_detail,
								      "Compte" ));

		      operation_echeance -> montant = g_strtod ( xmlGetProp ( node_detail,
									      "Montant" ),
								 NULL );

		      operation_echeance -> devise = atoi ( xmlGetProp ( node_detail,
									 "Devise" ));

		      operation_echeance -> tiers = atoi ( xmlGetProp ( node_detail,
									"Tiers" ));

		      operation_echeance -> categorie = atoi ( xmlGetProp ( node_detail,
								      "Categorie" ));

		      operation_echeance -> sous_categorie = atoi ( xmlGetProp ( node_detail,
								      "Sous-categorie" ));

		      operation_echeance -> compte_virement = atoi ( xmlGetProp ( node_detail,
								      "Virement_compte" ));

		      operation_echeance -> type_ope = atoi ( xmlGetProp ( node_detail,
								      "Type" ));

		      operation_echeance -> contenu_type = xmlGetProp ( node_detail,
							    "Contenu_du_type" );
		      if ( !strlen ( operation_echeance -> contenu_type ))
			operation_echeance -> contenu_type = NULL;

		      operation_echeance -> no_exercice = atoi ( xmlGetProp ( node_detail,
								      "Exercice" ));

		      operation_echeance -> imputation = atoi ( xmlGetProp ( node_detail,
								      "Imputation" ));

		      operation_echeance -> sous_imputation = atoi ( xmlGetProp ( node_detail,
								      "Sous-imputation" ));

		      operation_echeance -> notes = xmlGetProp ( node_detail,
								      "Notes" );
		      if ( !strlen ( operation_echeance -> notes ))
			operation_echeance -> notes = NULL;

		      operation_echeance -> auto_man = atoi ( xmlGetProp ( node_detail,
								      "Automatique" ));

		      operation_echeance -> periodicite = atoi ( xmlGetProp ( node_detail,
								      "Periodicite" ));

		      operation_echeance -> intervalle_periodicite_personnalisee = atoi ( xmlGetProp ( node_detail,
								      "Intervalle_periodicite" ));

		      operation_echeance -> periodicite_personnalisee = atoi ( xmlGetProp ( node_detail,
								      "Periodicite_personnalisee" ));

		      if ( strlen ( xmlGetProp ( node_detail ,
						 "Date_limite" )))
			{
			  pointeur_char = g_strsplit ( xmlGetProp ( node_detail ,
								    "Date_limite" ),
						       "/",
						       3 );

			  operation_echeance -> jour_limite = atoi ( pointeur_char[0] );
			  operation_echeance -> mois_limite = atoi ( pointeur_char[1] );
			  operation_echeance -> annee_limite = atoi ( pointeur_char[2] );
			  operation_echeance -> date_limite = g_date_new_dmy ( operation_echeance -> jour_limite,
									       operation_echeance -> mois_limite,
									       operation_echeance -> annee_limite );
			  g_strfreev ( pointeur_char );
			}
		      else
			{
			  operation_echeance -> jour_limite = 0;
			  operation_echeance -> mois_limite = 0;
			  operation_echeance -> annee_limite = 0;
			  operation_echeance -> date_limite = NULL;
			}


		      gsliste_echeances = g_slist_append ( gsliste_echeances,
							   operation_echeance);

		      node_detail = node_detail -> next;
		    }
		}
	      node_echeances = node_echeances -> next;
	    }
	}
	
      /* on recup�re ici les tiers */

      if ( !strcmp ( node_1 -> name,
		     "Tiers" ))
	{
	  xmlNodePtr node_tiers;

	  /* node_tiers va faire le tour de l'arborescence des tiers */

	  node_tiers = node_1 -> childs;

	  while ( node_tiers )
	    {
	      /* on va recuperer ici les generalites des tiers */
	      
	      if ( !strcmp ( node_tiers -> name,
			     "Generalites" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_tiers -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "Nb_tiers" ))
			nb_enregistrements_tiers = atoi ( xmlNodeGetContent ( node_generalites ));

		      if ( !strcmp ( node_generalites -> name,
				     "No_dernier_tiers" ))
			no_dernier_tiers = atoi ( xmlNodeGetContent ( node_generalites ));


			node_generalites = node_generalites -> next;
		    }
		}


	      /* on va recuperer ici les tiers */
	      
	      if ( !strcmp ( node_tiers -> name,
			     "Detail_des_tiers" ))
		{
		  xmlNodePtr node_detail;

		  liste_struct_tiers = NULL;
		  node_detail = node_tiers -> childs;

		  while ( node_detail )
		    {
		      struct struct_tiers *tiers;

		      tiers = calloc ( 1,
						     sizeof ( struct struct_tiers ) );

		      tiers -> no_tiers = atoi ( xmlGetProp ( node_detail,
							      "No" ));
		      tiers -> nom_tiers = xmlGetProp ( node_detail,
							"Nom" );
		      tiers -> texte = xmlGetProp ( node_detail,
						    "Informations" );
		      if ( !strlen ( tiers -> texte ))
			tiers -> texte = NULL;

		      tiers -> liaison = atoi ( xmlGetProp ( node_detail,
							     "Liaison" ));
		      
		      liste_struct_tiers = g_slist_append ( liste_struct_tiers,
							    tiers );


		      node_detail = node_detail -> next;
		    }
		}
	      node_tiers = node_tiers -> next;
	    }
	}
	
      /* on recup�re ici les categories */

      if ( !strcmp ( node_1 -> name,
		     "Categories" ))
	{
	  xmlNodePtr node_categories;

	  /* node_categories va faire le tour de l'arborescence des categories */

	  node_categories = node_1 -> childs;

	  while ( node_categories )
	    {
	      /* on va recuperer ici les generalites des categories */
	      
	      if ( !strcmp ( node_categories -> name,
			     "Generalites" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_categories -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "Nb_categories" ))
			nb_enregistrements_categories = atoi ( xmlNodeGetContent ( node_generalites ));

		      if ( !strcmp ( node_generalites -> name,
				     "No_derniere_categorie" ))
			no_derniere_categorie = atoi ( xmlNodeGetContent ( node_generalites ));

			node_generalites = node_generalites -> next;
		    }
		}


	      /* on va recuperer ici les categories */
	      
	      if ( !strcmp ( node_categories -> name,
			     "Detail_des_categories" ))
		{
		  xmlNodePtr node_detail;

		  liste_struct_categories = NULL;
		  node_detail = node_categories -> childs;

		  while ( node_detail )
		    {
		      struct struct_categ *categorie;
		      xmlNodePtr node_sous_categ;
		      
		      categorie = calloc ( 1,
						     sizeof ( struct struct_categ ) );

		      categorie -> no_categ = atoi ( xmlGetProp ( node_detail,
								  "No" ));
		      categorie -> nom_categ = xmlGetProp ( node_detail,
							    "Nom" );
		      categorie -> type_categ = atoi ( xmlGetProp ( node_detail,
								    "Type" ));
		      categorie -> no_derniere_sous_categ = atoi ( xmlGetProp ( node_detail,
										"No_derniere_sous_cagegorie" ));

		      /*  pour chaque categorie, on recup�re les sous-categories */

		      categorie -> liste_sous_categ = NULL;
		      node_sous_categ = node_detail -> childs;

		      while ( node_sous_categ )
			{
			  struct struct_sous_categ *sous_categ;

			  sous_categ = calloc ( 1,
						     sizeof ( struct struct_sous_categ ) );

			  sous_categ -> no_sous_categ = atoi ( xmlGetProp ( node_sous_categ,
									    "No" ));
			  sous_categ -> nom_sous_categ = xmlGetProp ( node_sous_categ,
									     "Nom" );

			  categorie -> liste_sous_categ = g_slist_append ( categorie -> liste_sous_categ,
									   sous_categ );
			  node_sous_categ = node_sous_categ -> next;
			}

		      liste_struct_categories = g_slist_append ( liste_struct_categories,
								 categorie );

		      node_detail = node_detail -> next;
		    }
		}
	      node_categories = node_categories -> next;
	    }
	  /* creation de la liste des categ pour le combofix */

	  creation_liste_categ_combofix ();
	}
	
      /* on recup�re ici les imputations */

      if ( !strcmp ( node_1 -> name,
		     "Imputations" ))
	{
	  xmlNodePtr node_imputations;

	  /* node_imputations va faire le tour de l'arborescence des imputations */

	  node_imputations = node_1 -> childs;

	  while ( node_imputations )
	    {
	      /* on va recuperer ici les generalites des imputations */
	      
	      if ( !strcmp ( node_imputations -> name,
			     "Generalites" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_imputations -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "Nb_imputations" ))
			nb_enregistrements_imputations = atoi ( xmlNodeGetContent ( node_generalites ));

		      if ( !strcmp ( node_generalites -> name,
				     "No_derniere_imputation" ))
			no_derniere_imputation = atoi ( xmlNodeGetContent ( node_generalites ));

			node_generalites = node_generalites -> next;
		    }
		}


	      /* on va recuperer ici les imputations */
	      
	      if ( !strcmp ( node_imputations -> name,
			     "Detail_des_imputations" ))
		{
		  xmlNodePtr node_detail;

		  liste_struct_imputation = NULL;
		  node_detail = node_imputations -> childs;

		  while ( node_detail )
		    {
		      struct struct_imputation *imputation;
		      xmlNodePtr node_sous_imputation;
		      
		      imputation = calloc ( 1,
						     sizeof ( struct struct_imputation ) );

		      imputation -> no_imputation = atoi ( xmlGetProp ( node_detail,
								  "No" ));
		      imputation -> nom_imputation = xmlGetProp ( node_detail,
							    "Nom" );
		      imputation -> type_imputation = atoi ( xmlGetProp ( node_detail,
								    "Type" ));
		      imputation -> no_derniere_sous_imputation = atoi ( xmlGetProp ( node_detail,
										"No_derniere_sous_imputation" ));

		      /*  pour chaque categorie, on recup�re les sous-categories */

		      imputation -> liste_sous_imputation = NULL;
		      node_sous_imputation = node_detail -> childs;

		      while ( node_sous_imputation )
			{
			  struct struct_sous_imputation *sous_imputation;

			  sous_imputation = calloc ( 1,
						     sizeof ( struct struct_sous_imputation ) );

			  sous_imputation -> no_sous_imputation = atoi ( xmlGetProp ( node_sous_imputation,
									    "No" ));
			  sous_imputation -> nom_sous_imputation = xmlGetProp ( node_sous_imputation,
									     "Nom" );

			  imputation -> liste_sous_imputation = g_slist_append ( imputation -> liste_sous_imputation,
									   sous_imputation );
			  node_sous_imputation = node_sous_imputation -> next;
			}

		      liste_struct_imputation = g_slist_append ( liste_struct_imputation,
								 imputation );

		      node_detail = node_detail -> next;
		    }
		}
	      node_imputations = node_imputations -> next;
	    }
	  /* creation de la liste des imputations pour le combofix */

	  creation_liste_imputation_combofix ();
	}

	
      /* on recup�re ici les devises */

      if ( !strcmp ( node_1 -> name,
		     "Devises" ))
	{
	  xmlNodePtr node_devises;

	  /* node_devises va faire le tour de l'arborescence des devises */

	  node_devises = node_1 -> childs;

	  while ( node_devises )
	    {
	      /* on va recuperer ici les generalites des devises */
	      
	      if ( !strcmp ( node_devises -> name,
			     "Generalites" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_devises -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "Nb_devises" ))
			nb_devises = atoi ( xmlNodeGetContent ( node_generalites ));

		      if ( !strcmp ( node_generalites -> name,
				     "No_derniere_devise" ))
			no_derniere_devise = atoi ( xmlNodeGetContent ( node_generalites ));

			node_generalites = node_generalites -> next;
		    }
		}


	      /* on va recuperer ici les devises */

	      if ( !strcmp ( node_devises -> name,
			     "Detail_des_devises" ))
		{
		  xmlNodePtr node_detail;

		  liste_struct_devises = NULL;
		  node_detail = node_devises -> childs;

		  while ( node_detail )
		    {
		      struct struct_devise *devise;
		      
		      devise = calloc ( 1,
						     sizeof ( struct struct_devise ));


		      devise -> no_devise = atoi ( xmlGetProp ( node_detail,
						 "No" ));
		      devise -> nom_devise = xmlGetProp ( node_detail,
						 "Nom" );
		      devise -> code_devise = xmlGetProp ( node_detail,
						 "Code" );
		      if ( !strlen ( devise -> code_devise ))
			devise -> code_devise = NULL;

		      devise -> passage_euro = atoi ( xmlGetProp ( node_detail,
						 "Passage_euro" ));

		      if ( strlen ( xmlGetProp ( node_detail,
						 "Date_dernier_change" )))
			{
			  gchar **pointeur_char;

			  pointeur_char = g_strsplit ( xmlGetProp ( node_detail,
								    "Date_dernier_change" ),
						       "/",
						       3 );

			  devise -> date_dernier_change = g_date_new_dmy ( atoi ( pointeur_char[0] ),
									   atoi ( pointeur_char[1] ),
									   atoi ( pointeur_char[2] ));
			  g_strfreev ( pointeur_char );
			}
		      else
			devise -> date_dernier_change = NULL;

		      devise -> une_devise_1_egale_x_devise_2 = atoi ( xmlGetProp ( node_detail,
						 "Rapport_entre_devises" ));
		      devise -> no_devise_en_rapport = atoi ( xmlGetProp ( node_detail,
						 "Devise_en_rapport" ));
		      devise -> change = g_strtod ( xmlGetProp ( node_detail,
								 "Change" ),
						    NULL );

		      liste_struct_devises = g_slist_append ( liste_struct_devises,
							      devise );

		      node_detail = node_detail -> next;
		    }
		}
	      node_devises = node_devises -> next;
	    }
	}
	
      /* on recup�re ici les banques */

      if ( !strcmp ( node_1 -> name,
		     "Banques" ))
	{
	  xmlNodePtr node_banques;

	  /* node_banques va faire le tour de l'arborescence des banques */

	  node_banques = node_1 -> childs;

	  while ( node_banques )
	    {
	      /* on va recuperer ici les generalites des banques */
	      
	      if ( !strcmp ( node_banques -> name,
			     "Generalites" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_banques -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "Nb_banques" ))
			nb_banques = atoi ( xmlNodeGetContent ( node_generalites ));

		      if ( !strcmp ( node_generalites -> name,
				     "No_derniere_banque" ))
			no_derniere_banque = atoi ( xmlNodeGetContent ( node_generalites ));


			node_generalites = node_generalites -> next;
		    }
		}


	      /* on va recuperer ici les banques */
	      
	      if ( !strcmp ( node_banques -> name,
			     "Detail_des_banques" ))
		{
		  xmlNodePtr node_detail;

		  liste_struct_banques = NULL;
		  node_detail = node_banques -> childs;

		  while ( node_detail )
		    {
		      struct struct_banque *banque;
		      
		      banque = calloc ( 1,
						     sizeof ( struct struct_banque ));

		      banque -> no_banque = atoi ( xmlGetProp ( node_detail,
						 "No" ));
		      banque -> nom_banque = xmlGetProp ( node_detail,
						 "Nom" );
		      banque -> code_banque = xmlGetProp ( node_detail,
						 "Code" );
		      if ( !strlen ( banque -> code_banque ))
			banque -> code_banque = NULL;
		      banque -> adr_banque = xmlGetProp ( node_detail,
						 "Adresse" );
		      if ( !strlen ( banque -> adr_banque ))
			banque -> adr_banque = NULL;
		      banque -> tel_banque = xmlGetProp ( node_detail,
						 "Tel" );
		      if ( !strlen ( banque -> tel_banque ))
			banque -> tel_banque = NULL;
		      banque -> email_banque = xmlGetProp ( node_detail,
						 "Mail" );
		      if ( !strlen ( banque -> email_banque ))
			banque -> email_banque = NULL;
		      banque -> web_banque = xmlGetProp ( node_detail,
						 "Web" );
		      if ( !strlen ( banque -> web_banque ))
			banque -> web_banque = NULL;
		      banque -> nom_correspondant = xmlGetProp ( node_detail,
						 "Nom_correspondant" );
		      if ( !strlen ( banque -> nom_correspondant ))
			banque -> nom_correspondant = NULL;
		      banque -> fax_correspondant = xmlGetProp ( node_detail,
						 "Fax_correspondant" );
		      if ( !strlen ( banque -> fax_correspondant ))
			banque -> fax_correspondant = NULL;
		      banque -> tel_correspondant = xmlGetProp ( node_detail,
						 "Tel_correspondant" );
		      if ( !strlen ( banque -> tel_correspondant ))
			banque -> tel_correspondant = NULL;
		      banque -> email_correspondant = xmlGetProp ( node_detail,
						 "Mail_correspondant" );
		      if ( !strlen ( banque -> email_correspondant ))
			banque -> email_correspondant = NULL;
		      banque -> remarque_banque = xmlGetProp ( node_detail,
						 "Remarques" );
		      if ( !strlen ( banque -> remarque_banque ))
			banque -> remarque_banque = NULL;

		      liste_struct_banques = g_slist_append ( liste_struct_banques,
							      banque );

		      node_detail = node_detail -> next;
		    }
		}
	      node_banques = node_banques -> next;
	    }
	}
	
      /* on recup�re ici les exercices */

      if ( !strcmp ( node_1 -> name,
		     "Exercices" ))
	{
	  xmlNodePtr node_exercices;

	  /* node_exercices va faire le tour de l'arborescence des exercices */

	  node_exercices = node_1 -> childs;

	  while ( node_exercices )
	    {
	      /* on va recuperer ici les generalites des exercices */
	      
	      if ( !strcmp ( node_exercices -> name,
			     "Generalites" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_exercices -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "Nb_exercices" ))
			nb_exercices = atoi ( xmlNodeGetContent ( node_generalites ));

		      if ( !strcmp ( node_generalites -> name,
				     "No_dernier_exercice" ))
			no_derniere_exercice = atoi ( xmlNodeGetContent ( node_generalites ));

			node_generalites = node_generalites -> next;
		    }
		}

	 
	      /* on va recuperer ici les exercices */
	      
	      if ( !strcmp ( node_exercices -> name,
			     "Detail_des_exercices" ))
		{
		  xmlNodePtr node_detail;

		  liste_struct_exercices = NULL;
		  node_detail = node_exercices -> childs;

		  while ( node_detail )
		    {
		      struct struct_exercice *exercice;
		      
		      exercice = calloc ( 1,
						     sizeof ( struct struct_exercice ));

		      exercice -> no_exercice = atoi ( xmlGetProp ( node_detail,
						 "No" ));
		      exercice -> nom_exercice = xmlGetProp ( node_detail,
						 "Nom" );

		      if ( strlen ( xmlGetProp ( node_detail,
						 "Date_debut" )))
			{
			  gchar **pointeur_char;

			  pointeur_char = g_strsplit ( xmlGetProp ( node_detail,
								    "Date_debut" ),
						       "/",
						       3 );

			  exercice -> date_debut = g_date_new_dmy ( atoi ( pointeur_char[0] ),
								    atoi ( pointeur_char[1] ),
								    atoi ( pointeur_char[2] ));
			  g_strfreev ( pointeur_char );
			}
		      else
			exercice -> date_debut = NULL;

		      if ( strlen ( xmlGetProp ( node_detail,
						 "Date_fin" )))
			{
			  gchar **pointeur_char;

			  pointeur_char = g_strsplit ( xmlGetProp ( node_detail,
								    "Date_fin" ),
						       "/",
						       3 );

			  exercice -> date_fin = g_date_new_dmy ( atoi ( pointeur_char[0] ),
								  atoi ( pointeur_char[1] ),
								  atoi ( pointeur_char[2] ));
			  g_strfreev ( pointeur_char );
			}
		      else
			exercice -> date_fin = NULL;

		      exercice -> affiche_dans_formulaire = atoi ( xmlGetProp ( node_detail,
										"Affiche" ));

		      liste_struct_exercices = g_slist_append ( liste_struct_exercices,
								exercice );

		      node_detail = node_detail -> next;
		    }
		}
	      node_exercices = node_exercices -> next;
	    }
	}
	
      /* on recup�re ici les rapprochements */

      if ( !strcmp ( node_1 -> name,
		     "Rapprochements" ))
	{
	  xmlNodePtr node_rapprochements;

	  /* node_rapprochements va faire le tour de l'arborescence des rapprochements */

	  node_rapprochements = node_1 -> childs;

	  while ( node_rapprochements )
	    {
	      /* il n'y a pas de generalites ... */

	      /* on va recuperer ici les rapprochements */
	      
	      if ( !strcmp ( node_rapprochements -> name,
			     "Detail_des_rapprochements" ))
		{
		  xmlNodePtr node_detail;

		  liste_no_rapprochements = NULL;
		  node_detail = node_rapprochements -> childs;

		  while ( node_detail )
		    {
		      struct struct_no_rapprochement *rapprochement;

		      rapprochement = calloc ( 1,
						     sizeof ( struct struct_no_rapprochement ));

		      rapprochement -> no_rapprochement = atoi ( xmlGetProp ( node_detail,
						 "No" ));
		      rapprochement -> nom_rapprochement = xmlGetProp ( node_detail,
									"Nom" );
		      
		      rapprochement -> nom_rapprochement = g_strstrip ( rapprochement -> nom_rapprochement);

		      liste_no_rapprochements = g_slist_append ( liste_no_rapprochements,
								 rapprochement );

		      node_detail = node_detail -> next;
		    }
		}
	      node_rapprochements = node_rapprochements -> next;
	    }
	}

      /* on recup�re ici les etats */

      if ( !strcmp ( node_1 -> name,
		     "Etats" ))
	{
	  xmlNodePtr node_etats;

	  /* node_etats va faire le tour de l'arborescence des etats */

	  node_etats = node_1 -> childs;

	  while ( node_etats )
	    {
	      /* on va recuperer ici les generalites des etats */
	      
	      if ( !strcmp ( node_etats -> name,
			     "Generalites" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_etats -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "No_dernier_etat" ))
			no_dernier_etat = atoi ( xmlNodeGetContent ( node_generalites ));

			node_generalites = node_generalites -> next;
		    }
		}

	 
	      /* on va recuperer ici les etats */
	    
	      if ( !strcmp ( node_etats -> name,
			     "Detail_des_etats" ))
		{
		  xmlNodePtr node_detail;

		  liste_struct_etats = NULL;
		  node_detail = node_etats -> childs;
		
		  while ( node_detail )
		    {
		      struct struct_etat *etat;
		      
		      etat = calloc ( 1,
				      sizeof ( struct struct_etat ));

		      etat -> no_etat = atoi ( xmlGetProp ( node_detail,
							    "No" ));
		      etat -> nom_etat = xmlGetProp ( node_detail,
						      "Nom" );

		      etat -> type_classement = NULL;

		      if ( strlen ( xmlGetProp ( node_detail,
						 "Type_classement" )))
			{
			  gchar **pointeur_char;
			  gint i;

			  pointeur_char = g_strsplit ( xmlGetProp ( node_detail,
								    "Type_classement" ),
						       "/",
						       0 );
			  i=0;

			  while ( pointeur_char[i] )
			    {
			      etat -> type_classement = g_list_append ( etat -> type_classement,
									GINT_TO_POINTER ( atoi ( pointeur_char[i] )));
			      i++;
			    }
			  g_strfreev ( pointeur_char );
			}

		      if (xmlGetProp ( node_detail, "Aff_r" ))
			{
			  etat -> afficher_r = atoi ( xmlGetProp ( node_detail,
								   "Aff_r" ));
			}
		      else 
			{
			  etat -> afficher_r = 0;
			}

		      etat -> afficher_opes = atoi ( xmlGetProp ( node_detail,
								  "Aff_ope" ));

		      etat -> afficher_no_ope = atoi ( xmlGetProp ( node_detail,
								    "Aff_no_ope" ));

		      etat -> afficher_date_ope = atoi ( xmlGetProp ( node_detail,
								      "Aff_date_ope" ));

		      etat -> afficher_tiers_ope = atoi ( xmlGetProp ( node_detail,
								       "Aff_tiers_ope" ));

		      etat -> afficher_categ_ope = atoi ( xmlGetProp ( node_detail,
								       "Aff_categ_ope" ));

		      etat -> afficher_sous_categ_ope = atoi ( xmlGetProp ( node_detail,
									    "Aff_ss_categ_ope" ));

		      etat -> afficher_type_ope = atoi ( xmlGetProp ( node_detail,
								      "Aff_type_ope" ));

		      etat -> afficher_ib_ope = atoi ( xmlGetProp ( node_detail,
								    "Aff_ib_ope" ));

		      etat -> afficher_sous_ib_ope = atoi ( xmlGetProp ( node_detail,
									 "Aff_ss_ib_ope" ));

		      etat -> afficher_cheque_ope = atoi ( xmlGetProp ( node_detail,
									"Aff_cheque_ope" ));

		      etat -> afficher_notes_ope = atoi ( xmlGetProp ( node_detail,
								       "Aff_notes_ope" ));

		      etat -> afficher_pc_ope = atoi ( xmlGetProp ( node_detail,
								    "Aff_pc_ope" ));

		      etat -> afficher_rappr_ope = atoi ( xmlGetProp ( node_detail,
								       "Aff_rappr_ope" ));

		      etat -> afficher_infobd_ope = atoi ( xmlGetProp ( node_detail,
									"Aff_infobd_ope" ));

		      etat -> afficher_exo_ope = atoi ( xmlGetProp ( node_detail,
									"Aff_exo_ope" ));

		      etat -> afficher_titre_colonnes = atoi ( xmlGetProp ( node_detail,
									"Aff_titres_col" ));

		      etat -> type_affichage_titres = atoi ( xmlGetProp ( node_detail,
									"Aff_titres_chgt" ));

		      etat -> pas_detailler_ventilation = atoi ( xmlGetProp ( node_detail,
									"Pas_detail_ventil" ));

		      etat -> devise_de_calcul_general = atoi ( xmlGetProp ( node_detail,
									"Devise_gen" ));

		      etat -> exo_date = atoi ( xmlGetProp ( node_detail,
							     "Exo_date" ));
							  			  
		      etat -> utilise_detail_exo = atoi ( xmlGetProp ( node_detail,
								       "Detail_exo" ));
							  			  
		      etat -> no_exercices = NULL;

		      if ( strlen ( xmlGetProp ( node_detail,
						 "No_exo" )))
			{
			  gchar **pointeur_char;
			  gint i;

			  pointeur_char = g_strsplit ( xmlGetProp ( node_detail,
								    "No_exo" ),
						       "/",
						       0 );
			  i=0;

			  while ( pointeur_char[i] )
			    {
			      etat -> no_exercices = g_slist_append ( etat -> no_exercices,
								      GINT_TO_POINTER ( atoi ( pointeur_char[i] )));
			      i++;
			    }
			  g_strfreev ( pointeur_char );
			}
			
							 
		      etat -> no_plage_date = atoi ( xmlGetProp ( node_detail,
								  "Plage_date" ));

		      if ( strlen ( xmlGetProp ( node_detail,
						 "Date_debut" )))
			{
			  gchar **pointeur_char;

			  pointeur_char = g_strsplit ( xmlGetProp ( node_detail,
								    "Date_debut" ),
						       "/",
						       3 );

			  etat -> date_perso_debut = g_date_new_dmy ( atoi ( pointeur_char[0] ),
								atoi ( pointeur_char[1] ),
								atoi ( pointeur_char[2] ));
			  g_strfreev ( pointeur_char );
			}
		      else
			etat -> date_perso_debut = NULL;

		      if ( strlen ( xmlGetProp ( node_detail,
						 "Date_fin" )))
			{
			  gchar **pointeur_char;

			  pointeur_char = g_strsplit ( xmlGetProp ( node_detail,
								    "Date_fin" ),
						       "/",
						       3 );

			  etat -> date_perso_fin = g_date_new_dmy ( atoi ( pointeur_char[0] ),
							      atoi ( pointeur_char[1] ),
							      atoi ( pointeur_char[2] ));
			  g_strfreev ( pointeur_char );
			}
		      else
			etat -> date_perso_fin = NULL;

		      etat -> separation_par_plage = atoi ( xmlGetProp ( node_detail,
									 "Utilise_plages" ));
		      etat -> type_separation_plage = atoi ( xmlGetProp ( node_detail,
									  "Sep_plages" ));
		      etat -> jour_debut_semaine = atoi ( xmlGetProp ( node_detail,
								       "Deb_sem_plages" ));


		      etat -> utilise_detail_comptes = atoi ( xmlGetProp ( node_detail,
									   "Detail_comptes" ));

		      etat -> no_comptes = NULL;

		      if ( strlen ( xmlGetProp ( node_detail,
						 "No_comptes" )))
			{
			  gchar **pointeur_char;
			  gint i;

			  pointeur_char = g_strsplit ( xmlGetProp ( node_detail,
								    "No_comptes" ),
						       "/",
						       0 );
			  i=0;

			  while ( pointeur_char[i] )
			    {
			      etat ->no_comptes  = g_slist_append ( etat -> no_comptes,
								    GINT_TO_POINTER ( atoi ( pointeur_char[i] )));
			      i++;
			    }
			  g_strfreev ( pointeur_char );
			}


		      etat -> regroupe_ope_par_compte = atoi ( xmlGetProp ( node_detail,
									    "Grp_ope_compte" ));

		      etat -> affiche_sous_total_compte = atoi ( xmlGetProp ( node_detail,
									    "Total_compte" ));

		      etat -> afficher_nom_compte = atoi ( xmlGetProp ( node_detail,
									"Aff_nom_compte" ));

		      etat -> type_virement = atoi ( xmlGetProp ( node_detail,
						     "Type_vir" ));

		      etat -> no_comptes_virements = NULL;

		      if (xmlGetProp ( node_detail, "No_comptes_virements" ) &&
			  strlen ( xmlGetProp ( node_detail,
						"No_comptes_virements" )))
			{
			  gchar **pointeur_char;
			  gint i;

			  pointeur_char = g_strsplit ( xmlGetProp ( node_detail,
								    "No_comptes_virements" ),
						       "/",
						       0 );
			  i=0;

			  while ( pointeur_char[i] )
			    {
			      etat ->no_comptes_virements  = g_slist_append ( etat -> no_comptes_virements,
									      GINT_TO_POINTER ( atoi ( pointeur_char[i] )));
			      i++;
			    }
			  g_strfreev ( pointeur_char );
			}


		      if (xmlGetProp ( node_detail, "Exclure_non_vir" ))
			{
			  etat -> exclure_ope_non_virement = atoi ( xmlGetProp ( node_detail,
										 "Exclure_non_vir" ));
			}
		      else
			{
			  etat -> exclure_ope_non_virement = 0;
			}
		      

		      if (xmlGetProp ( node_detail, "Categ" ))
			{
			  etat -> utilise_categ = atoi ( xmlGetProp ( node_detail,
								      "Categ" ));
			}
		      else
			{
			  etat -> utilise_categ = 0;
			}

		      if (xmlGetProp ( node_detail, "Detail_categ" ))
			{
			  etat -> utilise_detail_categ = atoi ( xmlGetProp ( node_detail,
									     "Detail_categ" ));
			}
		      else
			{
			  etat -> utilise_detail_categ = 0;
			}
		      
		      etat -> no_categ = NULL;

		      if ( strlen ( xmlGetProp ( node_detail,
						 "No_categ" )))
			{
			  gchar **pointeur_char;
			  gint i;

			  pointeur_char = g_strsplit ( xmlGetProp ( node_detail,
								    "No_categ" ),
						       "/",
						       0 );
			  i=0;

			  while ( pointeur_char[i] )
			    {
			      etat -> no_categ = g_slist_append ( etat -> no_categ,
								  GINT_TO_POINTER ( atoi ( pointeur_char[i] )));
			      i++;
			    }
			  g_strfreev ( pointeur_char );
			}

		      etat -> exclure_ope_sans_categ = atoi ( xmlGetProp ( node_detail,
									"Exclut_categ" ));

		      etat -> affiche_sous_total_categ = atoi ( xmlGetProp ( node_detail,
									"Total_categ" ));

		      etat -> afficher_sous_categ = atoi ( xmlGetProp ( node_detail,
									"Aff_ss_categ" ));

		      etat -> afficher_pas_de_sous_categ = atoi ( xmlGetProp ( node_detail,
									"Aff_pas_ss_categ" ));

		      etat -> affiche_sous_total_sous_categ = atoi ( xmlGetProp ( node_detail,
									"Total_ss_categ" ));

		      etat -> devise_de_calcul_categ = atoi ( xmlGetProp ( node_detail,
									"Devise_categ" ));

		      etat -> afficher_nom_categ = atoi ( xmlGetProp ( node_detail,
									"Aff_nom_categ" ));

		      etat -> utilise_ib = atoi ( xmlGetProp ( node_detail,
								  "IB" ));
		      etat -> utilise_detail_ib = atoi ( xmlGetProp ( node_detail,
								  "Detail_ib" ));
		      etat -> no_ib = NULL;

		      if ( strlen ( xmlGetProp ( node_detail,
						 "No_ib" )))
			{
			  gchar **pointeur_char;
			  gint i;

			  pointeur_char = g_strsplit ( xmlGetProp ( node_detail,
								    "No_ib" ),
						       "/",
						       0 );
			  i=0;

			  while ( pointeur_char[i] )
			    {
			      etat -> no_ib = g_slist_append ( etat -> no_ib,
								      GINT_TO_POINTER ( atoi ( pointeur_char[i] )));
			      i++;
			    }
			  g_strfreev ( pointeur_char );
			}


		      etat -> exclure_ope_sans_ib = atoi ( xmlGetProp ( node_detail,
									"Exclut_ib" ));

		      etat -> affiche_sous_total_ib = atoi ( xmlGetProp ( node_detail,
									"Total_ib" ));

		      etat -> afficher_sous_ib = atoi ( xmlGetProp ( node_detail,
									"Aff_ss_ib" ));

		      etat -> afficher_pas_de_sous_ib = atoi ( xmlGetProp ( node_detail,
									"Aff_pas_ss_ib" ));

		      etat -> affiche_sous_total_sous_ib = atoi ( xmlGetProp ( node_detail,
									"Total_ss_ib" ));

		      etat -> devise_de_calcul_ib = atoi ( xmlGetProp ( node_detail,
									"Devise_ib" ));

		      etat -> afficher_nom_ib = atoi ( xmlGetProp ( node_detail,
								    "Aff_nom_ib" ));

		      etat -> utilise_tiers = atoi ( xmlGetProp ( node_detail,
								  "Tiers" ));
		      etat -> utilise_detail_tiers = atoi ( xmlGetProp ( node_detail,
								  "Detail_tiers" ));

		      etat -> no_tiers = NULL;

		      if ( strlen ( xmlGetProp ( node_detail,
						 "No_tiers" )))
			{
			  gchar **pointeur_char;
			  gint i;

			  pointeur_char = g_strsplit ( xmlGetProp ( node_detail,
								    "No_tiers" ),
						       "/",
						       0 );
			  i=0;

			  while ( pointeur_char[i] )
			    {
			      etat -> no_tiers = g_slist_append ( etat -> no_tiers,
								      GINT_TO_POINTER ( atoi ( pointeur_char[i] )));
			      i++;
			    }
			  g_strfreev ( pointeur_char );
			}

		      etat -> affiche_sous_total_tiers = atoi ( xmlGetProp ( node_detail,
								  "Total_tiers" ));

		      etat -> devise_de_calcul_tiers = atoi ( xmlGetProp ( node_detail,
									"Devise_tiers" ));

		      etat -> afficher_nom_tiers = atoi ( xmlGetProp ( node_detail,
								       "Aff_nom_tiers" ));

		      if ( strlen ( xmlGetProp ( node_detail,
						 "Texte" )))
			etat -> texte = xmlGetProp ( node_detail,
						     "Texte" );
		      else
			etat -> texte = NULL;



		      if (xmlGetProp ( node_detail, "Montant_n_p" ))
			{
			  etat -> utilise_montant_neg_pos = atoi ( xmlGetProp ( node_detail,
										"Montant_n_p" ));
			}
		      else
			{
			  etat -> utilise_montant_neg_pos = 0;
			}
		      
		      if (xmlGetProp ( node_detail, "Type_n_p" ))
			{
			  etat -> type_neg_pos = atoi ( xmlGetProp ( node_detail,
								     "Type_n_p" ));
			}
		      else
			{
			  etat -> type_neg_pos = 0;
			}
		      
		      if (xmlGetProp ( node_detail, "Util_val" ))
			{
			  etat -> utilise_valeur = atoi ( xmlGetProp ( node_detail,
								       "Util_val" ));
			}
		      else
			{
			  etat -> utilise_valeur = 0;
			}
		      
		      if (xmlGetProp ( node_detail, "Type_ope_val" ))
			{
			  etat -> type_operateur_valeur = atoi ( xmlGetProp ( node_detail,
									      "Type_ope_val" ));
			}
		      else
			{
			  etat -> type_operateur_valeur = 0;
			}

		      if ( xmlGetProp ( node_detail, "Montant_val" ) )
			{
			  etat -> montant_valeur = g_strtod ( xmlGetProp ( node_detail,
									   "Montant_val" ), 
							      NULL );
			}
		      else
			{
			  etat -> montant_valeur = 0;
			}
		      
		      if (xmlGetProp ( node_detail, "Util_incl" ))
			{
			  etat -> utilise_inclusion = atoi ( xmlGetProp ( node_detail,
									  "Util_incl" ));
			}
		      else
			{
			  etat -> utilise_inclusion = 0;
			}
		      
		      if ( xmlGetProp ( node_detail, "Montant_incl_inf" ) )
			{
			  etat -> montant_inclusion_inf = g_strtod ( xmlGetProp ( node_detail,
										  "Montant_incl_inf" ), 
								     NULL );
			}
		      else
			{
			  etat -> montant_inclusion_inf = 0;
			}
		      
		      if ( xmlGetProp ( node_detail, "Type_ope_incl_inf" ))
			{
			  etat -> type_operateur_inf_inclusion = atoi ( xmlGetProp ( node_detail,
										     "Type_ope_incl_inf" ));
			}
		      else
			{
			  etat -> type_operateur_inf_inclusion = 0;
			}
		      
		      if ( xmlGetProp ( node_detail, "Type_ope_incl_sup" ))
			{
			  etat -> type_operateur_sup_inclusion = atoi ( xmlGetProp ( node_detail,
										     "Type_ope_incl_sup" ));
			}
		      else
			{
			  etat -> type_operateur_sup_inclusion = 0;
			}
		      
		      if ( xmlGetProp ( node_detail, "Montant_incl_sup" ))
			{
			  etat -> montant_inclusion_sup = g_strtod ( xmlGetProp ( node_detail,
										  "Montant_incl_sup" ),
								     NULL );
			}
		      else
			{
			  etat -> montant_inclusion_sup = 0;
			}
		      
		      if (xmlGetProp ( node_detail, "Montant_nul" ))
			{
			  etat -> choix_montant_nul = atoi ( xmlGetProp ( node_detail,
									  "Montant_nul" ));
			}
		      else
			{
			  etat -> choix_montant_nul = 0;
			}
		      
		      if (xmlGetProp ( node_detail, "Montant_devise" ))
			{
			  etat -> choix_devise_montant = atoi ( xmlGetProp ( node_detail,
									     "Montant_devise" ));
			}
		      else
			{
			  etat -> choix_devise_montant = 1;
			}
		      
		      liste_struct_etats = g_slist_append ( liste_struct_etats,
							    etat );

		      node_detail = node_detail -> next;
		    }
		}
	      node_etats = node_etats -> next;
	    }
	}
      node_1 = node_1 -> next;
    }


  /* on lib�re la memoire */

  xmlFreeDoc ( doc );

  /* on marque le fichier comme ouvert */

  fichier_marque_ouvert ( TRUE );

  modification_fichier ( FALSE );

  return ( TRUE );
}
/***********************************************************************************************************/




/***********************************************************************************************************/
/* Procedure qui sauvegarde le fichier */
/* retourne TRUE : no problem */
/* retourne FALSE : pas de nom => gtkfileselection a pris la main ou erreur */
/***********************************************************************************************************/

gboolean enregistre_fichier ( void )
{
  xmlDocPtr doc;
  xmlNodePtr node;
  xmlNodePtr node_1;
  gint resultat;
  gchar *pointeur_char;
  GSList *pointeur_liste;
  GList *pointeur_list;
  gint i, j;
  GSList *pointeur_liste_2;

  /*   si le fichier est dej� ouvert par un autre, ne peut enregistrer */

  if ( etat.fichier_deja_ouvert && !etat.force_enregistrement )
    {
      dialogue ( _(_(" Fichier ouvert par un autre utilisateur, enregistrement impossible\nVous pouvez forcer l'enregistrement � partir des param�tres.")) );
      return ( FALSE );
    }


  /* on met � jour les soldes des comptes */

  for ( i=0 ; i<nb_comptes ; i++ )
    mise_a_jour_solde ( i );


  /* creation de l'arbre xml en memoire */

  doc = xmlNewDoc("1.0");

  /* la racine est grisbi */

  doc->root = xmlNewDocNode ( doc,
			      NULL,
			      "Grisbi",
			      NULL );

  /* on commence � ajouter les generalites */

  node = xmlNewChild ( doc->root,
		       NULL,
		       "Generalites",
		       NULL );
  xmlNewTextChild ( node,
		    NULL,
		    "Version_fichier",
		    VERSION_FICHIER );

  xmlNewTextChild ( node,
		    NULL,
		    "Version_grisbi",
		    VERSION );

  /* on met fichier_ouvert � 0 car si c'est une backup ... */
  /* qd c'est un enregistrement normal, la mise � 1 se fait plus tard */

  xmlNewTextChild ( node,
		    NULL,
		    "Fichier_ouvert",
		    "0" );

  etat.fichier_deja_ouvert = 0;

  xmlNewTextChild ( node,
		    NULL,
		    "Backup",
		    nom_fichier_backup );
  xmlNewTextChild ( node,
		    NULL,
		    "Titre",
		    titre_fichier);
  xmlNewTextChild ( node,
		    NULL,
		    "Adresse_commune",
		    adresse_commune);
  xmlNewTextChild ( node,
		    NULL,
		    "Utilise_exercices",
		    itoa (etat.utilise_exercice));
  xmlNewTextChild ( node,
		    NULL,
		    "Utilise_IB",
		    itoa (etat.utilise_imputation_budgetaire));
  xmlNewTextChild ( node,
		    NULL,
		    "Utilise_PC",
		    itoa (etat.utilise_piece_comptable));
  xmlNewTextChild ( node,
		    NULL,
		    "Utilise_info_BG",
		    itoa (etat.utilise_info_banque_guichet));
  xmlNewTextChild ( node,
		    NULL,
		    "Numero_devise_totaux_tiers",
		    itoa ( no_devise_totaux_tiers));
  xmlNewTextChild ( node,
		    NULL,
		    "Type_affichage_des_echeances",
		    itoa (affichage_echeances));
  xmlNewTextChild ( node,
		    NULL,
		    "Affichage_echeances_perso_nb_libre",
		    itoa (affichage_echeances_perso_nb_libre));
  xmlNewTextChild ( node,
		    NULL,
		    "Type_affichage_perso_echeances",
		    itoa (affichage_echeances_perso_j_m_a));
  xmlNewTextChild ( node,
		    NULL,
		    "Numero_derniere_operation",
		    itoa ( no_derniere_operation));

  xmlNewTextChild ( node,
		    NULL,
		    "Chemin_logo",
		    chemin_logo );

  /* creation de l'ordre de l'affichage des op�s */

  pointeur_char = NULL;

  for ( i=0 ; i<4 ; i++ )
    for ( j=0 ; j< 7 ; j++ )
       if ( pointeur_char )
	 pointeur_char = g_strconcat ( pointeur_char,
				       "-",
				       itoa ( tab_affichage_ope[i][j] ),
				       NULL );
       else
	 pointeur_char = itoa ( tab_affichage_ope[i][j] );
   

  xmlNewTextChild ( node,
		    NULL,
		    "Affichage_opes",
		    pointeur_char );

  /* creation des rapport largeurs colonnes */

  pointeur_char = NULL;

  for ( i=0 ; i<7 ; i++ )
    if ( pointeur_char )
      pointeur_char = g_strconcat ( pointeur_char,
				    "-",
				    itoa ( rapport_largeur_colonnes[i] ),
				    NULL );
    else
      pointeur_char = itoa ( rapport_largeur_colonnes[i] );
   

  xmlNewTextChild ( node,
		    NULL,
		    "Rapport_largeur_col",
		    pointeur_char );


  /*   on commence la sauvegarde des comptes : 2 parties, les generalites */
  /* puis les comptes 1 par 1 */

  node = xmlNewChild ( doc->root,
		       NULL,
		       "Comptes",
		       NULL );

  /* c'est node_1 qui va contenir les generalites puis les comptes */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "Generalites",
			 NULL );

  /* creation de l'ordre des comptes */

  pointeur_liste = ordre_comptes;
  pointeur_char = NULL;

  do
  {
    if ( pointeur_char )
      pointeur_char = g_strconcat ( pointeur_char,
				    "-",
				    itoa ( GPOINTER_TO_INT ( pointeur_liste -> data )),
				    NULL );
    else
      pointeur_char = itoa ( GPOINTER_TO_INT ( pointeur_liste -> data ));

    pointeur_liste = pointeur_liste -> next;
  }
  while ( pointeur_liste );

  xmlNewTextChild ( node_1,
		    NULL,
		    "Ordre_des_comptes",
		    pointeur_char );

  xmlNewTextChild ( node_1,
		    NULL,
		    "Compte_courant",
		    itoa ( compte_courant ) );


  /* mise en place des comptes 1 par 1 */

  for ( i=0 ; i < nb_comptes ; i++)
    {
      xmlNodePtr node_compte;
   
      p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;

      node_1 = xmlNewChild ( node,
			     NULL,
			     "Compte",
			     NULL );

      /* on met d'abord le detail du compte, les types puis les operations */

      /* mise en forme de la date du dernier releve */

      if ( DATE_DERNIER_RELEVE )
	pointeur_char = g_strconcat ( itoa ( g_date_day ( DATE_DERNIER_RELEVE ) ),
				      "/",
				      itoa ( g_date_month ( DATE_DERNIER_RELEVE ) ),
				      "/",
				      itoa ( g_date_year ( DATE_DERNIER_RELEVE ) ),
				      NULL );
      else
	pointeur_char = NULL;


      node_compte = xmlNewChild ( node_1,
				  NULL,
				  "Details",
				  NULL );

      xmlNewTextChild ( node_compte,
			NULL,
			"Nom",
			NOM_DU_COMPTE );

      xmlNewTextChild ( node_compte,
			NULL,
			"No_de_compte",
			itoa ( NO_COMPTE ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Titulaire",
			TITULAIRE );

      xmlNewTextChild ( node_compte,
			NULL,
			"Type_de_compte",
			itoa ( TYPE_DE_COMPTE ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Nb_operations",
			itoa ( NB_OPE_COMPTE ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Devise",
			itoa ( DEVISE ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Banque",
			itoa ( BANQUE ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Guichet",
			NO_GUICHET );

      xmlNewTextChild ( node_compte,
			NULL,
			"No_compte_banque",
			NO_COMPTE_BANQUE );

      xmlNewTextChild ( node_compte,
			NULL,
			"Cle_du_compte",
			CLE_COMPTE );

      xmlNewTextChild ( node_compte,
			NULL,
			"Solde_initial",
			g_strdup_printf ( "%4.2f",
					  SOLDE_INIT ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Solde_mini_voulu",
			g_strdup_printf ( "%4.2f",
					  SOLDE_MINI_VOULU ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Solde_mini_autorise",
			g_strdup_printf ( "%4.2f",
					  SOLDE_MINI ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Solde_courant",
			g_strdup_printf ( "%4.2f",
					  SOLDE_COURANT ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Date_dernier_releve",
			pointeur_char );

      xmlNewTextChild ( node_compte,
			NULL,
			"Solde_dernier_releve",
			g_strdup_printf ( "%4.2f",
					  SOLDE_DERNIER_RELEVE ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Dernier_no_de_rapprochement",
			itoa ( DERNIER_NO_RAPPROCHEMENT ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Compte_cloture",
			itoa ( COMPTE_CLOTURE ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Affichage_r",
			itoa ( AFFICHAGE_R ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Nb_lignes_ope",
			itoa ( NB_LIGNES_OPE ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Commentaires",
			COMMENTAIRE );

      xmlNewTextChild ( node_compte,
			NULL,
			"Adresse_du_titulaire",
			ADRESSE_TITULAIRE );

      xmlNewTextChild ( node_compte,
			NULL,
			"Nombre_de_types",
			itoa ( g_slist_length ( TYPES_OPES ) ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Type_defaut_debit",
			itoa ( TYPE_DEFAUT_DEBIT ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Type_defaut_credit",
			itoa ( TYPE_DEFAUT_CREDIT ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Tri_par_type",
			itoa ( TRI ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Neutres_inclus",
			itoa ( NEUTRES_INCLUS ));

      /* mise en place de l'ordre du tri */


      pointeur_liste = LISTE_TRI;
      pointeur_char = NULL;

      while ( pointeur_liste )
	{
	  if ( pointeur_char )
	    pointeur_char = g_strconcat ( pointeur_char,
					  "/",
					  itoa ( GPOINTER_TO_INT ( pointeur_liste -> data )),
					  NULL );
	  else
	    pointeur_char = itoa ( GPOINTER_TO_INT ( pointeur_liste -> data ));

	  pointeur_liste = pointeur_liste -> next;
	}

      xmlNewTextChild ( node_compte,
			NULL,
			"Ordre_du_tri",
			pointeur_char );

      /* mise en place des types */

      node_compte = xmlNewChild ( node_1,
				  NULL,
				  "Detail_de_Types",
				  NULL );

      pointeur_liste = TYPES_OPES;

      while ( pointeur_liste )
	{
	  struct struct_type_ope *type;
	  xmlNodePtr node_type;

	  node_type = xmlNewChild ( node_compte,
				    NULL,
				    "Type",
				    NULL );

	  type = pointeur_liste -> data;


	  xmlSetProp ( node_type,
		       "No",
		       itoa ( type -> no_type ));

	  xmlSetProp ( node_type,
		       "Nom",
		       type -> nom_type );

	  xmlSetProp ( node_type,
		       "Signe",
		       itoa ( type -> signe_type ));

	  xmlSetProp ( node_type,
		       "Affiche_entree",
		       itoa ( type -> affiche_entree ));

	  xmlSetProp ( node_type,
		       "Numerotation_auto",
		       itoa ( type -> numerotation_auto ));

	  xmlSetProp ( node_type,
		       "No_en_cours",
		       itoa ( type -> no_en_cours ));

	  pointeur_liste = pointeur_liste -> next;
	}


      /* mise en place des operations */

      node_compte = xmlNewChild ( node_1,
				  NULL,
				  "Detail_des_operations",
				  NULL );

      pointeur_liste = LISTE_OPERATIONS;

      while ( pointeur_liste )
	{
	  struct structure_operation *operation;
	  xmlNodePtr node_ope;

	  operation = pointeur_liste -> data;

	  node_ope = xmlNewChild ( node_compte,
				   NULL,
				   "Operation",
				   NULL );

	  xmlSetProp ( node_ope,
		       "No",
		       itoa ( operation -> no_operation ));

	  xmlSetProp ( node_ope,
		       "D",
		       g_strdup_printf ( "%d/%d/%d",
					 operation -> jour,
					 operation -> mois,
					 operation -> annee ));

    /* GDC : Ecriture de la date bancaire */
	  xmlSetProp ( node_ope,
		       "Db",
		       g_strdup_printf ( "%d/%d/%d",
					 operation -> jour_bancaire,
					 operation -> mois_bancaire,
					 operation -> annee_bancaire ));
    /* GDCFin */

	  xmlSetProp ( node_ope,
		       "M",
		       g_strdup_printf ( "%4.2f",
					 operation -> montant ));

	  xmlSetProp ( node_ope,
		       "De",
		       itoa ( operation -> devise ));

	  xmlSetProp ( node_ope,
		       "Rdc",
		       itoa ( operation -> une_devise_compte_egale_x_devise_ope ));

	  xmlSetProp ( node_ope,
		       "Tc",
		       g_strdup_printf ( "%4.2f",
					 operation -> taux_change ));

	  xmlSetProp ( node_ope,
		       "Fc",
		       g_strdup_printf ( "%4.2f",
					 operation -> frais_change ));

	  xmlSetProp ( node_ope,
		       "T",
		       itoa ( operation -> tiers ));

	  xmlSetProp ( node_ope,
		       "C",
		       itoa ( operation -> categorie ));

	  xmlSetProp ( node_ope,
		       "Sc",
		       itoa ( operation -> sous_categorie ));

	  xmlSetProp ( node_ope,
		       "Ov",
		       itoa ( operation -> operation_ventilee ));

	  xmlSetProp ( node_ope,
		       "N",
		       operation -> notes );

	  xmlSetProp ( node_ope,
		       "Ty",
		       itoa ( operation -> type_ope ));

	  xmlSetProp ( node_ope,
		       "Ct",
		       operation -> contenu_type );

	  xmlSetProp ( node_ope,
		       "P",
		       itoa ( operation -> pointe ));

	  xmlSetProp ( node_ope,
		       "A",
		       itoa ( operation -> auto_man ));

	  xmlSetProp ( node_ope,
		       "R",
		       itoa ( operation -> no_rapprochement ));

	  xmlSetProp ( node_ope,
		       "E",
		       itoa ( operation -> no_exercice ));

	  xmlSetProp ( node_ope,
		       "I",
		       itoa ( operation -> imputation ));

	  xmlSetProp ( node_ope,
		       "Si",
		       itoa ( operation -> sous_imputation ));

	  xmlSetProp ( node_ope,
		       "Pc",
		       operation -> no_piece_comptable );

	  xmlSetProp ( node_ope,
		       "Ibg",
		       operation -> info_banque_guichet );

	  xmlSetProp ( node_ope,
		       "Ro",
		       itoa ( operation -> relation_no_operation ));

	  xmlSetProp ( node_ope,
		       "Rc",
		       itoa ( operation -> relation_no_compte ));

	  xmlSetProp ( node_ope,
		       "Va",
		       itoa ( operation -> no_operation_ventilee_associee ));

	  pointeur_liste = pointeur_liste -> next;
	}
    }


  /* on sauvegarde maintenant les echeances : generalites puis liste des echeances */


  node = xmlNewChild ( doc->root,
		       NULL,
		       "Echeances",
		       NULL );

  /* c'est node_1 qui va contenir les generalites */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "Generalites",
			 NULL );

  xmlNewTextChild ( node_1,
		    NULL,
		    "Nb_echeances",
		    itoa ( nb_echeances ));

  xmlNewTextChild ( node_1,
		    NULL,
		    "No_derniere_echeance",
		    itoa ( no_derniere_echeance ));

  /* on met les echeances */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "Detail_des_echeances",
			 NULL );

  pointeur_liste = gsliste_echeances;

  while ( pointeur_liste )
    {
      struct operation_echeance *echeance;
      xmlNodePtr node_echeance;

      echeance = pointeur_liste -> data;


      node_echeance = xmlNewChild ( node_1,
				    NULL,
				    "Echeance",
				    NULL );

      xmlSetProp ( node_echeance,
		   "No",
		   itoa ( echeance -> no_operation ));

      xmlSetProp ( node_echeance,
		   "Date",
		   g_strdup_printf ( "%d/%d/%d",
				     echeance -> jour,
				     echeance -> mois,
				     echeance -> annee ));

      xmlSetProp ( node_echeance,
		   "Compte",
		   itoa ( echeance -> compte ));

      xmlSetProp ( node_echeance,
		   "Montant",
		   g_strdup_printf ( "%4.2f",
				     echeance -> montant ));

      xmlSetProp ( node_echeance,
		   "Devise",
		   itoa ( echeance -> devise ));

      xmlSetProp ( node_echeance,
		   "Tiers",
		   itoa ( echeance -> tiers ));

      xmlSetProp ( node_echeance,
		   "Categorie",
		   itoa ( echeance -> categorie ));

      xmlSetProp ( node_echeance,
		   "Sous-categorie",
		   itoa ( echeance -> sous_categorie ));

      xmlSetProp ( node_echeance,
		   "Virement_compte",
		   itoa ( echeance -> compte_virement ));

      xmlSetProp ( node_echeance,
		   "Type",
		   itoa ( echeance -> type_ope ));

      xmlSetProp ( node_echeance,
		   "Contenu_du_type",
		   echeance -> contenu_type );

      xmlSetProp ( node_echeance,
		   "Exercice",
		   itoa ( echeance -> no_exercice ));

      xmlSetProp ( node_echeance,
		   "Imputation",
		   itoa ( echeance -> imputation ));

      xmlSetProp ( node_echeance,
		   "Sous-imputation",
		   itoa ( echeance -> sous_imputation ));

      xmlSetProp ( node_echeance,
		   "Notes",
		   echeance -> notes );

      xmlSetProp ( node_echeance,
		   "Automatique",
		   itoa ( echeance -> auto_man ));

      xmlSetProp ( node_echeance,
		   "Periodicite",
		   itoa ( echeance -> periodicite ));

      xmlSetProp ( node_echeance,
		   "Intervalle_periodicite",
		   itoa ( echeance -> intervalle_periodicite_personnalisee ));

      xmlSetProp ( node_echeance,
		   "Periodicite_personnalisee",
		   itoa ( echeance -> periodicite_personnalisee ));

      if ( echeance -> jour_limite )
	xmlSetProp ( node_echeance,
		     "Date_limite",
		     g_strdup_printf ( "%d/%d/%d",
				       echeance -> jour_limite,
				       echeance -> mois_limite,
				       echeance -> annee_limite ));
      else
	xmlSetProp ( node_echeance,
		     "Date_limite",
		     NULL );

      pointeur_liste = pointeur_liste -> next;
    }


  /* mise en place des tiers : generalites puis liste */


  node = xmlNewChild ( doc->root,
		       NULL,
		       "Tiers",
		       NULL );

  /* c'est node_1 qui va contenir les generalites */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "Generalites",
			 NULL );

  xmlNewTextChild ( node_1,
		    NULL,
		    "Nb_tiers",
		    itoa ( nb_enregistrements_tiers ));

  xmlNewTextChild ( node_1,
		    NULL,
		    "No_dernier_tiers",
		    itoa ( no_dernier_tiers ));

  /* on met les tiers */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "Detail_des_tiers",
			 NULL );

  pointeur_liste = liste_struct_tiers;

  while ( pointeur_liste )
    {
      struct struct_tiers *tiers;
      xmlNodePtr node_tiers;

      tiers = pointeur_liste -> data;


      node_tiers = xmlNewChild ( node_1,
				 NULL,
				 "Tiers",
				 NULL );

      xmlSetProp ( node_tiers,
		   "No",
		   itoa ( tiers -> no_tiers ));

      xmlSetProp ( node_tiers,
		   "Nom",
		   tiers -> nom_tiers );

      xmlSetProp ( node_tiers,
		   "Informations",
		   tiers -> texte );

      xmlSetProp ( node_tiers,
		   "Liaison",
		   itoa ( tiers -> liaison ));

      pointeur_liste = pointeur_liste -> next;
    }


  /* mise en place des categories : generalites puis liste */


  node = xmlNewChild ( doc->root,
		       NULL,
		       "Categories",
		       NULL );

  /* c'est node_1 qui va contenir les generalites */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "Generalites",
			 NULL );

  xmlNewTextChild ( node_1,
		    NULL,
		    "Nb_categories",
		    itoa ( nb_enregistrements_categories ));

  xmlNewTextChild ( node_1,
		    NULL,
		    "No_derniere_categorie",
		    itoa ( no_derniere_categorie ));

  /* on met les categories */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "Detail_des_categories",
			 NULL );

  pointeur_liste = liste_struct_categories;

  while ( pointeur_liste )
    {
      struct struct_categ *categ;
      xmlNodePtr node_categ;
      GSList *pointeur_sous_categ;

      categ = pointeur_liste -> data;


      node_categ = xmlNewChild ( node_1,
				 NULL,
				 "Categorie",
				 NULL );

      xmlSetProp ( node_categ,
		   "No",
		   itoa ( categ -> no_categ ));

      xmlSetProp ( node_categ,
		   "Nom",
		   categ -> nom_categ );

      xmlSetProp ( node_categ,
		   "Type",
		   itoa ( categ -> type_categ ));

      xmlSetProp ( node_categ,
		   "No_derniere_sous_cagegorie",
		   itoa ( categ -> no_derniere_sous_categ ));


      /* on ajoute les sous categories */

      pointeur_sous_categ = categ -> liste_sous_categ;

      while ( pointeur_sous_categ )
	{
	  struct struct_sous_categ *sous_categ;
	  xmlNodePtr node_sous_categ;

	  sous_categ = pointeur_sous_categ -> data;

	  node_sous_categ = xmlNewChild ( node_categ,
					  NULL,
					  "Sous-categorie",
					  NULL );

	  xmlSetProp ( node_sous_categ,
		       "No",
		       itoa ( sous_categ -> no_sous_categ ));

	  xmlSetProp ( node_sous_categ,
		       "Nom",
		       sous_categ -> nom_sous_categ );

	  pointeur_sous_categ = pointeur_sous_categ -> next;
	}
      pointeur_liste = pointeur_liste -> next;
    }


  /* mise en place des imputations budgetaires : generalites puis liste */


  node = xmlNewChild ( doc->root,
		       NULL,
		       "Imputations",
		       NULL );

  /* c'est node_1 qui va contenir les generalites */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "Generalites",
			 NULL );

  xmlNewTextChild ( node_1,
		    NULL,
		    "Nb_imputations",
		    itoa ( nb_enregistrements_imputations ));

  xmlNewTextChild ( node_1,
		    NULL,
		    "No_derniere_imputation",
		    itoa ( no_derniere_imputation ));

  /* on met les categories */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "Detail_des_imputations",
			 NULL );

  pointeur_liste = liste_struct_imputation;

  while ( pointeur_liste )
    {
      struct struct_imputation *imputation;
      xmlNodePtr node_imputation;
      GSList *pointeur_sous_imputation;

      imputation = pointeur_liste -> data;


      node_imputation = xmlNewChild ( node_1,
				      NULL,
				      "Imputation",
				      NULL );

      xmlSetProp ( node_imputation,
		   "No",
		   itoa ( imputation -> no_imputation ));

      xmlSetProp ( node_imputation,
		   "Nom",
		   imputation -> nom_imputation );

      xmlSetProp ( node_imputation,
		   "Type",
		   itoa ( imputation -> type_imputation ));

      xmlSetProp ( node_imputation,
		   "No_derniere_sous_imputation",
		   itoa ( imputation -> no_derniere_sous_imputation ));

      /* on ajoute les sous ib */

      pointeur_sous_imputation = imputation -> liste_sous_imputation;

      while ( pointeur_sous_imputation )
	{
	  struct struct_sous_imputation *sous_imputation;
	  xmlNodePtr node_sous_imputation;

	  sous_imputation = pointeur_sous_imputation -> data;

	  node_sous_imputation = xmlNewChild ( node_imputation,
					  NULL,
					  "Sous-imputation",
					  NULL );

	  xmlSetProp ( node_sous_imputation,
		       "No",
		       itoa ( sous_imputation -> no_sous_imputation ));

	  xmlSetProp ( node_sous_imputation,
		       "Nom",
		       sous_imputation -> nom_sous_imputation );

	  pointeur_sous_imputation = pointeur_sous_imputation -> next;
	}
      pointeur_liste = pointeur_liste -> next;
    }


  /* mise en place des devises : generalites puis liste */


  node = xmlNewChild ( doc->root,
		       NULL,
		       "Devises",
		       NULL );

  /* c'est node_1 qui va contenir les generalites */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "Generalites",
			 NULL );

  xmlNewTextChild ( node_1,
		    NULL,
		    "Nb_devises",
		    itoa ( nb_devises ));

  xmlNewTextChild ( node_1,
		    NULL,
		    "No_derniere_devise",
		    itoa ( no_derniere_devise ));

  /* on met les devises */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "Detail_des_devises",
			 NULL );

  pointeur_liste = liste_struct_devises;

  while ( pointeur_liste )
    {
      struct struct_devise *devise;
      xmlNodePtr node_devise;

      devise = pointeur_liste -> data;


      node_devise = xmlNewChild ( node_1,
				  NULL,
				  "Devise",
				  NULL );

      xmlSetProp ( node_devise,
		   "No",
		   itoa ( devise -> no_devise ));

      xmlSetProp ( node_devise,
		   "Nom",
		   devise -> nom_devise );

      xmlSetProp ( node_devise,
		   "Code",
		   devise -> code_devise );

      xmlSetProp ( node_devise,
		   "Passage_euro",
		   itoa ( devise -> passage_euro ));

      if ( devise -> date_dernier_change )
	xmlSetProp ( node_devise,
		     "Date_dernier_change",
		     g_strdup_printf (  "%d/%d/%d",
					g_date_day ( devise -> date_dernier_change ),
					g_date_month ( devise -> date_dernier_change ),
					g_date_year ( devise -> date_dernier_change )));
      else
	xmlSetProp ( node_devise,
		     "Date_dernier_change",
		     NULL );

      xmlSetProp ( node_devise,
		   "Rapport_entre_devises",
		   itoa ( devise -> une_devise_1_egale_x_devise_2 ));

      xmlSetProp ( node_devise,
		   "Devise_en_rapport",
		   itoa ( devise -> no_devise_en_rapport ));

      xmlSetProp ( node_devise,
		   "Change",
		   g_strdup_printf ( "%f",
				     devise -> change ));

      pointeur_liste = pointeur_liste -> next;
    }


  /* mise en place des banques : generalites puis liste */


  node = xmlNewChild ( doc->root,
		       NULL,
		       "Banques",
		       NULL );

  /* c'est node_1 qui va contenir les generalites */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "Generalites",
			 NULL );

  xmlNewTextChild ( node_1,
		    NULL,
		    "Nb_banques",
		    itoa ( nb_banques ));

  xmlNewTextChild ( node_1,
		    NULL,
		    "No_derniere_banque",
		    itoa ( no_derniere_banque ));

  /* on met les banques */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "Detail_des_banques",
			 NULL );

  pointeur_liste = liste_struct_banques;

  while ( pointeur_liste )
    {
      struct struct_banque *banque;
      xmlNodePtr node_banque;

      banque = pointeur_liste -> data;


      node_banque = xmlNewChild ( node_1,
				  NULL,
				  "Banque",
				  NULL );

      xmlSetProp ( node_banque,
		   "No",
		   itoa ( banque -> no_banque ));

      xmlSetProp ( node_banque,
		   "Nom",
		   banque -> nom_banque );

      xmlSetProp ( node_banque,
		   "Code",
		   banque -> code_banque );

      xmlSetProp ( node_banque,
		   "Adresse",
		   banque -> adr_banque );

      xmlSetProp ( node_banque,
		   "Tel",
		   banque -> tel_banque );

      xmlSetProp ( node_banque,
		   "Mail",
		   banque -> email_banque );

      xmlSetProp ( node_banque,
		   "Web",
		   banque -> web_banque );

      xmlSetProp ( node_banque,
		   "Nom_correspondant",
		   banque -> nom_correspondant );

      xmlSetProp ( node_banque,
		   "Fax_correspondant",
		   banque -> fax_correspondant );

      xmlSetProp ( node_banque,
		   "Tel_correspondant",
		   banque -> tel_correspondant );

      xmlSetProp ( node_banque,
		   "Mail_correspondant",
		   banque -> email_correspondant );

      xmlSetProp ( node_banque,
		   "Remarques",
		   banque -> remarque_banque );

      pointeur_liste = pointeur_liste -> next;
    }

  /* mise en place des exercices : generalites puis liste */


  node = xmlNewChild ( doc->root,
		       NULL,
		       "Exercices",
		       NULL );

  /* c'est node_1 qui va contenir les generalites */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "Generalites",
			 NULL );

  xmlNewTextChild ( node_1,
		    NULL,
		    "Nb_exercices",
		    itoa ( nb_exercices ));

  xmlNewTextChild ( node_1,
		    NULL,
		    "No_dernier_exercice",
		    itoa ( no_derniere_exercice ));

  /* on met les exercices */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "Detail_des_exercices",
			 NULL );

  pointeur_liste = liste_struct_exercices;

  while ( pointeur_liste )
    {
      struct struct_exercice *exercice;
      xmlNodePtr node_exercice;

      exercice = pointeur_liste -> data;


      node_exercice = xmlNewChild ( node_1,
				  NULL,
				  "Exercice",
				  NULL );

      xmlSetProp ( node_exercice,
		   "No",
		   itoa ( exercice -> no_exercice ));

      xmlSetProp ( node_exercice,
		   "Nom",
		   exercice -> nom_exercice );

      if ( exercice->date_debut )
	xmlSetProp ( node_exercice,
		     "Date_debut",
		     g_strdup_printf ( "%d/%d/%d",
				       g_date_day ( exercice->date_debut ),
				       g_date_month ( exercice->date_debut ),
				       g_date_year ( exercice->date_debut )));
      else
	xmlSetProp ( node_exercice,
		     "Date_debut",
		     NULL );

      if ( exercice->date_fin )
	xmlSetProp ( node_exercice,
		     "Date_fin",
		     g_strdup_printf ( "%d/%d/%d",
				       g_date_day ( exercice->date_fin ),
				       g_date_month ( exercice->date_fin ),
				       g_date_year ( exercice->date_fin )));
      else
	xmlSetProp ( node_exercice,
		     "Date_fin",
		     NULL );

      xmlSetProp ( node_exercice,
		   "Affiche",
		   itoa ( exercice -> affiche_dans_formulaire ));

      pointeur_liste = pointeur_liste -> next;
    }


  /* mise en place des no de rapprochement : generalites puis liste */


  node = xmlNewChild ( doc->root,
		       NULL,
		       "Rapprochements",
		       NULL );

  /* c'est node_1 qui va contenir les generalites */

  /* on n'a pas de generalites, peut �tre un jour ... */

/*   node_1 = xmlNewChild ( node, */
/* 			 NULL, */
/* 			 "Generalites", */
/* 			 NULL ); */

/*   xmlNewTextChild ( node_1, */
/* 		    NULL, */
/* 		    "Nb_rapprochements", */
/* 		    itoa ( g_slist_length ( liste_no_rapprochements ))); */

  /* on met les rapprochements */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "Detail_des_rapprochements",
			 NULL );

  pointeur_liste = liste_no_rapprochements;

  while ( pointeur_liste )
    {
      struct struct_no_rapprochement *rapprochement;
      xmlNodePtr node_rapprochement;

      rapprochement = pointeur_liste -> data;


      node_rapprochement = xmlNewChild ( node_1,
					 NULL,
					 "Rapprochement",
					 NULL );

      xmlSetProp ( node_rapprochement,
		   "No",
		   itoa ( rapprochement -> no_rapprochement ));

      xmlSetProp ( node_rapprochement,
		   "Nom",
		   rapprochement -> nom_rapprochement );

      pointeur_liste = pointeur_liste -> next;
    }


  /* sauvegarde des etats */

  node = xmlNewChild ( doc->root,
		       NULL,
		       "Etats",
		       NULL );

  /* c'est node_1 qui va contenir les generalites */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "Generalites",
			 NULL );

  xmlNewTextChild ( node_1,
		    NULL,
		    "No_dernier_etat",
		    itoa ( no_dernier_etat ));

  /* on met les etats */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "Detail_des_etats",
			 NULL );

  pointeur_liste_2 = liste_struct_etats;

  while ( pointeur_liste_2 )
    {
      struct struct_etat *etat;
      xmlNodePtr node_etat;

      etat = pointeur_liste_2 -> data;


      node_etat = xmlNewChild ( node_1,
				NULL,
				"Etat",
				NULL );

      xmlSetProp ( node_etat,
		   "No",
		   itoa ( etat -> no_etat ));

      xmlSetProp ( node_etat,
		   "Nom",
		   etat -> nom_etat );

      pointeur_list = etat -> type_classement;
      pointeur_char = NULL;

      while ( pointeur_list )
	{
	  if ( pointeur_char )
	    pointeur_char = g_strconcat ( pointeur_char,
					  "/",
					  itoa ( GPOINTER_TO_INT ( pointeur_list -> data )),
					  NULL );
	  else
	    pointeur_char = itoa ( GPOINTER_TO_INT ( pointeur_list -> data ));

	  pointeur_list = pointeur_list -> next;
	}

      xmlSetProp ( node_etat,
		   "Type_classement",
		   pointeur_char );


      xmlSetProp ( node_etat,
		   "Aff_r",
		   itoa ( etat -> afficher_r ));

      xmlSetProp ( node_etat,
		   "Aff_ope",
		   itoa ( etat -> afficher_opes ));

      xmlSetProp ( node_etat,
		   "Aff_no_ope",
		   itoa ( etat -> afficher_no_ope ));

      xmlSetProp ( node_etat,
		   "Aff_date_ope",
		   itoa ( etat -> afficher_date_ope ));

      xmlSetProp ( node_etat,
		   "Aff_tiers_ope",
		   itoa ( etat -> afficher_tiers_ope ));

      xmlSetProp ( node_etat,
		   "Aff_categ_ope",
		   itoa ( etat -> afficher_categ_ope ));

      xmlSetProp ( node_etat,
		   "Aff_ss_categ_ope",
		   itoa ( etat -> afficher_sous_categ_ope ));

      xmlSetProp ( node_etat,
		   "Aff_type_ope",
		   itoa ( etat -> afficher_type_ope ));

      xmlSetProp ( node_etat,
		   "Aff_ib_ope",
		   itoa ( etat -> afficher_ib_ope ));

      xmlSetProp ( node_etat,
		   "Aff_ss_ib_ope",
		   itoa ( etat -> afficher_sous_ib_ope ));

      xmlSetProp ( node_etat,
		   "Aff_cheque_ope",
		   itoa ( etat -> afficher_cheque_ope ));

      xmlSetProp ( node_etat,
		   "Aff_notes_ope",
		   itoa ( etat -> afficher_notes_ope ));

      xmlSetProp ( node_etat,
		   "Aff_pc_ope",
		   itoa ( etat -> afficher_pc_ope ));

      xmlSetProp ( node_etat,
		   "Aff_rappr_ope",
		   itoa ( etat -> afficher_rappr_ope ));

      xmlSetProp ( node_etat,
		   "Aff_infobd_ope",
		   itoa ( etat -> afficher_infobd_ope ));

      xmlSetProp ( node_etat,
		   "Aff_exo_ope",
		   itoa ( etat -> afficher_exo_ope ));

      xmlSetProp ( node_etat,
		   "Aff_titres_col",
		   itoa ( etat -> afficher_titre_colonnes ));

      xmlSetProp ( node_etat,
		   "Aff_titres_chgt",
		   itoa ( etat -> type_affichage_titres ));

      xmlSetProp ( node_etat,
		   "Pas_detail_ventil",
		   itoa ( etat -> pas_detailler_ventilation ));

      xmlSetProp ( node_etat,
		   "Devise_gen",
		   itoa ( etat -> devise_de_calcul_general ));


      xmlSetProp ( node_etat,
		   "Exo_date",
		   itoa ( etat -> exo_date ));

      xmlSetProp ( node_etat,
		   "Detail_exo",
		   itoa ( etat -> utilise_detail_exo ));

      pointeur_liste = etat -> no_exercices;
      pointeur_char = NULL;

      while ( pointeur_liste )
	{
	  if ( pointeur_char )
	    pointeur_char = g_strconcat ( pointeur_char,
					  "/",
					  itoa ( GPOINTER_TO_INT ( pointeur_liste -> data )),
					  NULL );
	  else
	    pointeur_char = itoa ( GPOINTER_TO_INT ( pointeur_liste -> data ));

	  pointeur_liste = pointeur_liste -> next;
	}

      xmlSetProp ( node_etat,
		   "No_exo",
		   pointeur_char );

      xmlSetProp ( node_etat,
		   "Plage_date",
		   itoa ( etat -> no_plage_date ));


      if ( etat->date_perso_debut )
	xmlSetProp ( node_etat,
		     "Date_debut",
		     g_strdup_printf ( "%d/%d/%d",
				       g_date_day ( etat->date_perso_debut ),
				       g_date_month ( etat->date_perso_debut ),
				       g_date_year ( etat->date_perso_debut )));
      else
	xmlSetProp ( node_etat,
		     "Date_debut",
		     NULL );

      if ( etat->date_perso_fin )
	xmlSetProp ( node_etat,
		     "Date_fin",
		     g_strdup_printf ( "%d/%d/%d",
				       g_date_day ( etat->date_perso_fin ),
				       g_date_month ( etat->date_perso_fin ),
				       g_date_year ( etat->date_perso_fin )));
      else
	xmlSetProp ( node_etat,
		     "Date_fin",
		     NULL );


      xmlSetProp ( node_etat,
		   "Utilise_plages",
		   itoa ( etat -> separation_par_plage ));

      xmlSetProp ( node_etat,
		   "Sep_plages",
		   itoa ( etat -> type_separation_plage ));

      xmlSetProp ( node_etat,
		   "Deb_sem_plages",
		   itoa ( etat -> jour_debut_semaine ));


      xmlSetProp ( node_etat,
		   "Detail_comptes",
		   itoa ( etat -> utilise_detail_comptes ));

      pointeur_liste = etat -> no_comptes;
      pointeur_char = NULL;

      while ( pointeur_liste )
	{
	  if ( pointeur_char )
	    pointeur_char = g_strconcat ( pointeur_char,
					  "/",
					  itoa ( GPOINTER_TO_INT ( pointeur_liste -> data )),
					  NULL );
	  else
	    pointeur_char = itoa ( GPOINTER_TO_INT ( pointeur_liste -> data ));

	  pointeur_liste = pointeur_liste -> next;
	}

      xmlSetProp ( node_etat,
			"No_comptes",
			pointeur_char );

      xmlSetProp ( node_etat,
		   "Grp_ope_compte",
		   itoa ( etat -> regroupe_ope_par_compte ));

      xmlSetProp ( node_etat,
		   "Total_compte",
		   itoa ( etat -> affiche_sous_total_compte ));

      xmlSetProp ( node_etat,
		   "Aff_nom_compte",
		   itoa ( etat -> afficher_nom_compte ));

      xmlSetProp ( node_etat,
		   "Type_vir",
		   itoa ( etat -> type_virement ));

      pointeur_liste = etat -> no_comptes_virements;
      pointeur_char = NULL;

      while ( pointeur_liste )
	{
	  if ( pointeur_char )
	    pointeur_char = g_strconcat ( pointeur_char,
					  "/",
					  itoa ( GPOINTER_TO_INT ( pointeur_liste -> data )),
					  NULL );
	  else
	    pointeur_char = itoa ( GPOINTER_TO_INT ( pointeur_liste -> data ));

	  pointeur_liste = pointeur_liste -> next;
	}

      xmlSetProp ( node_etat,
			"No_comptes_virements",
			pointeur_char );

      xmlSetProp ( node_etat,
		   "Exclure_non_vir",
		   itoa ( etat -> exclure_ope_non_virement ));



      xmlSetProp ( node_etat,
		   "Categ",
		   itoa ( etat -> utilise_categ ));

      xmlSetProp ( node_etat,
		   "Detail_categ",
		   itoa ( etat -> utilise_detail_categ ));

      pointeur_liste = etat -> no_categ;
      pointeur_char = NULL;

      while ( pointeur_liste )
	{
	  if ( pointeur_char )
	    pointeur_char = g_strconcat ( pointeur_char,
					  "/",
					  itoa ( GPOINTER_TO_INT ( pointeur_liste -> data )),
					  NULL );
	  else
	    pointeur_char = itoa ( GPOINTER_TO_INT ( pointeur_liste -> data ));

	  pointeur_liste = pointeur_liste -> next;
	}

      xmlSetProp ( node_etat,
			"No_categ",
			pointeur_char );

      xmlSetProp ( node_etat,
		   "Exclut_categ",
		   itoa ( etat -> exclure_ope_sans_categ ));

      xmlSetProp ( node_etat,
		   "Total_categ",
		   itoa ( etat -> affiche_sous_total_categ ));

      xmlSetProp ( node_etat,
		   "Aff_ss_categ",
		   itoa ( etat -> afficher_sous_categ ));

      xmlSetProp ( node_etat,
		   "Aff_pas_ss_categ",
		   itoa ( etat -> afficher_pas_de_sous_categ ));

      xmlSetProp ( node_etat,
		   "Total_ss_categ",
		   itoa ( etat -> affiche_sous_total_sous_categ ));

      xmlSetProp ( node_etat,
		   "Devise_categ",
		   itoa ( etat -> devise_de_calcul_categ ));

      xmlSetProp ( node_etat,
		   "Aff_nom_categ",
		   itoa ( etat -> afficher_nom_categ ));

      xmlSetProp ( node_etat,
		   "IB",
		   itoa ( etat -> utilise_ib ));

      xmlSetProp ( node_etat,
		   "Detail_ib",
		   itoa ( etat -> utilise_detail_ib ));

      pointeur_liste = etat -> no_ib;
      pointeur_char = NULL;

      while ( pointeur_liste )
	{
	  if ( pointeur_char )
	    pointeur_char = g_strconcat ( pointeur_char,
					  "/",
					  itoa ( GPOINTER_TO_INT ( pointeur_liste -> data )),
					  NULL );
	  else
	    pointeur_char = itoa ( GPOINTER_TO_INT ( pointeur_liste -> data ));

	  pointeur_liste = pointeur_liste -> next;
	}

      xmlSetProp ( node_etat,
			"No_ib",
			pointeur_char );

      xmlSetProp ( node_etat,
		   "Exclut_ib",
		   itoa ( etat -> exclure_ope_sans_ib ));

      xmlSetProp ( node_etat,
		   "Total_ib",
		   itoa ( etat -> affiche_sous_total_ib ));

      xmlSetProp ( node_etat,
		   "Aff_ss_ib",
		   itoa ( etat -> afficher_sous_ib ));

      xmlSetProp ( node_etat,
		   "Aff_pas_ss_ib",
		   itoa ( etat -> afficher_pas_de_sous_ib ));

      xmlSetProp ( node_etat,
		   "Total_ss_ib",
		   itoa ( etat -> affiche_sous_total_sous_ib ));

      xmlSetProp ( node_etat,
		   "Devise_ib",
		   itoa ( etat -> devise_de_calcul_ib ));

      xmlSetProp ( node_etat,
		   "Aff_nom_ib",
		   itoa ( etat -> afficher_nom_ib ));

      xmlSetProp ( node_etat,
		   "Tiers",
		   itoa ( etat -> utilise_tiers ));

      xmlSetProp ( node_etat,
		   "Detail_tiers",
		   itoa ( etat -> utilise_detail_tiers ));

      pointeur_liste = etat -> no_tiers;
      pointeur_char = NULL;

      while ( pointeur_liste )
	{
	  if ( pointeur_char )
	    pointeur_char = g_strconcat ( pointeur_char,
					  "/",
					  itoa ( GPOINTER_TO_INT ( pointeur_liste -> data )),
					  NULL );
	  else
	    pointeur_char = itoa ( GPOINTER_TO_INT ( pointeur_liste -> data ));

	  pointeur_liste = pointeur_liste -> next;
	}

      xmlSetProp ( node_etat,
			"No_tiers",
			pointeur_char );

      xmlSetProp ( node_etat,
		   "Total_tiers",
		   itoa ( etat -> affiche_sous_total_tiers ));

      xmlSetProp ( node_etat,
		   "Devise_tiers",
		   itoa ( etat -> devise_de_calcul_tiers ));

      xmlSetProp ( node_etat,
		   "Aff_nom_tiers",
		   itoa ( etat -> afficher_nom_tiers ));

      xmlSetProp ( node_etat,
		   "Texte",
		   etat -> texte );



      xmlSetProp ( node_etat,
		   "Montant_n_p",
		   itoa ( etat -> utilise_montant_neg_pos ));

      xmlSetProp ( node_etat,
		   "Type_n_p",
		   itoa ( etat -> type_neg_pos ));

      xmlSetProp ( node_etat,
		   "Util_val",
		   itoa ( etat -> utilise_valeur ));

      xmlSetProp ( node_etat,
		   "Type_ope_val",
		   itoa ( etat -> type_operateur_valeur ));

      xmlSetProp ( node_etat,
		   "Montant_val",
		   g_strdup_printf ( "%4.2f",
				     etat -> montant_valeur ));
      xmlSetProp ( node_etat,
		   "Util_incl",
		   itoa ( etat -> utilise_inclusion ));

      xmlSetProp ( node_etat,
		   "Montant_incl_inf",
		   g_strdup_printf ( "%4.2f",
				     etat -> montant_inclusion_inf ));
      xmlSetProp ( node_etat,
		   "Type_ope_incl_inf",
		   itoa ( etat -> type_operateur_inf_inclusion ));

      xmlSetProp ( node_etat,
		   "Type_ope_incl_sup",
		   itoa ( etat -> type_operateur_sup_inclusion ));

      xmlSetProp ( node_etat,
		   "Montant_incl_sup",
		   g_strdup_printf ( "%4.2f",
				     etat -> montant_inclusion_sup ));

      xmlSetProp ( node_etat,
		   "Montant_nul",
		   itoa ( etat -> choix_montant_nul ));

      xmlSetProp ( node_etat,
		   "Montant_devise",
		   itoa ( etat -> choix_devise_montant ));

      pointeur_liste_2 = pointeur_liste_2 -> next;
    }




  /* l'arbre est fait, on sauvegarde */

  resultat = xmlSaveFile ( nom_fichier_comptes,
			   doc );

  /* on lib�re la memoire */

  xmlFreeDoc ( doc );


  if ( resultat == -1 )
    {
      dialogue ( g_strconcat ( _("Erreur dans l'enregistrement du fichier :\n\n"),
			       nom_fichier_comptes,
			       _("\n\nErreur :\n"),
			       strerror ( errno ),
			       NULL ));
      return ( FALSE );
    }

  modification_fichier ( FALSE );

  affiche_titre_fenetre ();

  return ( TRUE );
}
/***********************************************************************************************************/




/* ************************************************************************************************** */
/* itoa : transforme un integer en chaine ascii */
/* ************************************************************************************************** */

gchar *itoa ( gint integer )
{
  div_t result_div;
  gchar *chaine;
  gint i = 0;
  gint num;

  chaine = malloc ( 11*sizeof (char) );
  num = abs(integer);

  do
    {
      result_div = div ( num, 10 );
      chaine[i] = result_div.rem + 48;
      i++;
    }
  while ( ( num = result_div.quot ));

  chaine[i] = 0;

  g_strreverse ( chaine );

  if ( integer < 0 )
    chaine = g_strconcat ( "-",
			   chaine,
			   NULL );

  return ( chaine );
}
/***********************************************************************************************************/



/***********************************************************************************************************/
/* Fonction vire_les_points_virgules */
/* utilisee � l'enregistrement de texte */
/* quand les ; sont utilises comme separation */
/***********************************************************************************************************/

gchar *vire_les_points_virgules ( gchar *origine )
{

  if ( !origine )
    return ( NULL );

  return ( g_strstrip ( g_strdelimit ( g_strdup ( origine ),
				       ";",
				       '/' )));

}
/***********************************************************************************************************/



/***********************************************************************************************************/
/* Fonction vire_les_accolades */
/* utilisee � l'enregistrement de texte */
/* quand les {} sont utilises comme separation */
/***********************************************************************************************************/

gchar *vire_les_accolades ( gchar *origine )
{

  if ( !origine )
    return ( NULL );

  origine = g_strstrip ( g_strdelimit ( g_strdup ( origine ),
					"{",
					'/' ));
  return ( g_strstrip ( g_strdelimit ( g_strdup ( origine ),
				       "}",
				       '/' )));

}
/***********************************************************************************************************/




/***********************************************************************************************************/
/* ouvre le fichier, recherche <Fichier_ouvert> */
/* et le met � la valeur demandee */
/***********************************************************************************************************/

void fichier_marque_ouvert ( gint ouvert )
{
  gchar buffer[17];
  gint retour;

  if ( compression_fichier )
    return;

  /* ouverture du fichier */

  if (!(pointeur_fichier_comptes = fopen ( nom_fichier_comptes, "r+")) )
    {
      dialogue ( g_strconcat ( _("Erreur dans la tentative d'ouvrir le fichier\npour le d�clarer comme utilis�\n\n"),
			       strerror ( errno ),
			       NULL ));
      return;
    }

  do
    retour = fscanf ( pointeur_fichier_comptes, 
		      "%16s",
		      buffer);
  while ( strcmp ( buffer,
		   "<Fichier_ouvert>" )
	  &&
	  retour != EOF );

  /*   s'il n'a rien trouve, c'est que c'etait la version 0.3.1 et on passe */

  if ( retour != EOF )
    fprintf ( pointeur_fichier_comptes,
	      itoa ( ouvert ));

  fclose ( pointeur_fichier_comptes );

  return;
}
/***********************************************************************************************************/



/***********************************************************************************************************/
gboolean enregistre_etat ( gchar *nom_etat )
{
  xmlDocPtr doc;
  xmlNodePtr node;
  gint resultat;
  gchar *pointeur_char;
  GSList *pointeur_liste;
  GList *pointeur_list;

  /* creation de l'arbre xml en memoire */

  doc = xmlNewDoc("1.0");

  /* la racine est grisbi */

  doc->root = xmlNewDocNode ( doc,
			      NULL,
			      "Grisbi_etat",
			      NULL );

  /* on commence � ajouter les generalites */

  node = xmlNewChild ( doc->root,
		       NULL,
		       "Generalites",
		       NULL );
  xmlNewTextChild ( node,
		    NULL,
		    "Version_fichier_etat",
		    VERSION_FICHIER_ETAT );

  xmlNewTextChild ( node,
		    NULL,
		    "Version_grisbi",
		    VERSION );

  xmlNewTextChild ( node,
		    NULL,
		    "Nom",
		    etat_courant -> nom_etat );


  node = xmlNewChild ( doc->root,
		       NULL,
		       "Details",
		       NULL );

  pointeur_list = etat_courant -> type_classement;
  pointeur_char = NULL;

  while ( pointeur_list )
    {
      if ( pointeur_char )
	pointeur_char = g_strconcat ( pointeur_char,
				      "/",
				      itoa ( GPOINTER_TO_INT ( pointeur_list -> data )),
				      NULL );
      else
	pointeur_char = itoa ( GPOINTER_TO_INT ( pointeur_list -> data ));

      pointeur_list = pointeur_list -> next;
    }

  xmlSetProp ( node,
	       "Type_classement",
	       pointeur_char );


  xmlSetProp ( node,
	       "Aff_r",
	       itoa ( etat_courant -> afficher_r ));

  xmlSetProp ( node,
	       "Aff_ope",
	       itoa ( etat_courant -> afficher_opes ));

  xmlSetProp ( node,
	       "Aff_no_ope",
	       itoa ( etat_courant -> afficher_no_ope ));

  xmlSetProp ( node,
	       "Aff_date_ope",
	       itoa ( etat_courant -> afficher_date_ope ));

  xmlSetProp ( node,
	       "Aff_tiers_ope",
	       itoa ( etat_courant -> afficher_tiers_ope ));

  xmlSetProp ( node,
	       "Aff_categ_ope",
	       itoa ( etat_courant -> afficher_categ_ope ));

  xmlSetProp ( node,
	       "Aff_ss_categ_ope",
	       itoa ( etat_courant -> afficher_sous_categ_ope ));

  xmlSetProp ( node,
	       "Aff_type_ope",
	       itoa ( etat_courant -> afficher_type_ope ));

  xmlSetProp ( node,
	       "Aff_ib_ope",
	       itoa ( etat_courant -> afficher_ib_ope ));

  xmlSetProp ( node,
	       "Aff_ss_ib_ope",
	       itoa ( etat_courant -> afficher_sous_ib_ope ));

  xmlSetProp ( node,
	       "Aff_cheque_ope",
	       itoa ( etat_courant -> afficher_cheque_ope ));

  xmlSetProp ( node,
	       "Aff_notes_ope",
	       itoa ( etat_courant -> afficher_notes_ope ));

  xmlSetProp ( node,
	       "Aff_pc_ope",
	       itoa ( etat_courant -> afficher_pc_ope ));

  xmlSetProp ( node,
	       "Aff_rappr_ope",
	       itoa ( etat_courant -> afficher_rappr_ope ));

  xmlSetProp ( node,
	       "Aff_infobd_ope",
	       itoa ( etat_courant -> afficher_infobd_ope ));

  xmlSetProp ( node,
	       "Aff_exo_ope",
	       itoa ( etat_courant -> afficher_exo_ope ));

  xmlSetProp ( node,
	       "Aff_titres_col",
	       itoa ( etat_courant -> afficher_titre_colonnes ));

  xmlSetProp ( node,
	       "Aff_titres_chgt",
	       itoa ( etat_courant -> type_affichage_titres ));

  xmlSetProp ( node,
	       "Pas_detail_ventil",
	       itoa ( etat_courant -> pas_detailler_ventilation ));

  xmlSetProp ( node,
	       "Devise_gen",
	       itoa ( etat_courant -> devise_de_calcul_general ));


  xmlSetProp ( node,
	       "Exo_date",
	       itoa ( etat_courant -> exo_date ));

  xmlSetProp ( node,
	       "Detail_exo",
	       itoa ( etat_courant -> utilise_detail_exo ));

  pointeur_liste = etat_courant -> no_exercices;
  pointeur_char = NULL;

  while ( pointeur_liste )
    {
      if ( pointeur_char )
	pointeur_char = g_strconcat ( pointeur_char,
				      "/",
				      itoa ( GPOINTER_TO_INT ( pointeur_liste -> data )),
				      NULL );
      else
	pointeur_char = itoa ( GPOINTER_TO_INT ( pointeur_liste -> data ));

      pointeur_liste = pointeur_liste -> next;
    }

  xmlSetProp ( node,
	       "No_exo",
	       pointeur_char );

  xmlSetProp ( node,
	       "Plage_date",
	       itoa ( etat_courant -> no_plage_date ));


  if ( etat_courant->date_perso_debut )
    xmlSetProp ( node,
		 "Date_debut",
		 g_strdup_printf ( "%d/%d/%d",
				   g_date_day ( etat_courant->date_perso_debut ),
				   g_date_month ( etat_courant->date_perso_debut ),
				   g_date_year ( etat_courant->date_perso_debut )));
  else
    xmlSetProp ( node,
		 "Date_debut",
		 NULL );

  if ( etat_courant->date_perso_fin )
    xmlSetProp ( node,
		 "Date_fin",
		 g_strdup_printf ( "%d/%d/%d",
				   g_date_day ( etat_courant->date_perso_fin ),
				   g_date_month ( etat_courant->date_perso_fin ),
				   g_date_year ( etat_courant->date_perso_fin )));
  else
    xmlSetProp ( node,
		 "Date_fin",
		 NULL );

  xmlSetProp ( node,
	       "Utilise_plages",
	       itoa ( etat_courant -> separation_par_plage ));

  xmlSetProp ( node,
	       "Sep_plages",
	       itoa ( etat_courant -> type_separation_plage ));

  xmlSetProp ( node,
	       "Deb_sem_plages",
	       itoa ( etat_courant -> jour_debut_semaine ));

  xmlSetProp ( node,
	       "Detail_comptes",
	       itoa ( etat_courant -> utilise_detail_comptes ));

  pointeur_liste = etat_courant -> no_comptes;
  pointeur_char = NULL;

  while ( pointeur_liste )
    {
      if ( pointeur_char )
	pointeur_char = g_strconcat ( pointeur_char,
				      "/",
				      itoa ( GPOINTER_TO_INT ( pointeur_liste -> data )),
				      NULL );
      else
	pointeur_char = itoa ( GPOINTER_TO_INT ( pointeur_liste -> data ));

      pointeur_liste = pointeur_liste -> next;
    }

  xmlSetProp ( node,
	       "No_comptes",
	       pointeur_char );

  xmlSetProp ( node,
	       "Grp_ope_compte",
	       itoa ( etat_courant -> regroupe_ope_par_compte ));

  xmlSetProp ( node,
	       "Total_compte",
	       itoa ( etat_courant -> affiche_sous_total_compte ));

  xmlSetProp ( node,
	       "Aff_nom_compte",
	       itoa ( etat_courant -> afficher_nom_compte ));

  xmlSetProp ( node,
	       "Type_vir",
	       itoa ( etat_courant -> type_virement ));

  pointeur_liste = etat_courant -> no_comptes_virements;
  pointeur_char = NULL;

  while ( pointeur_liste )
    {
      if ( pointeur_char )
	pointeur_char = g_strconcat ( pointeur_char,
				      "/",
				      itoa ( GPOINTER_TO_INT ( pointeur_liste -> data )),
				      NULL );
      else
	pointeur_char = itoa ( GPOINTER_TO_INT ( pointeur_liste -> data ));

      pointeur_liste = pointeur_liste -> next;
    }

  xmlSetProp ( node,
	       "No_comptes_virements",
	       pointeur_char );

  xmlSetProp ( node,
	       "Exclure_non_vir",
	       itoa ( etat_courant -> exclure_ope_non_virement ));

  xmlSetProp ( node,
	       "Categ",
	       itoa ( etat_courant -> utilise_categ ));

  xmlSetProp ( node,
	       "Detail_categ",
	       itoa ( etat_courant -> utilise_detail_categ ));

  pointeur_liste = etat_courant -> no_categ;
  pointeur_char = NULL;

  while ( pointeur_liste )
    {
      if ( pointeur_char )
	pointeur_char = g_strconcat ( pointeur_char,
				      "/",
				      itoa ( GPOINTER_TO_INT ( pointeur_liste -> data )),
				      NULL );
      else
	pointeur_char = itoa ( GPOINTER_TO_INT ( pointeur_liste -> data ));

      pointeur_liste = pointeur_liste -> next;
    }

  xmlSetProp ( node,
	       "No_categ",
	       pointeur_char );

  xmlSetProp ( node,
	       "Exclut_categ",
	       itoa ( etat_courant -> exclure_ope_sans_categ ));

  xmlSetProp ( node,
	       "Total_categ",
	       itoa ( etat_courant -> affiche_sous_total_categ ));

  xmlSetProp ( node,
	       "Aff_ss_categ",
	       itoa ( etat_courant -> afficher_sous_categ ));

  xmlSetProp ( node,
	       "Aff_pas_ss_categ",
	       itoa ( etat_courant -> afficher_pas_de_sous_categ ));

  xmlSetProp ( node,
	       "Total_ss_categ",
	       itoa ( etat_courant -> affiche_sous_total_sous_categ ));

  xmlSetProp ( node,
	       "Devise_categ",
	       itoa ( etat_courant -> devise_de_calcul_categ ));

  xmlSetProp ( node,
	       "Aff_nom_categ",
	       itoa ( etat_courant -> afficher_nom_categ ));



  xmlSetProp ( node,
	       "IB",
	       itoa ( etat_courant -> utilise_ib ));

  xmlSetProp ( node,
	       "Detail_ib",
	       itoa ( etat_courant -> utilise_detail_ib ));

  pointeur_liste = etat_courant -> no_ib;
  pointeur_char = NULL;

  while ( pointeur_liste )
    {
      if ( pointeur_char )
	pointeur_char = g_strconcat ( pointeur_char,
				      "/",
				      itoa ( GPOINTER_TO_INT ( pointeur_liste -> data )),
				      NULL );
      else
	pointeur_char = itoa ( GPOINTER_TO_INT ( pointeur_liste -> data ));

      pointeur_liste = pointeur_liste -> next;
    }

  xmlSetProp ( node,
	       "No_ib",
	       pointeur_char );

  xmlSetProp ( node,
	       "Exclut_ib",
	       itoa ( etat_courant -> exclure_ope_sans_ib ));

  xmlSetProp ( node,
	       "Total_ib",
	       itoa ( etat_courant -> affiche_sous_total_ib ));

  xmlSetProp ( node,
	       "Aff_ss_ib",
	       itoa ( etat_courant -> afficher_sous_ib ));

  xmlSetProp ( node,
	       "Aff_pas_ss_ib",
	       itoa ( etat_courant -> afficher_pas_de_sous_ib ));

  xmlSetProp ( node,
	       "Total_ss_ib",
	       itoa ( etat_courant -> affiche_sous_total_sous_ib ));

  xmlSetProp ( node,
	       "Devise_ib",
	       itoa ( etat_courant -> devise_de_calcul_ib ));

  xmlSetProp ( node,
	       "Aff_nom_ib",
	       itoa ( etat_courant -> afficher_nom_ib ));

  xmlSetProp ( node,
	       "Tiers",
	       itoa ( etat_courant -> utilise_tiers ));

  xmlSetProp ( node,
	       "Detail_tiers",
	       itoa ( etat_courant -> utilise_detail_tiers ));

  pointeur_liste = etat_courant -> no_tiers;
  pointeur_char = NULL;

  while ( pointeur_liste )
    {
      if ( pointeur_char )
	pointeur_char = g_strconcat ( pointeur_char,
				      "/",
				      itoa ( GPOINTER_TO_INT ( pointeur_liste -> data )),
				      NULL );
      else
	pointeur_char = itoa ( GPOINTER_TO_INT ( pointeur_liste -> data ));

      pointeur_liste = pointeur_liste -> next;
    }

  xmlSetProp ( node,
	       "No_tiers",
	       pointeur_char );

  xmlSetProp ( node,
	       "Total_tiers",
	       itoa ( etat_courant -> affiche_sous_total_tiers ));

  xmlSetProp ( node,
	       "Devise_tiers",
	       itoa ( etat_courant -> devise_de_calcul_tiers ));

  xmlSetProp ( node,
	       "Aff_nom_tiers",
	       itoa ( etat_courant -> afficher_nom_tiers ));

  xmlSetProp ( node,
	       "Texte",
	       etat_courant -> texte );


  xmlSetProp ( node,
	       "Montant_n_p",
	       itoa ( etat_courant -> utilise_montant_neg_pos ));

  xmlSetProp ( node,
	       "Type_n_p",
	       itoa ( etat_courant -> type_neg_pos ));

  xmlSetProp ( node,
	       "Util_val",
	       itoa ( etat_courant -> utilise_valeur ));

  xmlSetProp ( node,
	       "Type_ope_val",
	       itoa ( etat_courant -> type_operateur_valeur ));

  xmlSetProp ( node,
	       "Montant_val",
	       g_strdup_printf ( "%4.2f",
				 etat_courant -> montant_valeur ));
  xmlSetProp ( node,
	       "Util_incl",
	       itoa ( etat_courant -> utilise_inclusion ));

  xmlSetProp ( node,
	       "Montant_incl_inf",
	       g_strdup_printf ( "%4.2f",
				 etat_courant -> montant_inclusion_inf ));
  xmlSetProp ( node,
	       "Type_ope_incl_inf",
	       itoa ( etat_courant -> type_operateur_inf_inclusion ));

  xmlSetProp ( node,
	       "Type_ope_incl_sup",
	       itoa ( etat_courant -> type_operateur_sup_inclusion ));

  xmlSetProp ( node,
	       "Montant_incl_sup",
	       g_strdup_printf ( "%4.2f",
				 etat_courant -> montant_inclusion_sup ));
 
  xmlSetProp ( node,
	       "Montant_nul",
	       itoa ( etat_courant -> choix_montant_nul ));

  xmlSetProp ( node,
	       "Montant_devise",
	       itoa ( etat_courant -> choix_devise_montant ));



  /* l'arbre est fait, on sauvegarde */

  resultat = xmlSaveFile ( nom_etat,
			   doc );

  /* on lib�re la memoire */

  xmlFreeDoc ( doc );


  if ( resultat == -1 )
    {
      dialogue ( g_strconcat ( _("Erreur dans l'enregistrement du fichier :\n\n"),
			       nom_etat,
			       _("\n\nErreur :\n"),
			       strerror ( errno ),
			       NULL ));
      return ( FALSE );
    }


  return ( TRUE );
}
/***********************************************************************************************************/



/***********************************************************************************************************/
gboolean charge_etat ( gchar *nom_etat )
{
  xmlDocPtr doc;

  doc = xmlParseFile ( nom_etat );

  if ( doc )
    {
      /* v�rifications d'usage */

      if ( !doc->root
	   ||
	   !doc->root->name
	   ||
	   g_strcasecmp ( doc->root->name,
			  _("Grisbi_etat") ))
	{
	  dialogue ( _("Ce fichier n'est pas un �tat Grisbi") );
	  xmlFreeDoc ( doc );
	  return ( FALSE );
	}

      /* r�cup�re la version de fichier */

      if (( !strcmp (  xmlNodeGetContent ( doc->root->childs->childs ),
		       "0.4.0" )))
	return ( charge_etat_version_0_4_0 ( doc ));

      /* 	� ce niveau, c'est que que la version n'est pas connue de grisbi, on donne alors */
      /* la version n�cessaire pour l'ouvrir */

      dialogue ( g_strdup_printf ( _("Pour ouvrir ce fichier, il vous faut la version %s de Grisbi "),
				   xmlNodeGetContent ( doc->root->childs->childs->next )));

      xmlFreeDoc ( doc );

      return ( FALSE );
    }
  else
    {
      dialogue ( _("Fichier d'�tat invalide.") );
      return ( FALSE );
    }
}
/***********************************************************************************************************/



/***********************************************************************************************************/
gboolean charge_etat_version_0_4_0 ( xmlDocPtr doc )
{
  xmlNodePtr node;
  struct struct_etat *etat;
		      
  etat = calloc ( 1,
		  sizeof ( struct struct_etat ));

  /* on place node sur les generalites */

  node = doc -> root -> childs;

  while ( node )
    {
      if ( !strcmp ( node -> name,
		     "Generalites" ) )
	{
	  xmlNodePtr node_generalites;

	  /* node_generalites va faire le tour des generalites */

	  node_generalites = node -> childs;

	  while ( node_generalites )
	    {
	      if ( !strcmp ( node_generalites -> name,
			     "Nom" ))
		etat -> nom_etat = xmlNodeGetContent ( node_generalites );


	      node_generalites = node_generalites -> next;
	    }
	}

      /* on recup�re ici l'etat */

      if ( !strcmp ( node -> name,
		     "Details" ))
	{
	  etat -> type_classement = NULL;

	  if ( strlen ( xmlGetProp ( node,
				     "Type_classement" )))
	    {
	      gchar **pointeur_char;
	      gint i;

	      pointeur_char = g_strsplit ( xmlGetProp ( node,
							"Type_classement" ),
					   "/",
					   0 );
	      i=0;

	      while ( pointeur_char[i] )
		{
		  etat -> type_classement = g_list_append ( etat -> type_classement,
							    GINT_TO_POINTER ( atoi ( pointeur_char[i] )));
		  i++;
		}
	      g_strfreev ( pointeur_char );
	    }


	  etat -> afficher_r = atoi ( xmlGetProp ( node,
						   "Aff_r" ));

	  etat -> afficher_opes = atoi ( xmlGetProp ( node,
						      "Aff_ope" ));

	  etat -> afficher_no_ope = atoi ( xmlGetProp ( node,
							"Aff_no_ope" ));

	  etat -> afficher_date_ope = atoi ( xmlGetProp ( node,
							  "Aff_date_ope" ));

	  etat -> afficher_tiers_ope = atoi ( xmlGetProp ( node,
							   "Aff_tiers_ope" ));

	  etat -> afficher_categ_ope = atoi ( xmlGetProp ( node,
							   "Aff_categ_ope" ));

	  etat -> afficher_sous_categ_ope = atoi ( xmlGetProp ( node,
								"Aff_ss_categ_ope" ));

	  etat -> afficher_type_ope = atoi ( xmlGetProp ( node,
							  "Aff_type_ope" ));

	  etat -> afficher_ib_ope = atoi ( xmlGetProp ( node,
							"Aff_ib_ope" ));

	  etat -> afficher_sous_ib_ope = atoi ( xmlGetProp ( node,
							     "Aff_ss_ib_ope" ));

	  etat -> afficher_cheque_ope = atoi ( xmlGetProp ( node,
							    "Aff_cheque_ope" ));

	  etat -> afficher_notes_ope = atoi ( xmlGetProp ( node,
							   "Aff_notes_ope" ));

	  etat -> afficher_pc_ope = atoi ( xmlGetProp ( node,
							"Aff_pc_ope" ));

	  etat -> afficher_rappr_ope = atoi ( xmlGetProp ( node,
							   "Aff_rappr_ope" ));

	  etat -> afficher_infobd_ope = atoi ( xmlGetProp ( node,
							    "Aff_infobd_ope" ));

	  etat -> afficher_exo_ope = atoi ( xmlGetProp ( node,
							 "Aff_exo_ope" ));

	  etat -> afficher_titre_colonnes = atoi ( xmlGetProp ( node,
								"Aff_titres_col" ));

	  etat -> type_affichage_titres = atoi ( xmlGetProp ( node,
							      "Aff_titres_chgt" ));

	  etat -> pas_detailler_ventilation = atoi ( xmlGetProp ( node,
								  "Pas_detail_ventil" ));

	  etat -> devise_de_calcul_general = atoi ( xmlGetProp ( node,
								 "Devise_gen" ));

	  etat -> exo_date = atoi ( xmlGetProp ( node,
						 "Exo_date" ));
							  			  
	  etat -> utilise_detail_exo = atoi ( xmlGetProp ( node,
							   "Detail_exo" ));
							  			  
	  etat -> no_exercices = NULL;

	  if ( strlen ( xmlGetProp ( node,
				     "No_exo" )))
	    {
	      gchar **pointeur_char;
	      gint i;

	      pointeur_char = g_strsplit ( xmlGetProp ( node,
							"No_exo" ),
					   "/",
					   0 );
	      i=0;

	      while ( pointeur_char[i] )
		{
		  etat -> no_exercices = g_slist_append ( etat -> no_exercices,
							  GINT_TO_POINTER ( atoi ( pointeur_char[i] )));
		  i++;
		}
	      g_strfreev ( pointeur_char );
	    }
			
							 
	  etat -> no_plage_date = atoi ( xmlGetProp ( node,
						      "Plage_date" ));

	  if ( strlen ( xmlGetProp ( node,
				     "Date_debut" )))
	    {
	      gchar **pointeur_char;

	      pointeur_char = g_strsplit ( xmlGetProp ( node,
							"Date_debut" ),
					   "/",
					   3 );

	      etat -> date_perso_debut = g_date_new_dmy ( atoi ( pointeur_char[0] ),
							  atoi ( pointeur_char[1] ),
							  atoi ( pointeur_char[2] ));
	      g_strfreev ( pointeur_char );
	    }
	  else
	    etat -> date_perso_debut = NULL;

	  if ( strlen ( xmlGetProp ( node,
				     "Date_fin" )))
	    {
	      gchar **pointeur_char;

	      pointeur_char = g_strsplit ( xmlGetProp ( node,
							"Date_fin" ),
					   "/",
					   3 );

	      etat -> date_perso_fin = g_date_new_dmy ( atoi ( pointeur_char[0] ),
							atoi ( pointeur_char[1] ),
							atoi ( pointeur_char[2] ));
	      g_strfreev ( pointeur_char );
	    }
	  else
	    etat -> date_perso_fin = NULL;

	  etat -> separation_par_plage = atoi ( xmlGetProp ( node,
							     "Utilise_plages" ));

	  etat -> type_separation_plage = atoi ( xmlGetProp ( node,
							      "Sep_plages" ));

	  etat -> jour_debut_semaine = atoi ( xmlGetProp ( node,
							   "Deb_sem_plages" ));


	  etat -> utilise_detail_comptes = atoi ( xmlGetProp ( node,
							       "Detail_comptes" ));

	  etat -> no_comptes = NULL;

	  if ( strlen ( xmlGetProp ( node,
				     "No_comptes" )))
	    {
	      gchar **pointeur_char;
	      gint i;

	      pointeur_char = g_strsplit ( xmlGetProp ( node,
							"No_comptes" ),
					   "/",
					   0 );
	      i=0;

	      while ( pointeur_char[i] )
		{
		  etat ->no_comptes  = g_slist_append ( etat -> no_comptes,
							GINT_TO_POINTER ( atoi ( pointeur_char[i] )));
		  i++;
		}
	      g_strfreev ( pointeur_char );
	    }


	  etat -> regroupe_ope_par_compte = atoi ( xmlGetProp ( node,
								"Grp_ope_compte" ));

	  etat -> affiche_sous_total_compte = atoi ( xmlGetProp ( node,
								  "Total_compte" ));

	  etat -> afficher_nom_compte = atoi ( xmlGetProp ( node,
								  "Aff_nom_compte" ));

	  etat -> type_virement = atoi ( xmlGetProp ( node,
						      "Type_vir" ));


	  if ( strlen ( xmlGetProp ( node,
				     "No_comptes_virements" )))
	    {
	      gchar **pointeur_char;
	      gint i;

	      pointeur_char = g_strsplit ( xmlGetProp ( node,
							"No_comptes_virements" ),
					   "/",
					   0 );
	      i=0;

	      while ( pointeur_char[i] )
		{
		  etat ->no_comptes_virements  = g_slist_append ( etat -> no_comptes_virements,
								  GINT_TO_POINTER ( atoi ( pointeur_char[i] )));
		  i++;
		}
	      g_strfreev ( pointeur_char );
	    }

	  etat -> exclure_ope_non_virement = atoi ( xmlGetProp ( node,
								 "Exclure_non_vir" ));

	  etat -> utilise_categ = atoi ( xmlGetProp ( node,
						      "Categ" ));
	  etat -> utilise_detail_categ = atoi ( xmlGetProp ( node,
							     "Detail_categ" ));

	  etat -> no_categ = NULL;

	  if ( strlen ( xmlGetProp ( node,
				     "No_categ" )))
	    {
	      gchar **pointeur_char;
	      gint i;

	      pointeur_char = g_strsplit ( xmlGetProp ( node,
							"No_categ" ),
					   "/",
					   0 );
	      i=0;

	      while ( pointeur_char[i] )
		{
		  etat -> no_categ = g_slist_append ( etat -> no_categ,
						      GINT_TO_POINTER ( atoi ( pointeur_char[i] )));
		  i++;
		}
	      g_strfreev ( pointeur_char );
	    }

	  etat -> exclure_ope_sans_categ = atoi ( xmlGetProp ( node,
							       "Exclut_categ" ));

	  etat -> affiche_sous_total_categ = atoi ( xmlGetProp ( node,
								 "Total_categ" ));

	  etat -> afficher_sous_categ = atoi ( xmlGetProp ( node,
							    "Aff_ss_categ" ));

	  etat -> afficher_pas_de_sous_categ = atoi ( xmlGetProp ( node,
								   "Aff_pas_ss_categ" ));

	  etat -> affiche_sous_total_sous_categ = atoi ( xmlGetProp ( node,
								      "Total_ss_categ" ));

	  etat -> devise_de_calcul_categ = atoi ( xmlGetProp ( node,
							       "Devise_categ" ));
	  etat -> afficher_nom_categ = atoi ( xmlGetProp ( node,
							   "Aff_nom_categ" ));

	  etat -> utilise_ib = atoi ( xmlGetProp ( node,
						   "IB" ));
	  etat -> utilise_detail_ib = atoi ( xmlGetProp ( node,
							  "Detail_ib" ));
	  etat -> no_ib = NULL;

	  if ( strlen ( xmlGetProp ( node,
				     "No_ib" )))
	    {
	      gchar **pointeur_char;
	      gint i;

	      pointeur_char = g_strsplit ( xmlGetProp ( node,
							"No_ib" ),
					   "/",
					   0 );
	      i=0;

	      while ( pointeur_char[i] )
		{
		  etat -> no_ib = g_slist_append ( etat -> no_ib,
						   GINT_TO_POINTER ( atoi ( pointeur_char[i] )));
		  i++;
		}
	      g_strfreev ( pointeur_char );
	    }


	  etat -> exclure_ope_sans_ib = atoi ( xmlGetProp ( node,
							    "Exclut_ib" ));

	  etat -> affiche_sous_total_ib = atoi ( xmlGetProp ( node,
							      "Total_ib" ));

	  etat -> afficher_sous_ib = atoi ( xmlGetProp ( node,
							 "Aff_ss_ib" ));

	  etat -> afficher_pas_de_sous_ib = atoi ( xmlGetProp ( node,
								"Aff_pas_ss_ib" ));

	  etat -> affiche_sous_total_sous_ib = atoi ( xmlGetProp ( node,
								   "Total_ss_ib" ));

	  etat -> devise_de_calcul_ib = atoi ( xmlGetProp ( node,
							    "Devise_ib" ));

	  etat -> afficher_nom_ib = atoi ( xmlGetProp ( node,
							    "Aff_nom_ib" ));

	  etat -> utilise_tiers = atoi ( xmlGetProp ( node,
						      "Tiers" ));
	  etat -> utilise_detail_tiers = atoi ( xmlGetProp ( node,
							     "Detail_tiers" ));

	  etat -> no_tiers = NULL;

	  if ( strlen ( xmlGetProp ( node,
				     "No_tiers" )))
	    {
	      gchar **pointeur_char;
	      gint i;

	      pointeur_char = g_strsplit ( xmlGetProp ( node,
							"No_tiers" ),
					   "/",
					   0 );
	      i=0;

	      while ( pointeur_char[i] )
		{
		  etat -> no_tiers = g_slist_append ( etat -> no_tiers,
						      GINT_TO_POINTER ( atoi ( pointeur_char[i] )));
		  i++;
		}
	      g_strfreev ( pointeur_char );
	    }

	  etat -> affiche_sous_total_tiers = atoi ( xmlGetProp ( node,
								 "Total_tiers" ));

	  etat -> devise_de_calcul_tiers = atoi ( xmlGetProp ( node,
							       "Devise_tiers" ));

	  etat -> afficher_nom_tiers = atoi ( xmlGetProp ( node,
							       "Aff_nom_tiers" ));

	  if ( strlen ( xmlGetProp ( node,
				     "Texte" )))
	    etat -> texte = xmlGetProp ( node,
					 "Texte" );
	  else
	    etat -> texte = NULL;


	  etat -> utilise_montant_neg_pos = atoi ( xmlGetProp ( node,
								"Montant_n_p" ));

	  etat -> type_neg_pos = atoi ( xmlGetProp ( node,
						     "Type_n_p" ));

	  etat -> utilise_valeur = atoi ( xmlGetProp ( node,
						       "Util_val" ));

	  etat -> type_operateur_valeur = atoi ( xmlGetProp ( node,
							      "Type_ope_val" ));

	  etat -> montant_valeur = g_strtod ( xmlGetProp ( node,
							   "Montant_val" ),
					      NULL );

	  etat -> utilise_inclusion = atoi ( xmlGetProp ( node,
							  "Util_incl" ));

	  etat -> montant_inclusion_inf = g_strtod ( xmlGetProp ( node,
								  "Montant_incl_inf" ),
						     NULL );

	  etat -> type_operateur_inf_inclusion = atoi ( xmlGetProp ( node,
								     "Type_ope_incl_inf" ));

	  etat -> type_operateur_sup_inclusion = atoi ( xmlGetProp ( node,
								     "Type_ope_incl_sup" ));

	  etat -> montant_inclusion_sup = g_strtod ( xmlGetProp ( node,
								  "Montant_incl_sup" ),
						     NULL );

	  etat -> choix_montant_nul = atoi ( xmlGetProp ( node,
							  "Montant_nul" ));
	  
	  etat -> choix_devise_montant = atoi ( xmlGetProp ( node,
							     "Montant_devise" ));

	  liste_struct_etats = g_slist_append ( liste_struct_etats,
						etat );
	}
      node = node -> next;
    }

  /* on lib�re la memoire */
  
  xmlFreeDoc ( doc );

  /* on rafraichit la liste des �tats */
 
  remplissage_liste_etats ();

  gtk_widget_set_sensitive ( bouton_effacer_etat,
			     TRUE );

  modification_fichier ( TRUE );

  return ( TRUE );
}
/***********************************************************************************************************/





/***********************************************************************************************************/
gboolean enregistre_categ ( gchar *nom_categ )
{
  xmlDocPtr doc;
  xmlNodePtr node;
  gint resultat;
  GSList *pointeur_liste;

  /* creation de l'arbre xml en memoire */

  doc = xmlNewDoc("1.0");

  /* la racine est grisbi */

  doc->root = xmlNewDocNode ( doc,
			      NULL,
			      "Grisbi_categ",
			      NULL );

  /* on commence � ajouter les generalites */

  node = xmlNewChild ( doc->root,
		       NULL,
		       "Generalites",
		       NULL );
  xmlNewTextChild ( node,
		    NULL,
		    "Version_fichier_categ",
		    VERSION_FICHIER_CATEG );

  xmlNewTextChild ( node,
		    NULL,
		    "Version_grisbi",
		    VERSION );


  node = xmlNewChild ( doc->root,
		       NULL,
		       "Details_des_categories",
		       NULL );


  pointeur_liste = liste_struct_categories;

  while ( pointeur_liste )
    {
      struct struct_categ *categ;
      xmlNodePtr node_categ;
      GSList *pointeur_sous_categ;

      categ = pointeur_liste -> data;


      node_categ = xmlNewChild ( node,
				 NULL,
				 "Categorie",
				 NULL );

      xmlSetProp ( node_categ,
		   "Nom",
		   categ -> nom_categ );

      xmlSetProp ( node_categ,
		   "Type",
		   itoa ( categ -> type_categ ));

      xmlSetProp ( node_categ,
		   "No_derniere_sous_cagegorie",
		   itoa ( categ -> no_derniere_sous_categ ));


      /* on ajoute les sous categories */

      pointeur_sous_categ = categ -> liste_sous_categ;

      while ( pointeur_sous_categ )
	{
	  struct struct_sous_categ *sous_categ;
	  xmlNodePtr node_sous_categ;

	  sous_categ = pointeur_sous_categ -> data;

	  node_sous_categ = xmlNewChild ( node_categ,
					  NULL,
					  "Sous-categorie",
					  NULL );

	  xmlSetProp ( node_sous_categ,
		       "No",
		       itoa ( sous_categ -> no_sous_categ ));

	  xmlSetProp ( node_sous_categ,
		       "Nom",
		       sous_categ -> nom_sous_categ );

	  pointeur_sous_categ = pointeur_sous_categ -> next;
	}
      pointeur_liste = pointeur_liste -> next;
    }


  /* l'arbre est fait, on sauvegarde */

  resultat = xmlSaveFile ( nom_categ,
			   doc );

  /* on lib�re la memoire */

  xmlFreeDoc ( doc );


  if ( resultat == -1 )
    {
      dialogue ( g_strconcat ( _("Erreur dans l'enregistrement du fichier :\n\n"),
			       nom_categ,
			       _("\n\nErreur :\n"),
			       strerror ( errno ),
			       NULL ));
      return ( FALSE );
    }


  return ( TRUE );
}
/***********************************************************************************************************/




/***********************************************************************************************************/
gboolean charge_categ ( gchar *nom_categ )
{
  xmlDocPtr doc;

  doc = xmlParseFile ( nom_categ );

  if ( doc )
    {
      /* v�rifications d'usage */

      if ( !doc->root
	   ||
	   !doc->root->name
	   ||
	   g_strcasecmp ( doc->root->name,
			  _("Grisbi_categ") ))
	{
	  dialogue ( _("Ce fichier n'est pas une liste de cat�gories Grisbi") );
	  xmlFreeDoc ( doc );
	  return ( FALSE );
	}

      /* r�cup�re la version de fichier */

      if (( !strcmp (  xmlNodeGetContent ( doc->root->childs->childs ),
		       "0.4.0" )))
	return ( charge_categ_version_0_4_0 ( doc ));

      /* 	� ce niveau, c'est que que la version n'est pas connue de grisbi, on donne alors */
      /* la version n�cessaire pour l'ouvrir */

      dialogue ( g_strdup_printf ( _("Pour ouvrir ce fichier, il vous faut la version %s de Grisbi "),
				   xmlNodeGetContent ( doc->root->childs->childs->next )));

      xmlFreeDoc ( doc );

      return ( FALSE );
    }
  else
    {
      dialogue ( _("Fichier de cat�gories invalide.") );
      return ( FALSE );
    }
}
/***********************************************************************************************************/




/***********************************************************************************************************/
gboolean charge_categ_version_0_4_0 ( xmlDocPtr doc )
{
  xmlNodePtr node;
  struct struct_categ *categ;
		      
  categ = calloc ( 1,
						     sizeof ( struct struct_categ ));

  /* on place node sur les generalites */

  node = doc -> root -> childs;


  while ( node )
    {
      if ( !strcmp ( node -> name,
		     "Generalites" ) )
	{
	  xmlNodePtr node_generalites;

	  /* node_generalites va faire le tour des generalites */

	  node_generalites = node -> childs;

	  while ( node_generalites )
	    {
	      /* rien pour l'instant, peut �tre un jour ? */


	      node_generalites = node_generalites -> next;
	    }
	}

      /* on recup�re ici les cat�g */

      if ( !strcmp ( node -> name,
		     "Details_des_categories" ))
	{
	  xmlNodePtr node_detail;
	  xmlNodePtr node_sous_categ;

	  node_detail = node -> childs;

	  while ( node_detail )
	    {
	      GSList *liste_tmp;
	      struct struct_categ *categorie;

	      /* 	      on doit r�aliser une fusion, pour �a, soit la cat�g existe, et on fait le */
	      /* tour des sous cat�g en ajoutant celles qui n'existent pas, soit elle n'existe pas et on */
	      /* ajoute la cat�g et ses sous categ */

	      liste_tmp = g_slist_find_custom ( liste_struct_categories,
						xmlGetProp ( node_detail,
							     "Nom" ),
						(GCompareFunc) recherche_categorie_par_nom );


	      if ( liste_tmp )
		{
		  /* 		  la cat�gorie existe, on fait le tour des sous cat�gories */

		  categorie = liste_tmp -> data;

		  node_sous_categ = node_detail -> childs;

		  while ( node_sous_categ )
		    {
		      struct struct_sous_categ *sous_categ;
		      GSList *liste_tmp_2;

		      /* on ne prend la sous cat�g que si elle n'existe pas */

		      liste_tmp_2 = g_slist_find_custom ( categorie -> liste_sous_categ,
							  xmlGetProp ( node_sous_categ,
								       "Nom" ),
							  (GCompareFunc) recherche_sous_categorie_par_nom );

		      if ( !liste_tmp_2 )
			{

			  sous_categ = calloc ( 1,
						     sizeof ( struct struct_sous_categ ) );

			  sous_categ -> no_sous_categ = ++categorie -> no_derniere_sous_categ;

			  sous_categ -> nom_sous_categ = xmlGetProp ( node_sous_categ,
								      "Nom" );

			  categorie -> liste_sous_categ = g_slist_append ( categorie -> liste_sous_categ,
									   sous_categ );
			}
		      node_sous_categ = node_sous_categ -> next;
		    }
		}
	      else
		{
		  /* la cat�gorie n'existe pas, on l'ajoute */

		      
		  categorie = calloc ( 1,
						     sizeof ( struct struct_categ ) );

		  categorie -> no_categ = ++no_derniere_categorie;
		  nb_enregistrements_categories++;

		  categorie -> nom_categ = xmlGetProp ( node_detail,
							"Nom" );
		  categorie -> type_categ = atoi ( xmlGetProp ( node_detail,
								"Type" ));
		  categorie -> no_derniere_sous_categ = atoi ( xmlGetProp ( node_detail,
									    "No_derniere_sous_cagegorie" ));

		  /*  pour chaque categorie, on recup�re les sous-categories */

		  categorie -> liste_sous_categ = NULL;
		  node_sous_categ = node_detail -> childs;

		  while ( node_sous_categ )
		    {
		      struct struct_sous_categ *sous_categ;

		      sous_categ = calloc ( 1,
						     sizeof ( struct struct_sous_categ ) );

		      sous_categ -> no_sous_categ = atoi ( xmlGetProp ( node_sous_categ,
									"No" ));
		      sous_categ -> nom_sous_categ = xmlGetProp ( node_sous_categ,
								  "Nom" );

		      categorie -> liste_sous_categ = g_slist_append ( categorie -> liste_sous_categ,
								       sous_categ );
		      node_sous_categ = node_sous_categ -> next;
		    }

		  liste_struct_categories = g_slist_append ( liste_struct_categories,
							     categorie );
		}
	      node_detail = node_detail -> next;
	    }
	}
      node = node -> next;
    }

  /* on lib�re la memoire */
  
  xmlFreeDoc ( doc );

  /* creation de la liste des categ pour le combofix */

  creation_liste_categ_combofix ();
  mise_a_jour_categ ();
  remplit_arbre_categ ();

  /* on rafraichit la liste des categ */
 

  modification_fichier ( TRUE );

  return ( TRUE );
}
/***********************************************************************************************************/








/***********************************************************************************************************/
gboolean enregistre_ib ( gchar *nom_ib )
{
  xmlDocPtr doc;
  xmlNodePtr node;
  gint resultat;
  GSList *pointeur_liste;

  /* creation de l'arbre xml en memoire */

  doc = xmlNewDoc("1.0");

  /* la racine est grisbi */

  doc->root = xmlNewDocNode ( doc,
			      NULL,
			      "Grisbi_ib",
			      NULL );

  /* on commence � ajouter les generalites */

  node = xmlNewChild ( doc->root,
		       NULL,
		       "Generalites",
		       NULL );
  xmlNewTextChild ( node,
		    NULL,
		    "Version_fichier_ib",
		    VERSION_FICHIER_IB );

  xmlNewTextChild ( node,
		    NULL,
		    "Version_grisbi",
		    VERSION );


  node = xmlNewChild ( doc->root,
		       NULL,
		       "Details_des_ib",
		       NULL );


  pointeur_liste = liste_struct_imputation;

  while ( pointeur_liste )
    {
      struct struct_imputation *ib;
      xmlNodePtr node_ib;
      GSList *pointeur_sous_ib;

      ib = pointeur_liste -> data;


      node_ib = xmlNewChild ( node,
				 NULL,
				 "Imputation",
				 NULL );

      xmlSetProp ( node_ib,
		   "Nom",
		   ib -> nom_imputation );

      xmlSetProp ( node_ib,
		   "Type",
		   itoa ( ib -> type_imputation ));

      xmlSetProp ( node_ib,
		   "No_derniere_sous_imputation",
		   itoa ( ib -> no_derniere_sous_imputation ));


      /* on ajoute les sous ib */

      pointeur_sous_ib = ib -> liste_sous_imputation;

      while ( pointeur_sous_ib )
	{
	  struct struct_sous_imputation *sous_ib;
	  xmlNodePtr node_sous_ib;

	  sous_ib = pointeur_sous_ib -> data;

	  node_sous_ib = xmlNewChild ( node_ib,
				       NULL,
				       "Sous-imputation",
				       NULL );

	  xmlSetProp ( node_sous_ib,
		       "No",
		       itoa ( sous_ib -> no_sous_imputation ));

	  xmlSetProp ( node_sous_ib,
		       "Nom",
		       sous_ib -> nom_sous_imputation );

	  pointeur_sous_ib = pointeur_sous_ib -> next;
	}
      pointeur_liste = pointeur_liste -> next;
    }


  /* l'arbre est fait, on sauvegarde */

  resultat = xmlSaveFile ( nom_ib,
			   doc );

  /* on lib�re la memoire */

  xmlFreeDoc ( doc );


  if ( resultat == -1 )
    {
      dialogue ( g_strconcat ( _("Erreur dans l'enregistrement du fichier :\n\n"),
			       nom_ib,
			       _("\n\nErreur :\n"),
			       strerror ( errno ),
			       NULL ));
      return ( FALSE );
    }


  return ( TRUE );
}
/***********************************************************************************************************/




/***********************************************************************************************************/
gboolean charge_ib ( gchar *nom_ib )
{
  xmlDocPtr doc;

  doc = xmlParseFile ( nom_ib );

  if ( doc )
    {
      /* v�rifications d'usage */

      if ( !doc->root
	   ||
	   !doc->root->name
	   ||
	   g_strcasecmp ( doc->root->name,
			  _("Grisbi_ib") ))
	{
	  dialogue ( _("Ce fichier n'est pas une liste d'imputaions budg�taires Grisbi") );
	  xmlFreeDoc ( doc );
	  return ( FALSE );
	}

      /* r�cup�re la version de fichier */

      if (( !strcmp (  xmlNodeGetContent ( doc->root->childs->childs ),
		       "0.4.0" )))
	return ( charge_ib_version_0_4_0 ( doc ));

      /* 	� ce niveau, c'est que que la version n'est pas connue de grisbi, on donne alors */
      /* la version n�cessaire pour l'ouvrir */

      dialogue ( g_strdup_printf ( _("Pour ouvrir ce fichier, il vous faut la version %s de Grisbi "),
				   xmlNodeGetContent ( doc->root->childs->childs->next )));

      xmlFreeDoc ( doc );

      return ( FALSE );
    }
  else
    {
      dialogue ( _("Fichier d'imputations budg�taires invalide.") );
      return ( FALSE );
    }
}
/***********************************************************************************************************/




/***********************************************************************************************************/
gboolean charge_ib_version_0_4_0 ( xmlDocPtr doc )
{
  xmlNodePtr node;
  struct struct_imputation *ib;
		      
  ib = calloc ( 1,
						     sizeof ( struct struct_imputation ));

  /* on place node sur les generalites */

  node = doc -> root -> childs;


  while ( node )
    {
      if ( !strcmp ( node -> name,
		     "Generalites" ) )
	{
	  xmlNodePtr node_generalites;

	  /* node_generalites va faire le tour des generalites */

	  node_generalites = node -> childs;

	  while ( node_generalites )
	    {
	      /* rien pour l'instant, peut �tre un jour ? */


	      node_generalites = node_generalites -> next;
	    }
	}

      /* on recup�re ici les ib */

      if ( !strcmp ( node -> name,
		     "Details_des_ib" ))
	{
	  xmlNodePtr node_detail;
	  xmlNodePtr node_sous_ib;

	  node_detail = node -> childs;

	  while ( node_detail )
	    {
	      GSList *liste_tmp;
	      struct struct_imputation *ib;

	      /* 	      on doit r�aliser une fusion, pour �a, soit l'ib existe, et on fait le */
	      /* tour des sous ib en ajoutant celles qui n'existent pas, soit elle n'existe pas et on */
	      /* ajoute l'ib et ses sous ib */

	      liste_tmp = g_slist_find_custom ( liste_struct_imputation,
						xmlGetProp ( node_detail,
							     "Nom" ),
						(GCompareFunc) recherche_imputation_par_nom );


	      if ( liste_tmp )
		{
		  /* 		  la cat�gorie existe, on fait le tour des sous cat�gories */

		  ib = liste_tmp -> data;

		  node_sous_ib = node_detail -> childs;

		  while ( node_sous_ib )
		    {
		      struct struct_sous_imputation *sous_ib;
		      GSList *liste_tmp_2;

		      /* on ne prend la sous cat�g que si elle n'existe pas */

		      liste_tmp_2 = g_slist_find_custom ( ib -> liste_sous_imputation,
							  xmlGetProp ( node_sous_ib,
								       "Nom" ),
							  (GCompareFunc) recherche_sous_imputation_par_nom );

		      if ( !liste_tmp_2 )
			{

			  sous_ib = calloc ( 1,
						     sizeof ( struct struct_sous_imputation ) );

			  sous_ib -> no_sous_imputation = ++ib -> no_derniere_sous_imputation;

			  sous_ib -> nom_sous_imputation = xmlGetProp ( node_sous_ib,
									"Nom" );

			  ib -> liste_sous_imputation = g_slist_append ( ib -> liste_sous_imputation,
									     sous_ib );
			}
		      node_sous_ib = node_sous_ib -> next;
		    }
		}
	      else
		{
		  /* l'ib n'existe pas, on l'ajoute */

		      
		  ib = calloc ( 1,
						     sizeof ( struct struct_imputation ) );

		  ib -> no_imputation = ++no_derniere_imputation;
		  nb_enregistrements_imputations++;

		  ib -> nom_imputation = xmlGetProp ( node_detail,
						      "Nom" );
		  ib -> type_imputation = atoi ( xmlGetProp ( node_detail,
							      "Type" ));
		  ib -> no_derniere_sous_imputation = atoi ( xmlGetProp ( node_detail,
								  "No_derniere_sous_imputation" ));

		  /*  pour chaque ib, on recup�re les sous-ib */

		  ib -> liste_sous_imputation = NULL;
		  node_sous_ib = node_detail -> childs;

		  while ( node_sous_ib )
		    {
		      struct struct_sous_imputation *sous_ib;

		      sous_ib = calloc ( 1,
					 sizeof ( struct struct_sous_imputation ) );

		      sous_ib -> no_sous_imputation = atoi ( xmlGetProp ( node_sous_ib,
									  "No" ));
		      sous_ib -> nom_sous_imputation = xmlGetProp ( node_sous_ib,
								    "Nom" );

		      ib -> liste_sous_imputation = g_slist_append ( ib -> liste_sous_imputation,
								     sous_ib );
		      node_sous_ib = node_sous_ib -> next;
		    }

		  liste_struct_imputation = g_slist_append ( liste_struct_imputation,
							  ib );
		}
	      node_detail = node_detail -> next;
	    }
	}
      node = node -> next;
    }

  /* on lib�re la memoire */
  
  xmlFreeDoc ( doc );

  /* creation de la liste des ib pour le combofix */

  creation_liste_imputation_combofix ();
  mise_a_jour_imputation ();
  remplit_arbre_imputation ();


  modification_fichier ( TRUE );

  return ( TRUE );
}
/***********************************************************************************************************/
