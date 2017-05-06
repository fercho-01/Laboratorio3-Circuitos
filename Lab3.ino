#include <Keypad.h>
#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;
const byte Filas = 4; 
const byte Cols = 4;  
int posicion = 0;

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

 Keypad Teclado1 = Keypad(makeKeymap(Teclas), Pins_Filas, Pins_Cols, Filas, Cols);


void setup() {
   Serial.begin(9600) ;
       // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
        // Print a message to the LCD.
    

}

void loop() {

  char pulsacion = Teclado1.getKey() ;
                if (pulsacion != 0) {             // Si el valor es 0 es que no se
                      lcd.setCursor(posicion, 1);
                      lcd.print("*");
                      posicion=(1+posicion)%6;
                      mensaje += pulsacion;
                    Serial.println(mensaje);   // se ha pulsado ninguna tecla
                    
                    setMsg(pulsacion);
                    
                }

}

void setMsg(String msg){
  lcd.print(msg);
}

