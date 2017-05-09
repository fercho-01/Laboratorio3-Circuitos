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


//  ----------- Estados ------------


bool estadoA = false;                     // variables que indican si estoy en la pantalla del boton
bool estadoB = false;                     // del menu seleccionado
bool estadoC = false;
bool estadoD = false;


//  ----------- variables globales ------------


String mensaje = "";
String codigoIngresado = "";
int longitud = 0;                         // longitud del mensaje

String codigo_1 = "";
String codigo_2 = "";

bool alarmaActiva = false;
bool garajeCerrado = true;


// ----------- contraseñas Iniciales ------------


bool primeraIteracion = true;
bool configuracionCompleta = false;       

bool codigoAlarma = false;
bool codigoApertura = false;


// ----------- Mensajes del Sistema ------------


String mensajeDeInsersion = "codigo Alarma:";
String mensajeDeInsersion1 = "codigo Garage:";

String alertaAlarma = "Alarma Activa !";
String alertaAlarma1= "Alarma Des. !";

String alertaGaraje =  "Garage Abierto";
String alertaGaraje1 =  "Garage Cerrado";

String notificacion = "Accion no Disp.";
String codigoErroneo = "no match";

String alarmaPrendida = "A: ON";        // Alarma prendida
String alarmaApagada = "A: OFF";

String GarageCerrado =  "G: CSD";       // Garage cerrado
String GarageAbierto =  "G: OPN";


// ----------- notificaciones ------------


unsigned int holdTime = 3000;              // 3 segundos para enviar el codigo completo
unsigned long tiempoActual = 0;
unsigned long tiempoInicial = 0;
unsigned long tiempoDeDespliegue = 3000;    // 3 segundos

bool notaPorDesplegar = false; 
String alerta = "";

// Teclado



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
   keypad.setHoldTime(holdTime);
       
    lcd.begin(16, 2);      
    setMsg("_");
}

void loop() {


  char key = keypad.getKey();
  
  inicializar();
  
  statusDelSistema();
  statusDelCodigoIngresado();

  desplegar();
  
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

  //Serial.println(key);
  switch (keypad.getState()){
    
    case PRESSED:
    if(key=='#'){

   
        if(contador>-1){
            int fila= teclaAnterior -'0';
            mensaje += Letras[fila][contador];
            String miLetra = Letras[fila][contador];
            contador=-1;
            setMsg(miLetra);
            posicion++;
            setMsg("_");

            Serial.println(mensaje);
          }
        }
        else if(contador==-1  && key != 'A' && key != 'B'       // no guarde las variables del menu
                  && key != 'C' && key != 'D'){
          
          contador++;
          teclaAnterior = key;
          int fila= teclaAnterior -'0';
          String miLetra = Letras[fila][contador];
          setMsg(miLetra);
        
        }else if(contador>=0){
          if(teclaAnterior == key){
            contador = (contador+1)%4;
            teclaAnterior = key;
            int fila= teclaAnterior -'0';
            String miLetra = Letras[fila][contador];
            setMsg(miLetra);
          }
        }

    if(key=='*'){
      
      setMsg(" ");          
      posicion--;

      // falta borrar del mensaje la letra 
      
      if(posicion<0){
        posicion=0;
      }
      
      setMsg("_");
      contador=-1;
      
    }

   
   if(key == 'A'){

      limpiarBanner();
      activarAlarma();
      
   }    
    break;

// ------------------------------------
    case HOLD:

    if(key == '#'){
      
      codigoIngresado = mensaje;        //guarde en una variable global lo que haya en mensaje 
      mensaje = "";
      
      limpiarFilaDeInsercion();
    }

    break;
  }
}  



void activarAlarma(){
 
  if(!alarmaActiva){

    setBanner(mensajeDeInsersion, 0);    
      
    estadoA = true;
    
  } else {

      // mensaje con delay
      
      // setBanner(notificacion,0);          
  }  
}



/*  */

