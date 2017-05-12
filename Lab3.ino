#include <Keypad.h>
#include <Wire.h>
#include "rgb_lcd.h"
#include <Servo.h>

rgb_lcd lcd;
const byte Filas = 4; 
const byte Cols = 4;  

int posicion = 0;
char teclaAnterior;
int contador = -1;
bool estoyMenuPrincipal = false;
byte Pins_Filas[] = {9, 10, 11, 6};     
byte Pins_Cols[] = { 5, 4, 3, 2};
const int pinTouch = 8;
const int pinServo = 7;
Servo myServo;
//  ----------- Estados ------------


bool estadoA = false;                     // variables que indican si estoy en la pantalla del boton
bool estadoB = false;                     // del menu seleccionado
bool estadoC = false;
bool estadoD = false;

int contadorEstadoA = 0;
int contadorEstadoB = 0;
int contadorEstadoC = 0;
int contadorEstadoD = 0;

int errores = 0;
bool reinicioDelSistema = false;

//  ----------- variables globales ------------


String mensaje = "";
String codigoIngresado = "";
int longitud = 0;                         // longitud del mensaje

String codigo_1 = "";
String codigo_2 = "";
String codigoMaestro = "2";

bool alarmaActiva = false;
bool garajeCerrado = true;

int estadoDelSuiche = 0;
bool puertaCerrada;                   

// ----------- contraseñas Iniciales ------------


bool primeraIteracion = true;
bool configuracionCompleta = false;       

bool codigoAlarma = false;
bool codigoApertura = false;


// ----------- Mensajes del Sistema ------------


String mensajeDeInsersion = "C. Alarma:";
String mensajeDeInsersion1 = "C. Garage:";

String notificacion = "Accion no Disp.";
String codigoErroneo = "- Error -";
String reinicio = "C. Maestro";

String alarmaPrendida = "A: ON";        // Alarma prendida
String alarmaApagada = "A: OFF";

String GarageCerrado =  "G: CSD";       // Garage cerrado
String GarageAbierto =  "G: OPN";

String PuertaCerrada = "D: CSD" ;        // puerta cerrada 
String PuertaAbierta = "D: OPN";


// ----------- notificaciones ------------


unsigned int holdTime = 2000;              // 3 segundos para enviar el codigo completo
unsigned long tiempoActual = 0;
unsigned long tiempoInicial = 0;
unsigned long tiempoDeDespliegue = 1500;    // 3 segundos

bool notaPorDesplegar = false; 

String alerta = "";
String vacio = "";

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

String inputString = "";
boolean stringComplete = false;
String serialPass = "123456";

void setup() {
   Serial.begin(9600) ;
   keypad.addEventListener(keypadEvent); //add an event listener for this keypad
   keypad.setHoldTime(holdTime);
       
    lcd.begin(16, 2); 
    lcd.setRGB(200, 200, 100);
    setMsg("_");
    
}

void loop() {

  char key = keypad.getKey();
  
  inicializar();
  
  statusDelSistema();
  estadoDeSensores();
  statusDelCodigoIngresado();

  desplegar(); 

  if(stringComplete){
    Serial.println(inputString);
    stringComplete = false;
    String comando = inputString.substring(0,2);
    String pass = inputString.substring(3,9);
     String newPass = "";
    if(inputString.length()>10){
      newPass = inputString.substring(10,16);
    }
    
    if(configuracionCompleta){
      if(pass.equals(serialPass)){
        if(comando.equals("OG")){
          //Abrir garaje
          if(garajeCerrado == true){
            controlarServo(180,true);
            garajeCerrado = false;
          }else{
            Serial.println("El garaje esta abierto");
          }
        }else if(comando.equals("CG")){
          //Cerrar garaje
          if(garajeCerrado == false){
            controlarServo(180,false);
            garajeCerrado = true;
          }else{
            Serial.println("El garaje esta cerrado");
          }
        }else if(comando.equals("EA")){
          //Activar alarma
          if(!alarmaActiva){
            estadoA = true;
            alarmaActiva = true;
          }else{
            Serial.println("La alarma esta activa");
          }
        }else if(comando.equals("DA")){
          //Desactivar alarma
          if(alarmaActiva){
            estadoA = false;
            alarmaActiva = false;
          }else{
            Serial.println("La alarma esta desactivada");
          }
        }else if(comando.equals("MG")){
            //Cambiar contraseña del garaje
            if(!newPass.equals("") && newPass.length()==6){
              codigo_2 = newPass;
              Serial.println("Contraseña del garaje cambiada");
            }else{
              Serial.println("Contraseña invalida");
            }
        }else if(comando.equals("MA")){
          //Cambiar contraseña de la alarma
          if(!newPass.equals("") && newPass.length()==6){
              codigo_1 = newPass;
              Serial.println("Contraseña de la alarma cambiada");
          }else{
              Serial.println("Contraseña invalida");
          }
        }else if(comando.equals("MS")){
          if(!newPass.equals("") && newPass.length()==6){
              serialPass = newPass;
              Serial.println("Contraseña del serial cambiada");
          }else{
              Serial.println("Contraseña invalida");
          }
        }else if(comando.equals("RT")){
          //Reiniciar el sistema
          if(!newPass.equals("") && newPass.length()==6){
              if(newPass.equals(codigoMaestro)){
                reinicioDelSistema = false;                  // si el codigo ingresado es igual al codigo_1, desactivo la alarma.
                configuracionCompleta = false;
        
                mensaje = "";                           // reinicie otra vez las variables para insercion posterior
                codigoIngresado = "";

                codigoAlarma = false;
                codigoApertura = false;
           
                limpiarBanner();
              }else{
                Serial.println("Error codigo invalido");
              }
          }else{
              Serial.println("Contraseña invalida");
          }
        }
      }else{
        Serial.println("Contraseña del serial invalida");
      }
    }else{
      Serial.println("Por favor inicie el sistema");
    }
    inputString = "";
  }
}


