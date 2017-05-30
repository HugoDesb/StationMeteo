/**
 * @file     threads.c
 *
 * @author   GOURDON Arnaud
 *           PREVOST Theophile
 *
 * @brief    Modules contenant differents threads.
 */

#include "buttons.h"
#include "threads.h"



// Initialisation des variables globales
volatile int button_pressed = 0;
volatile int g_etat_boutons = BUTTON_01;
volatile int g_fin_programme = 0;
volatile t_tendances g_tendances;
volatile t_captors_data g_donnees_capteurs = { 0, 0, 0 };
volatile t_captors_data g_donnees_moyennes_capteurs = { 0, 0, 0 };

static t_captors_data g_dernieres_mesures_tendances = { 0, 0, 0 };
t_captors_data g_archives_donnees_heure[NB_MAX_ARCHIVES_HEURE];  //
t_captors_data g_archives_donnees_week[NB_MAX_ARCHIVES_SEMAINE]; //
t_captors_data g_archives_tendances[NB_MAX_ARCHIVES_HEURE-1];    //
static int g_id_archive = 0;
static int g_nb_archives_heure = 0;     //~
int g_nb_archives_semaine = 0;   //
int g_nb_archives_tendances = 0; //


void archiver_donnees_capteurs(void)
{
  int i;
  double avgT=0, avgH=0, avgP=0;
  t_captors_data g_donnees_capteurs_AVG;
  g_archives_donnees_heure[g_id_archive] = g_donnees_capteurs;
  g_id_archive = (g_id_archive + 1) % NB_MAX_ARCHIVES_HEURE;

  if (g_nb_archives_heure < NB_MAX_ARCHIVES_HEURE){
    g_nb_archives_heure++;
  }
    

  // if g_archives_donnees_heure is full, the hour is done
  if(g_id_archive == 0 ){
    //avg hour
    for(i = 0; i<NB_MAX_ARCHIVES_HEURE;i++){
      avgH += g_archives_donnees_heure[i].RH;
      avgT += g_archives_donnees_heure[i].T;
      avgP += g_archives_donnees_heure[i].P;

      //empty it
      g_archives_donnees_heure[i].RH = 0;
      g_archives_donnees_heure[i].P = 0;
      g_archives_donnees_heure[i].T = 0;
    }
    g_donnees_capteurs_AVG.RH = avgH/g_nb_archives_heure;
    g_donnees_capteurs_AVG.P = avgP/g_nb_archives_heure;
    g_donnees_capteurs_AVG.T = avgT/g_nb_archives_heure;

    //store in g_archives_donnes_week
    if(g_nb_archives_semaine==0){
      //if week empty, just add
      g_archives_donnees_week[g_nb_archives_semaine] = g_donnees_capteurs_AVG;
    }else{
      //shift all items to the right
      for(i=g_nb_archives_semaine-1;i>=0;i--){
	//if at the end of the array, do nothing and lose this data
	if(!(g_nb_archives_semaine>=NB_MAX_ARCHIVES_SEMAINE-1)){
	  g_archives_donnees_week[i+1] = g_archives_donnees_week[i];
	}
      }
      //store
      g_archives_donnees_week[0] = g_donnees_capteurs_AVG;
    }

    
    
    //reset
    if(g_nb_archives_semaine<NB_MAX_ARCHIVES_SEMAINE){
      g_nb_archives_semaine++;
    }
    g_nb_archives_heure = 0;

  }
    
  
} // archiver_donnees_capteurs

void calculer_moyennes(void)
{
    int i;

    g_donnees_moyennes_capteurs.T = 0;
    g_donnees_moyennes_capteurs.P = 0;
    g_donnees_moyennes_capteurs.RH = 0;

    for (i = 0; i < g_nb_archives_heure; ++i)
    {
        g_donnees_moyennes_capteurs.T  += g_archives_donnees_heure[i].T;
        g_donnees_moyennes_capteurs.P  += g_archives_donnees_heure[i].P;
        g_donnees_moyennes_capteurs.RH += g_archives_donnees_heure[i].RH;
    }

    g_donnees_moyennes_capteurs.T /= g_nb_archives_heure;
    g_donnees_moyennes_capteurs.P /= g_nb_archives_heure;
    g_donnees_moyennes_capteurs.RH /= g_nb_archives_heure;

} // calculer_moyennes

void * verifier_etat_boutons(void * arg)
{
    KEYBOARD_STATUS KbStatus;

    while (!g_fin_programme)
    {
        KbStatus = keyboard_status();

        if((KbStatus & BUTTON_01) == BUTTON_01) 
        {
            g_etat_boutons = BUTTON_01;
            button_pressed=1;
            //while(keyboard_status() == KbStatus);
        }
        else if((KbStatus & BUTTON_02) == BUTTON_02) 
        {
            g_etat_boutons = BUTTON_02;
            button_pressed=1;
            //while(keyboard_status() == KbStatus);
        }
        else if((KbStatus & BUTTON_03) == BUTTON_03) 
        {
            g_etat_boutons = BUTTON_03;
            button_pressed=1;
            //while(keyboard_status() == KbStatus);
        }
        else if((KbStatus & BUTTON_04)== BUTTON_04) 
        {
            g_etat_boutons = BUTTON_04;
            button_pressed=1;
            //while(keyboard_status() == KbStatus);
        }

        usleep(10000); 
    }

} // verifier_etat_boutons

void * maj_donnees_capteurs(void * arg)
{
    int i, r;
    
    for (i = 0; i < NB_MAX_ARCHIVES_HEURE; ++i)
    {
        g_archives_donnees_heure[i].T  = 0;
        g_archives_donnees_heure[i].P  = 0;
        g_archives_donnees_heure[i].RH = 0;
    }

    while (!g_fin_programme)
    {
        r = lire_donnees_capteurs(&g_donnees_capteurs);
        if (r == EXIT_SUCCESS)
        {
            archiver_donnees_capteurs();
            calculer_moyennes();
            sleep(INTERVAL_MAJ);
        }
        else
        {
            g_fin_programme = 1;
        }
    }

} // maj_donnees_capteurs

void * maj_tendances(void * arg)
{
  int i;
  t_captors_data g_tendance;
    while (!g_fin_programme)
    {
        sleep(INTERVAL_TENDANCES);
	
	g_nb_archives_tendances = 0;
	for(i=1; i<g_nb_archives_semaine;i++){
	  g_tendance.T = g_archives_donnees_week[i].T - g_archives_donnees_week[i-1].T;
	  g_tendance.RH = g_archives_donnees_week[i].RH - g_archives_donnees_week[i-1].RH;
	  g_tendance.P = g_archives_donnees_week[i].P - g_archives_donnees_week[i-1].P;
	  g_archives_tendances[i-1] = g_tendance;
	  g_nb_archives_tendances++;
	}
	
	/*
        if (g_donnees_capteurs.T > g_dernieres_mesures_tendances.T)
            g_tendances.T = 1;
        else
            g_tendances.T = -1;

        if (g_donnees_capteurs.P > g_dernieres_mesures_tendances.P)
            g_tendances.P = 1;
        else
            g_tendances.P = -1;

        if (g_donnees_capteurs.RH > g_dernieres_mesures_tendances.RH)
            g_tendances.RH = 1;
        else
            g_tendances.RH = -1;

        g_dernieres_mesures_tendances = g_donnees_capteurs;
	*/
    }

} // maj_tendances

