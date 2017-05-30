/**
 * @file     main.c
 *
 * @author   GOURDON Arnaud
 *           PREVOST Theophile
 *
 * @brief    Programme principal de la station meteorologique.
 *           Gestion de l'affichage et des differents menus.
 */

#define MWINCLUDECOLORS
#include <graphics.h>
#include <nano-X.h>
#include <pthread.h>
#include <stdio.h>
#include "buttons.h"
#include "threads.h"
#include "menus.h"

int switchScreen(GR_WINDOW_ID w, GR_GC_ID gc, int * current_screen);


int switchScreen(GR_WINDOW_ID w, GR_GC_ID gc, int * current_screen){
  printf("Passage dans SS, CS = %d\n",*current_screen);
  switch(*current_screen){
    case 0:
      printf("coucou0\n");
        *current_screen = 1;
        affichage_menu_02(w,gc,g_archives_donnees_week,g_nb_archives_semaine);
        break;
    case 1:
            printf("coucou1\n");
        *current_screen = 2;
        affichage_menu_03(w,gc,g_archives_tendances, g_nb_archives_tendances);
        break;
    case 2:
            printf("coucou2\n");
        *current_screen = 0;
        affichage_current_data(w,gc,&g_donnees_capteurs,0);
        break;

    }
  printf("Passage dans SS2, CS = %d\n",*current_screen);
    return EXIT_SUCCESS;
}


int main(int ac,char **av)
{
    GR_WINDOW_ID w;
    GR_GC_ID gc;
    GR_EVENT event;
    //int current_screen = 0; // ajout
    //int button_action = 0;
    pthread_t th_boutons, th_capteurs, th_tendances;

    if (GrOpen() < 0)
    {
        printf("Can't open graphics\n");
        exit(1);
    }

    // Configuration de l'ecran
    w = GrNewWindow(1, 5, 5, 150, 180, 5, WHITE, BLACK);
    gc = GrNewGC();
    GrSetGCForeground(gc, BLACK);
    GrSetGCUseBackground(gc, GR_FALSE);
    //  GrSelectEvents(w, GR_EVENT_MASK_EXPOSURE | GR_EVENT_MASK_TIMEOUT);
    GrMapWindow(w);

    // Creation des threads
    pthread_create(&th_boutons,   NULL, &verifier_etat_boutons, NULL);
    pthread_create(&th_capteurs,  NULL, &maj_donnees_capteurs,  NULL);
    pthread_create(&th_tendances, NULL, &maj_tendances,         NULL);

    // Boucle principale
    //on affiche qdf même qqchose au début
    affichage_current_data(w,gc,&g_donnees_capteurs,0);

    while (!g_fin_programme){
      
      while(button_pressed){
	printf("--------------Une autre boucle\n");
	printf("et curr screen %d\n",current_screen);
        switch (g_etat_boutons)
        {
          case BUTTON_01:       
	    
	    GrGetNextEventTimeout(&event, 1000);
	    printf("---- Button 1\n");
            GrClearWindow(w, 0);
	    button_pressed = 0;
	    
            switchScreen(w,gc,&current_screen);
            break;

          case BUTTON_02:
	    button_pressed = 0;
	    
            GrGetNextEventTimeout(&event, 1000);
	    printf("----Button 2\n");
	    if (current_screen == 0){
	      // touner l'affichage
	      // affichage_menu_02(w, gc, &g_donnees_moyennes_capteurs);
	    }else{
	      //changer capteur en cours
            }
	    break;

          case BUTTON_03:
	    	printf("curr screen = %d\n",current_screen);
	    button_pressed = 0;
            GrGetNextEventTimeout(&event, 1000);

	    printf("----Button 3\n");
            if(current_screen==0){
	      printf("----Screen 1 -- next format temp\n");
	      //current_format_temp = next(current_format_temp, FORMAT_TEMP_KELVIN);
            }else{
	      //changer l'unité du capteur en cours
            }
	    break;

          case BUTTON_04:       
	    button_pressed = 0;
	    
            GrGetNextEventTimeout(&event, 3000);
            /*switch (event.type)
            {*/
              //case GR_EVENT_TYPE_EXPOSURE:
                //affichage_menu_04(w, gc);
                //break;
              /*case GR_EVENT_TYPE_TIMEOUT:
                g_fin_programme = 1;
                break;*/
		//}
            break;
        }
      }
        
    }

    // Destruction des ressources
    GrClose();

    // Attente de la fin des threads
    pthread_join(th_capteurs,  NULL);
    pthread_join(th_boutons,   NULL);
    pthread_join(th_tendances, NULL);
    
    printf("Fin du programme\n");

    return 0;

} 