void keypadEvent(KeypadEvent key){

  //Serial.println(key);
  switch (keypad.getState()){
    
    case PRESSED:
    
    if(!estoyMenuPrincipal){
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
        else if(contador==-1 &&key!='*' && key != 'A' && key != 'B'       // no guarde las variables del menu
                  && key != 'C' && key != 'D' ){

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
          }else{
            teclaAnterior=key;
            contador=0;
            int fila= teclaAnterior -'0';
            String miLetra = Letras[fila][contador];
            setMsg(miLetra);
          }
        }

    if(key=='*'){
      Serial.println(contador);
      //Borra en una posicion anterior
      if(contador==-1){
        mensaje = mensaje.substring(0,mensaje.length()-1);
        Serial.println(mensaje);
        setMsg(" ");          
        posicion--;
  
        // falta borrar del mensaje la letra 
        
        if(posicion<0){
          posicion=0;
        }
        
        setMsg("_");
        contador=-1;
      }else{
        //Borra en la posicion actual
        Serial.println(mensaje);
        setMsg("_");
        contador=-1;
      }
      
    }
    }
   
   if(key == 'A'){

      limpiarBanner();
      estoyMenuPrincipal = false;
      limpiarFilaDeInsercion();
      activarAlarma();      
   }

   if(key == 'B'){
   
      limpiarBanner();
      estoyMenuPrincipal = false;
      limpiarFilaDeInsercion();
      desactivarAlarma();      
   }

   if(key == 'C'){
   
      limpiarBanner();
      estoyMenuPrincipal = false;
      limpiarFilaDeInsercion();
      abrirGaraje();      
   }

   if(key == 'D'){
   
      limpiarBanner();
      estoyMenuPrincipal = false;
      limpiarFilaDeInsercion();
      cerrarGaraje();      
   }
       
    break;

// ------------------------------------
    case HOLD:

    if(key == '#'){
      
      codigoIngresado = mensaje;        //guarde en una variable global lo que haya en mensaje 
      mensaje = "";
      
      limpiarFilaDeInsercion();         // como hacer para salirse del menu ?
    }

    break;
  }
  
}  // end keyEvent()



void activarAlarma(){
 
  if(!alarmaActiva){

    if(contadorEstadoA == 0){   // si es la primera vez que presiona el boton, ejecute su logica correspondiente
  
      setBanner(mensajeDeInsersion, 0);          
      estadoA = true;
      contadorEstadoA ++;
      
    }else {                   // si es la segunda vez que presiono el boton, devuelvase al menu principal

      estadoA = false;
      contadorEstadoA = 0;
      errores = 0;
      
      mensaje = "";             // caso tal que el usuario haya ingresado algo y retornado al menu principal
      limpiarFilaDeInsercion();         
    }
    
  } else {

      alerta = notificacion;      
  }  
}

void desactivarAlarma(){

  if(alarmaActiva){

    if(contadorEstadoB == 0){   // si es la primera vez que presiona el boton, ejecute su logica correspondiente

      setBanner(mensajeDeInsersion, 0);          
      estadoB = true;
      contadorEstadoB ++;      
    
    } else{                     // si es la segunda vez que presiono el boton, devuelvase al menu principal

      estadoB = false;
      contadorEstadoB = 0;
      errores = 0;


      mensaje = "";             // caso tal que el usuario haya ingresado algo y retornado al menu principal
      limpiarFilaDeInsercion();         
    }
    
  } else {

    alerta = notificacion;      
  }   
}


