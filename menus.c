
#include "capteurs.h"
#include "menus.h"


extern volatile int current_screen = 0;
extern volatile int current_data = 0;
extern volatile int current_format_temp = 0;
extern volatile int current_format_pressure = 0;
extern volatile int current_orientation = 0;


int affichage_current_data(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_captors_data p, int orientation){

  char sT[64], sP[64], sRH[64];
  
    if(orientation == 0){

      get_Temp(sT,p);
      get_Pressure(sP,p);
      sprintf(sRH, "%.1f %%\n",  p->RH);

      GrText(w, gc, 20, 30, "Temps reel",  -1, GR_TFASCII);
      GrText(w, gc, 35, 80,  sT,  -1, GR_TFASCII);
      GrText(w, gc, 35, 110, sP,  -1, GR_TFASCII);
      GrText(w, gc, 35, 140, sRH, -1, GR_TFASCII);
    }else if(orientation == 1) {
      //tourne d'un tour
    }else{
      //encore un
    }
   
    
    return EXIT_SUCCESS;

} // affichage_menu_01

int affichage_menu_02(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_captors_data p)
{
    char sT[64], sP[64], sRH[64];

    sprintf(sT,  "T = %.2f C\n",   p->T);
    sprintf(sP,  "P = %.2f hPa\n", p->P);
    sprintf(sRH, "H = %.2f %%\n",  p->RH);

    GrText(w, gc, 20, 30, "Moyennes",  -1, GR_TFASCII);
    GrText(w, gc, 35, 80,  sT,  -1, GR_TFASCII);
    GrText(w, gc, 35, 110, sP,  -1, GR_TFASCII);
    GrText(w, gc, 35, 140, sRH, -1, GR_TFASCII);
    
    return EXIT_SUCCESS;

} // affichage_menu_02

int affichage_menu_03(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_tendances p)
{
    GrText(w, gc, 20, 30, "Tendances", -1, GR_TFASCII);

    GrText(w, gc, 35, 80,  "T =", -1, GR_TFASCII);
    GrText(w, gc, 35, 110, "P =", -1, GR_TFASCII);
    GrText(w, gc, 35, 140, "H =", -1, GR_TFASCII);
    
    if (p->T > 0)
        GrDrawImageFromFile(w, gc, 60, 60, 30, 30, IMG_UP, 0);
    else
        GrDrawImageFromFile(w, gc, 60, 60, 30, 30, IMG_DOWN, 0);
    if (p->P > 0)
        GrDrawImageFromFile(w, gc, 60, 90, 30, 30, IMG_UP, 0);
    else
        GrDrawImageFromFile(w, gc, 60, 90, 30, 30, IMG_DOWN, 0);
    if (p->RH > 0)
        GrDrawImageFromFile(w, gc, 60, 120, 30, 30, IMG_UP, 0);
    else
        GrDrawImageFromFile(w, gc, 60, 120, 30, 30, IMG_DOWN, 0);

    return EXIT_SUCCESS;

} // affichage_menu_03

void get_Pressure(char data[64], t_ptr_captors_data p){

  float val = p->P; char t[64];int i = 0;
  if(current_format_pressure == FORMAT_PRESSURE_BAR){
    val *= 0.001;
    sprintf(t, "%.1f bar\n",val);
    
  }else{
    sprintf(t, "%.1f hPa\n",val);
  }
  for(i = 0;i<64;i++){
    data[i] = t[i];
  }
}

void get_Temp(char data[64], t_ptr_captors_data p){

  float val = p->T;
  char t[64];
  int i = 0;
  if(current_format_temp == FORMAT_TEMP_CELSIUS){
    sprintf(t, "%.1f C\n",val);
  }else if(current_format_temp == FORMAT_TEMP_FAHRENHEIT){
    val = val*1.8+32;
    sprintf(t, "%.1f F\n",val);
  }else{
    val = val + 273.15;
    sprintf(t, "%.1f K\n",val);
  }
  
  for(i = 0;i<64;i++){
    data[i] = t[i];
  }
}


int next(int current, int max){
  if(current == max){
    return 0;
  }else{
    return current + 1;
  }
}


