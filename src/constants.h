#ifndef _CONSTANTS_H
#define _CONSTANTS_H 1
/* Fichier de d�finition de constantes constant.h */

/* Constantes qui d�finisent le num�ro de colonne dans la liste des op�rations
   dans l'onglet op�rations */
#define TRANSACTION_COL_NB_CHECK 0
#define TRANSACTION_COL_NB_DATE 1
#define TRANSACTION_COL_NB_PARTY 2
#define TRANSACTION_COL_NB_PR 3
#define TRANSACTION_COL_NB_DEBIT 4
#define TRANSACTION_COL_NB_CREDIT 5
#define TRANSACTION_COL_NB_BALANCE 6
#define TRANSACTION_LIST_COL_NB 7	 /* must be the last of the list, defini aussi dans structures.h */

#define TRANSACTION_LIST_ROWS_NB 4

/* Constantes qui d�finisent le num�ro associ� au param�tre
 * dans le formulaire */
# define TRANSACTION_FORM_DATE 1
# define TRANSACTION_FORM_DEBIT 2
# define TRANSACTION_FORM_CREDIT 3
# define TRANSACTION_FORM_VALUE_DATE 4
# define TRANSACTION_FORM_EXERCICE 5
# define TRANSACTION_FORM_PARTY 6
# define TRANSACTION_FORM_CATEGORY 7
# define TRANSACTION_FORM_BREAKDOWN 8
# define TRANSACTION_FORM_BUDGET 9
# define TRANSACTION_FORM_NOTES 10
# define TRANSACTION_FORM_TYPE 11
# define TRANSACTION_FORM_CHEQUE 12
# define TRANSACTION_FORM_DEVISE 13
# define TRANSACTION_FORM_CHANGE 14
# define TRANSACTION_FORM_VOUCHER 15
# define TRANSACTION_FORM_BANK 16
# define TRANSACTION_FORM_CONTRA 17
# define TRANSACTION_FORM_OP_NB 18
# define TRANSACTION_FORM_MODE 19
# define TRANSACTION_FORM_WIDGET_NB 19

/* constantes qui d�finissent le no associ� au param�tre
 * dans la liste d'op� */
# define TRANSACTION_LIST_DATE 1
# define TRANSACTION_LIST_VALUE_DATE 2
# define TRANSACTION_LIST_PARTY 3
# define TRANSACTION_LIST_BUDGET 4
# define TRANSACTION_LIST_CREDIT 5
# define TRANSACTION_LIST_DEBIT 6
# define TRANSACTION_LIST_BALANCE 7
# define TRANSACTION_LIST_AMOUNT 8
# define TRANSACTION_LIST_TYPE 9
# define TRANSACTION_LIST_RECONCILE_NB 10
# define TRANSACTION_LIST_EXERCICE 11
# define TRANSACTION_LIST_CATEGORY 12
# define TRANSACTION_LIST_MARK 13
# define TRANSACTION_LIST_VOUCHER 14 
# define TRANSACTION_LIST_NOTES 15
# define TRANSACTION_LIST_BANK 16
# define TRANSACTION_LIST_NO 17
# define TRANSACTION_LIST_CHQ 18



/* Constantes qui d�finisent le num�ro de widget dans le formulaire
   de ventilation des op�rations */
# define TRANSACTION_BREAKDOWN_FORM_CATEGORY 0
# define TRANSACTION_BREAKDOWN_FORM_NOTES 1
# define TRANSACTION_BREAKDOWN_FORM_DEBIT 2
# define TRANSACTION_BREAKDOWN_FORM_CREDIT 3
# define TRANSACTION_BREAKDOWN_FORM_BUDGETARY 4
# define TRANSACTION_BREAKDOWN_FORM_CONTRA 5
# define TRANSACTION_BREAKDOWN_FORM_EXERCICE 6
# define TRANSACTION_BREAKDOWN_FORM_VOUCHER 7
# define TRANSACTION_BREAKDOWN_FORM_TOTAL_WIDGET 8	 /* must be the last of the list */