void abrirGaraje(){

if(garajeCerrado){

    if(contadorEstadoC == 0){

      setBanner(mensajeDeInsersion1, 0);          
      estadoC = true;
      contadorEstadoC ++;      
    
    } else{               // si es la segunda vez que presiono el boton, devuelvase al menu principal

      estadoC = false;
      contadorEstadoC = 0;
      errores = 0;


      mensaje = "";             // caso tal que el usuario haya ingresado algo y retornado al menu principal
      limpiarFilaDeInsercion(); 
    }
    
  } else {

    alerta = notificacion;      
  }     
}

void cerrarGaraje(){

if(!garajeCerrado){

    if(contadorEstadoD == 0){

      setBanner(mensajeDeInsersion1, 0);          
      estadoD = true;
      contadorEstadoD ++;      
    
    } else{               // si es la segunda vez que presiono el boton, devuelvase al menu principal

      estadoD = false;
      contadorEstadoD = 0;
      errores = 0;

      mensaje = "";             // caso tal que el usuario haya ingresado algo y retornado al menu principal
      limpiarFilaDeInsercion(); 
    }
    
  } else {

    alerta = notificacion;      
  }  
  
}


/*  */

void statusDelCodigoIngresado(){            // metodo que evalua el codigo dependiedo del estado (boton seleccionado del menu)

  if(estadoA){

   if(alerta.equals(vacio)){    
      setBanner(mensajeDeInsersion, 0);    // alerta estara vacio cuando se acabe el tiempo de notificacion        
      lcd.setRGB(200, 200, 100);
    }
  
   if(!codigoIngresado.equals(vacio)){

      if(codigoIngresado.equals(codigo_1)){
  
        alarmaActiva = true;                  // si el codigo ingresado es igual al codigo_1, activo la alarma.
        estadoA = false;                      // reinicie al estado original y resuma ejecucion principal (StatusDelSistema). 
  
        mensaje = "";                         // reinicie otra vez las variables para insercion posterior
        codigoIngresado = "";
         
        limpiarBanner();
        // añadir sonido, 
  
      } else {
  
        mensaje = "";                         // reinicie otra vez las variables para insercion posterior
        codigoIngresado = "";
  
        alerta = codigoErroneo;   
        errores ++;   

        // AÑADIR SONIDO
      }
    }
  }


  if(estadoB){
    
    if(alerta.equals(vacio)){
      setBanner(mensajeDeInsersion, 0);    // alerta estara vacio cuando se acabe el tiempo de notificacion  
      lcd.setRGB(200, 200, 100);
    }

    if(!codigoIngresado.equals(vacio)){

      if(codigoIngresado.equals(codigo_1)){
  
        alarmaActiva = false;                  // si el codigo ingresado es igual al codigo_1, desactivo la alarma.
        estadoB = false;
        
        mensaje = "";                         // reinicie otra vez las variables para insercion posterior
        codigoIngresado = "";
           
        limpiarBanner();
        
        //  AÑADIR SONIDO
  
      } else {
  
        mensaje = "";                         // reinicie otra vez las variables para insercion posterior
        codigoIngresado = "";
  
        alerta = codigoErroneo;
        errores ++;      
      }
    }
  }



  if(estadoC){

    if(alerta.equals(vacio)){      
      setBanner(mensajeDeInsersion1, 0);           // alerta estara vacio cuando se acabe el tiempo de notificacion  
      lcd.setRGB(200, 200, 100);
    }

    if(!codigoIngresado.equals(vacio)){

      if(codigoIngresado.equals(codigo_2)){
        //Abrir garaje
        controlarServo(180,true);
        
        
        garajeCerrado = false;                  // si el codigo ingresado es igual al codigo_1, desactivo la alarma.
        estadoC = false;
        
        mensaje = "";                           // reinicie otra vez las variables para insercion posterior
        codigoIngresado = "";
           
        limpiarBanner();

        
        //  AÑADIR SONIDO
  
      } else {                                  // si el codigo ingresado no es igual, despliego un mensaje
  
        mensaje = "";                           // reinicie otra vez las variables para insercion posterior
        codigoIngresado = "";
  
        alerta = codigoErroneo;
        errores ++;      
      }
    }
    
  }


  if(estadoD){

    if(alerta.equals(vacio)){      
      setBanner(mensajeDeInsersion1, 0);           // alerta estara vacio cuando se acabe el tiempo de notificacion  
      lcd.setRGB(200, 200, 100);
    }

    if(!codigoIngresado.equals(vacio)){

      if(codigoIngresado.equals(codigo_2)){
        //Cerrar garaje
        controlarServo(180,false);
        garajeCerrado = true;                  // si el codigo ingresado es igual al codigo_1, desactivo la alarma.
        estadoD = false;
        
        mensaje = "";                           // reinicie otra vez las variables para insercion posterior
        codigoIngresado = "";
           
        limpiarBanner();
        
        //  AÑADIR SONIDO
  
      } else {                                  // si el codigo ingresado no es igual, despliego un mensaje
  
        mensaje = "";                           // reinicie otra vez las variables para insercion posterior
        codigoIngresado = "";
  
        alerta = codigoErroneo;
        errores ++;      
      }
    }
    
  }

  if(reinicioDelSistema){

    if(alerta.equals(vacio)){      
      setBanner(reinicio, 0);           // alerta estara vacio cuando se acabe el tiempo de notificacion  
     // lcd.setRGB(250, 0, o);
    }

    if(!codigoIngresado.equals(vacio)){

      if(codigoIngresado.equals(codigoMaestro)){
  
        reinicioDelSistema = false;                  // si el codigo ingresado es igual al codigo_1, desactivo la alarma.
        configuracionCompleta = false;
        
        mensaje = "";                           // reinicie otra vez las variables para insercion posterior
        codigoIngresado = "";

        codigoAlarma = false;
        codigoApertura = false;
           
        limpiarBanner();
        
        //  AÑADIR SONIDO
  
      } else {                                     // si el codigo ingresado no es igual, despliego un mensaje
  
        mensaje = "";                           // reinicie otra vez las variables para insercion posterior
        codigoIngresado = "";
  
        alerta = codigoErroneo;
      }
    }
    
  }
  
}


