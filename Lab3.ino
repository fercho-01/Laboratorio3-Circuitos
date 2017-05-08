#include <Keypad.h>
#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;
const byte Filas = 4; 
const byte Cols = 4;  
int posicion = 0;
char teclaAnterior;
int contador = -1;

byte Pins_Filas[] = {9, 10, 11, 6};     
byte Pins_Cols[] = { 5, 4, 3, 2};
String mensaje = "";

char Teclas [ Filas ][ Cols ] =
    {
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'}
     };

String Letras [ 10][ 4 ] =
    {
        {"0","0","0","0"},
        {"1","1","1","1"},
        {"2","A","B","C"},
        {"3","D","E","F"},
        {"4","G","H","I"},
        {"5","J","K","L"},
        {"6","M","N","O"},
        {"7","P","R","S"},
        {"8","T","U","V"},
        {"9","W","X","Y"}
     };


 Keypad keypad = Keypad(makeKeymap(Teclas), Pins_Filas, Pins_Cols, Filas, Cols);


void setup() {
   Serial.begin(9600) ;
   keypad.addEventListener(keypadEvent); //add an event listener for this keypad
       // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
        // Print a message to the LCD.
    //setMsg("Helo world");
    setMsg("_",posicion);
}

void loop() {


  char key = keypad.getKey();
  /*
  char pulsacion = Teclado1.getKey() ;
                if (pulsacion != 0) {             // Si el valor es 0 es que no se
                      lcd.setCursor(posicion, 1);
                      posicion=(1+posicion)%6;
                      mensaje += pulsacion;
                      Serial.println(mensaje);   // se ha pulsado ninguna tecla
                      setMsg(mensaje);
                    
                }
    */            

}


void keypadEvent(KeypadEvent key){
  switch (keypad.getState()){
    case RELEASED:
    if(key=='#'){
          if(contador>-1){
            int fila= teclaAnterior -'0';
            mensaje += Letras[fila][contador];
            String miLetra = Letras[fila][contador];
            contador=-1;
            setMsg(miLetra,posicion);
            posicion++;
            setMsg("_",posicion);
          }
        }
        else if(contador==-1){
          contador++;
          teclaAnterior = key;
          int fila= teclaAnterior -'0';
          String miLetra = Letras[fila][contador];
          setMsg(miLetra,posicion);
        }else if(contador>=0){
          if(teclaAnterior == key){
            contador = (contador+1)%4;
            teclaAnterior = key;
            int fila= teclaAnterior -'0';
            String miLetra = Letras[fila][contador];
            setMsg(miLetra,posicion);
          }
        }

        if(key=='*'){
          setMsg(" ",posicion);
          posicion--;
          if(posicion<0){
            posicion=0;
          }
          setMsg("_",posicion);
          contador=-1;
          
        }
        break;
  }
}  
void setMsg(String msg, int pos){
  lcd.setCursor(posicion, 1);
  lcd.print(msg);
}