/* Constantes qui d�finisent le num�ro de widget dans le formulaire
   de saisie des �ch�ances */
# define SCHEDULER_FORM_DATE 0
# define SCHEDULER_FORM_PARTY 1
# define SCHEDULER_FORM_DEBIT 2
# define SCHEDULER_FORM_CREDIT 3
# define SCHEDULER_FORM_DEVISE 4
# define SCHEDULER_FORM_ACCOUNT 5
# define SCHEDULER_FORM_CATEGORY 6
# define SCHEDULER_FORM_TYPE 7
# define SCHEDULER_FORM_CHEQUE 8
# define SCHEDULER_FORM_EXERCICE 9
# define SCHEDULER_FORM_BUDGETARY 10
# define SCHEDULER_FORM_BREAKDOWN 11
# define SCHEDULER_FORM_VOUCHER 12
# define SCHEDULER_FORM_MODE 13
# define SCHEDULER_FORM_NOTES 14
# define SCHEDULER_FORM_FREQUENCY 15
# define SCHEDULER_FORM_FINAL_DATE 16
# define SCHEDULER_FORM_FREQ_CUSTOM_NB 17
# define SCHEDULER_FORM_FREQ_CUSTOM_MENU 18
# define SCHEDULER_FORM_TOTAL_WIDGET 19	 /* must be the last of the list */

/* Constantes qui d�finisent le num�ro de widget dans le formulaire
   de ventilation des op�rations planifi�es */
# define SCHEDULER_BREAKDOWN_FORM_CATEGORY 0
# define SCHEDULER_BREAKDOWN_FORM_NOTES 1
# define SCHEDULER_BREAKDOWN_FORM_DEBIT 2
# define SCHEDULER_BREAKDOWN_FORM_CREDIT 3
# define SCHEDULER_BREAKDOWN_FORM_BUDGETARY 4
# define SCHEDULER_BREAKDOWN_FORM_CONTRA 5
# define SCHEDULER_BREAKDOWN_FORM_EXERCICE 6
# define SCHEDULER_BREAKDOWN_FORM_VOUCHER 7
# define SCHEDULER_BREAKDOWN_FORM_TOTAL_WIDGET 8	 /* must be the last of the list */

/* Constantes pour style_entree_formulaire[] */
# define ENCLAIR 0
# define ENGRIS 1

/* Constantes pour la fonction gtk_misc_set_alignment() (GTK1) */
#define MISC_TOP 0
#define MISC_VERT_CENTER 0.5
#define MISC_BOTTOM 1
#define MISC_LEFT 0
#define MISC_HORIZ_CENTER 0.5
#define MISC_RIGHT 1

/* Constantes pour la fonction gtk_tree_view_column_set_alignment() (GTK2) */
#define COLUMN_LEFT 0.0
#define COLUMN_CENTER 0.5
#define COLUMN_RIGHT 1.0

#ifndef _WIN32
#define C_DIRECTORY_SEPARATOR "/"
#define C_GRISBIRC  "/.grisbirc"
#else
/* Some old Windows version have difficulties with dat starting file names */
#define C_DIRECTORY_SEPARATOR "\\"
#define C_GRISBIRC  "\\grisbi.rc"
#endif

/* quelques chaines de configuration*/
#define ETAT_WWW_BROWSER "www-browser"

#define COL_NB_DATE 0
#define COL_NB_ACCOUNT 1
#define COL_NB_PARTY 2
#define COL_NB_FREQUENCY 3
#define COL_NB_MODE 4
#define COL_NB_NOTES 5
#define COL_NB_AMOUNT 6		/* doit �tre le dernier de la liste
				   � cause de plusieurs boucles for */
#define NB_COLS_SCHEDULER 7

#endif // !_CONSTANTS_H