void statusDelCodigoIngresado(){            // metodo que evalua el codigo dependiedo del estado (boton seleccionado del menu)

 String vacio = "";

 if(!codigoIngresado.equals(vacio)){


  if(estadoA){

    if(codigoIngresado.equals(codigo_1)){

      alarmaActiva = true;                  // si el codigo ingresado es igual al codigo_1, activo la alarma.
      estadoA = false;                      // reinicie al estado original y resuma ejecucion principal (StatusDelSistema). 

      mensaje = "";                         // reinicie otra vez las variables para insercion posterior
      codigoIngresado = "";
      
      limpiarBanner();

    } else {

      mensaje = "";                         // reinicie otra vez las variables para insercion posterior
      codigoIngresado = "";

      alerta = codigoErroneo;
      //Serial.println("las contraseñas no son iguales");
      //mensaje con delay
    }
  }


  if(estadoB){

    if(codigoIngresado.equals(codigo_1)){

      alarmaActiva = false;                  // si el codigo ingresado es igual al codigo_1, desactivo la alarma.
      estadoB = false;
      
       //mensaje con delay

    } else {


        //mensaje con delay
    }
  }


  // replique el mismo procedimiento para el estadoC y el estadoD
  
 } 
  
}


void inicializar(){

if(!configuracionCompleta){
  

  if(!codigoAlarma){                      // configure el codigo de la alarma
    
    setBanner(mensajeDeInsersion, 0);    
    
    longitud = mensaje.length();

    if(longitud == 6){                    // el codigo tiene una longitud de 6 caracteres         

      codigo_1 = mensaje;                 // guarde el mensaje en otra posicion global
     
      mensaje = "";
      limpiarFilaDeInsercion();              // reinicie la posicion y elimine lo escrito
      setMsg("_");  

      // añadir sonido o un mensaje nuevo

      codigoAlarma = true;
    }
  }


  if(!codigoApertura && codigoAlarma){            // luego de ingresar el codigo de alarma, configure la apertura


    setBanner(mensajeDeInsersion1, 0);
    longitud = mensaje.length();

    if(longitud == 6){

    codigo_2 = mensaje;

    mensaje = "";      
    limpiarFilaDeInsercion();                     // reinicie la posicion y elimine lo escrito
    setMsg("_");    

    // añadir sonido o un mensaje nuevo
    
    codigoApertura = true;
    configuracionCompleta = true;    
    
    limpiarBanner();
    }    
   }

  }
}


void statusDelSistema(){                              // visualizacion en pantalla del estado actual de la alarma y el garage
  
  if(configuracionCompleta && !estadoA && !estadoB && !estadoC && !estadoD){     

    if(alarmaActiva){

      setBanner(alarmaPrendida, 0);
    } else {

      setBanner(alarmaApagada, 0);
    }
    


    if(garajeCerrado){

      setBanner(GarageCerrado,10);
    } else {

      setBanner(GarageAbierto, 10);
    }
    
  }
}


void desplegar(){

  String vacio = "";

  if (!alerta.equals(vacio)){                       //  ¿ existe una alerta por mostrar ?

    if(!notaPorDesplegar){                          // ¿ es la primera vez en la que vamos a guardar el tiempo?

    tiempoInicial = millis();                       // si entramos la primera vez al metodo, guarde el valor inicial
    notaPorDesplegar = true;
    
    limpiarBanner();
    limpiarFilaDeInsercion();

  
  } else {
    
    tiempoActual= millis();
    if(tiempoActual - tiempoInicial > tiempoDeDespliegue ){     // ¿ ya pasaron los tres segundos ?

          setMsg(alerta);
          
    }else{

          alerta = "";
          notaPorDesplegar = false;
          limpiarBanner();
    }
   }    
  }    
}
  

void setMsg(String msg){                          // imprima el mensaje en la fila 1, en la posicion de la variable global "posicion"
  lcd.setCursor(posicion, 1);
  lcd.print(msg);
}


void setBanner(String msg, int pos){              // imprima el mensaje en la fila 0, posicion "pos"
 lcd.setCursor(pos, 0);
 lcd.print(msg); 
}


void limpiarFilaDeInsercion(){                     // elimine todo lo que haya en la fila 1. UTILIZA UNA VARIABLE GLOBAL

  for(posicion = 0; posicion < 16; posicion++){
    
    setMsg(" ");
  }
  
  posicion = 0;
}

void limpiarBanner(){

  for(int index = 0; index < 16; index++){          // elimine todo lo que haya en la fila 0

    setBanner(" ", index);
  }

}