void inicializar(){

if(!configuracionCompleta){
  

  if(!codigoAlarma){                      // configure el codigo de la alarma
    
    setBanner(mensajeDeInsersion, 0);    
    
    longitud = mensaje.length();

    if(longitud == 3){                    // el codigo tiene una longitud de 6 caracteres         

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

    if(longitud == 1){

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

  if(errores == 3){

    errores = 0;
    reinicioDelSistema = true;                         // pare la ejecucion de cualquier estado
    
    estadoA = false;                      
    estadoB = false;
    estadoC = false;
    estadoD = false;

    mensaje = "";
    limpiarBanner();
    limpiarFilaDeInsercion();
    
  }
  
  if(configuracionCompleta && !estadoA &&
        !estadoB && !estadoC && !estadoD 
        && alerta.equals(vacio) && !reinicioDelSistema){     

    estoyMenuPrincipal = true;
    
    if(alarmaActiva){

      setBanner(alarmaPrendida, 0);
      lcd.setRGB(100, 250, 0);                        // cambie el color del display al color verde
      
    } else {

      setBanner(alarmaApagada, 0);
      lcd.setRGB(250, 100, 0);                        // cambie el color del display al color rojo

    }
    
    if(garajeCerrado){

      setBanner(GarageCerrado,10);
    } else {

      setBanner(GarageAbierto, 10);
    }

    if(puertaCerrada){

      setBanner2(PuertaCerrada,10);              
    }else{

      setBanner2(PuertaAbierta,10);                    
    }
    
  }
}


void desplegar(){


  if (!alerta.equals(vacio)){                       //  ¿ existe una alerta por mostrar ?

    if(!notaPorDesplegar){                          // ¿ es la primera vez en la que vamos a guardar el tiempo?

    tiempoInicial = millis();                       // si entramos la primera vez al metodo, guarde el valor inicial
    notaPorDesplegar = true;
    
    limpiarBanner();
    limpiarFilaDeInsercion();
    lcd.setRGB(250, 20, 0);                        // cambie el color del display al color rojo
  
  } else {
    
    tiempoActual= millis();
    if(tiempoActual - tiempoInicial < tiempoDeDespliegue ){     // ¿ ya pasaron los tres segundos ?

      setBanner(alerta,0);
      
    }else{

      alerta = "";                                              // reinicie variables para terminar ejecucion, y no seguir mostrando el mensaje      
      notaPorDesplegar = false;
      limpiarBanner();
      lcd.setRGB(200, 200, 100);
    }
   }    
  }    
}
  

void estadoDeSensores(){

    estadoDelSuiche = digitalRead(pinTouch);
    
    if(estadoDelSuiche == HIGH ){
      puertaCerrada = false;   

    }

    if(estadoDelSuiche == LOW ){
      puertaCerrada = true;
      //Serial.println(estadoPuertaAnterior);
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

void setBanner2(String msg, int pos){             // imprima el mensaje en la fila 1, desde la posicion "pos"

  lcd.setCursor(pos,1);
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

//true derecha, false izquierda
void controlarServo(int grados,bool sentido){
  //myServo.write(grados);
  myServo.attach(pinServo);
  if(sentido){
    for(int i=0;i<=grados;i++){
      myServo.write(i);
    }
    
  }else{
   for(int i=grados;i>0;i--){
      myServo.write(i);
    } 
  }
  delay(100);
  myServo.detach();
 
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}


