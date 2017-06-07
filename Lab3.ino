#include <TFT.h>  // Arduino LCD library
#include <SPI.h>
#include "TimerOne.h"


#define TFT_CS     10
#define TFT_RST    9 
#define TFT_DC     8

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

#define TFT_SCLK 13   
#define TFT_MOSI 11   
#define SD_CS    4  // Chip select line for SD card

const int buzzer = 6;

int myWidth = tft.width();
int myHeight = tft.height();

// --------------- Barras -----------------
 
// variables for the position of the ball and paddle
int paddle1 = 0;
int paddle2 = 0;
int oldPaddle1, oldPaddle2;

// --------------- pelota -----------------

int ballDirectionX = 1;
int ballDirectionY = 1;

int movingSpeed = 0;
int ballSpeed = 2; // lower numbers are faster
bool rebote1 = false;
bool rebote2 = false;
int highestFactor = 0;
bool tieneVelocidad = false;

int ballX = myWidth/2; 
int ballY = myHeight/2;
int oldBallX, oldBallY;

// --------------- Puntaje -----------------

int pointP1=0, oldPointP1 = 0 , pointP2=0, oldPointP2 = 0;
bool point=false;
bool mostrarPuntaje = false;
int contadorPuntaje = 0;
bool refresh = false;

int player1ScoreX = 20; 
int player1ScoreY = 30; 

int player2ScoreX = 90;
int player2ScoreY = 30;


void setup() {
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT); 

  
  tft.initR(INITR_BLACKTAB);        
  tft.background(0, 0, 0);  
}

void loop() {

      
  traducirPotenciometros();

  velocidad();
  
  //mostrarPuntaje();

  if(!mostrarPuntaje){

    dibujarPaletas();
    dibujarPelota();
    dibujarCancha();  
    
  } else{
    
    dibujarCancha;   
    dibujarPuntaje();
  }
  
}


void dibujarPaletas(){


  
  tft.fill(0, 0, 0);                        // Borramos la posicion anterior de la barrita     
  if (oldPaddle1 != paddle1) {              // si la barra se ha  movido, entonces borre la barra de la posicion anterior
    tft.rect(oldPaddle1, 0, 20, 5);         // screen.rect(xStart, yStart, width, height)
    point=false;
  }
    if (oldPaddle2 != paddle2) {
    tft.rect(oldPaddle2, myHeight-5, 20, 5);
    point=false;
  }

  tft.fill(0, 255, 0);                      // dibujando la primera barra de color rojo
  tft.rect(paddle1, 0, 20, 5);              
  oldPaddle1 = paddle1;
  
  //tft.fill(0, 255, 0);                      // dibujando la segunda barra de color verde
  tft.rect(paddle2, myHeight-5, 20, 5);
  oldPaddle2 = paddle2;

 
}

void dibujarPelota(){

   // actualizacion y velocidad de la pelota
  if (millis() % ballSpeed < 2 && !point) {
    moveBall();
  }
}

void traducirPotenciometros(){

  // mapeo de los potenciometros al eje x. 
  paddle1 = map(analogRead(A0), 0, 1024, 2, myWidth-21) ;     // puntero del rectangulo que va de la posicion; x > 2 && x < 22
  paddle2 = map(analogRead(A1), 0, 1024, 2, myWidth-21);
}


void moveBall() {  
  
  if (ballX > myWidth - 5 || ballX < 5) {         // si la pelota se desborda en el eje x, invierta la direccion, tenga en cuenta el radio de la bola.
    ballDirectionX = -ballDirectionX;
  }
  
  if (ballY > myHeight ) {                    // si sobrepasa la cancha de cualquier jugador, reinicie la posicion de la pelota en la mitad de pantalla
                                              // y anote el puntaje  
     
      pointP1++;
      oldPointP1 = pointP1;
      
      ballX = myWidth/2;                
      ballY = myHeight/2;
      
      point=true;
      mostrarPuntaje = true;
      refresh = true;
      
      ballDirectionX = 1;                    // pelota de saque para el jugador 1
      ballDirectionY = 1; 
      
  }else if(ballY < 0) {
    
      pointP2++;
      ballX = myWidth/2;
      ballY = myHeight/2;
      
      point=true;
      mostrarPuntaje = true;
      refresh = true;
      
      ballDirectionX = -1;                  // pelota de saque para el jugador 2
      ballDirectionY = -1; 
      
  }

  
  
  if (inPaddle(ballX, ballY, paddle1, 0, 20, 5) || inPaddle2(ballX, ballY, paddle2, tft.height()-5, 20, 5)) {
    
    //ballDirectionX = -ballDirectionX;         
    ballDirectionY = -ballDirectionY;         // si la pelota interactua con la barra, invierta la direccion. 
  }
  
  
  ballX += ballDirectionX;                    // actualize la posicion de la barra
  ballY += ballDirectionY;

  
  if (oldBallX != ballX || oldBallY != ballY) {
    
    tft.fill(0, 0, 0);                        // borre la ultima posicion de la pelota 
    tft.circle(oldBallX, oldBallY, 3);
  }
  
  tft.fill(0, 255, 0);
  //tft.fill(255, 255, 255);                    // dibuje la pelota    
  tft.circle(ballX, ballY, 3);

  oldBallX = ballX;
  oldBallY = ballY;

}


void velocidad(){                              // calcule el nuevo factor de la velocidad segun la diferencia de movimiento en el 
                                               // potenciometro de cualquier jugador

int factor;                                       
      
  if(rebote1){

      movingSpeed = paddle1 - oldPaddle1;        
  }

  if(rebote2){

     movingSpeed = paddle2 - oldPaddle2;               
  }
      
  if(movingSpeed == 0){                       // registre la diferencia de potencial mas alta al momento de pegarle a la pelota.

    highestFactor = 0;
    
  }else{

    movingSpeed = abs(movingSpeed);
    
    if(movingSpeed > highestFactor){

      highestFactor = movingSpeed;
    }
  }  

//  Serial.println(movingSpeed);

  if(rebote1 || rebote2){                       // 3 niveles de velocidad segun la diferencia de potencial. 

    rebote1 = false;
    rebote2 = false;

    if(highestFactor < 4){
      
      factor = 1;      
          
     } else if(highestFactor < 10){
    
      factor = 2;
      tieneVelocidad = true;
          
     } else {
    
      factor = 3;
      tieneVelocidad = true;
     }

    if(ballDirectionX < 0){                     // si la pelota iba en decremento o incremento, mantenga la misma direccion
  
      ballDirectionX = -factor;
      
    } else {

      ballDirectionX = factor;      
    }

    if(ballDirectionY < 0){

      ballDirectionY = -factor;
      
    }else{

      ballDirectionY = factor;
    }
  }
}

void dibujarCancha(){


  tft.stroke(0,255,0);
  tft.line(0, myHeight/2, myWidth, myHeight/2);       // linea horizontal
  tft.line(0, 0, 0, myHeight);                        // linea vertical izquierda
  tft.line(myWidth-1, 0, myWidth-1, myHeight);        // linea vertical derecha   
  tft.noStroke();  
}


void dibujarPuntaje(){

    if(refresh){

      refresh = false;
      
      tft.fillScreen(ST7735_BLACK);            
      dibujarCancha();   

      tft.setRotation(tft.getRotation()+1);             // dibuje los puntajes del jugador.

      tft.setCursor(player1ScoreX, player1ScoreY);
      tft.setTextColor(ST7735_GREEN);
      tft.setTextSize(10);
      tft.print(pointP1);

      tft.setCursor(player2ScoreX, player2ScoreY);
      tft.setTextColor(ST7735_GREEN);
      tft.setTextSize(10);
      tft.print(pointP2);
        
      tft.setRotation(tft.getRotation()-1);

      Timer1.initialize(1000000); 
      Timer1.attachInterrupt(calcularPuntaje);           
    }
    
}


void calcularPuntaje(){
  
  
  
  if(contadorPuntaje == 2){

    contadorPuntaje = 0;
    mostrarPuntaje = false;          // retorne a la pantalla de juego

    tft.fillScreen(ST7735_BLACK);   // borre lo escrito en pantalla
    tft.fill(255,255,255);          // restaure el color inicial 
    
    Timer1.stop();
    Timer1.detachInterrupt();
  
  } else{

    tone(buzzer, 3951, 100); 
    contadorPuntaje ++;    
  }
}


void sonido(){
     
  //if(error){
    
        tone(buzzer, 440, 83);
  //  }else{
      
        tone(buzzer, 3951, 100); 
  //  }    
}


boolean inScore(int x, int y, int rectX, int rectY, int rectWidth, int rectHeight){ // ¿ se intersecta la pelota y el puntaje ?

  
  boolean result = false;  // PH  

  if( (x >= (rectX-5) && x <= (rectX + rectWidth + 5)) &&   // tenga en cuenta el radio de la pelota y las posibles direcciones en la que podria entrar
       (y >= rectY && y <= (rectY + rectHeight + 5)) ){

          result = true;      
          Serial.println("im in score!");
       }
  
  return result; 
}


boolean inPaddle(int x, int y, int rectX, int rectY, int rectWidth, int rectHeight) {     // ¿ se intersecta la pelota y la barra ?
  
boolean result = false;

if ((x >= (rectX - 6) && x <= (rectX + rectWidth) + 6)) {   


    if(y == (rectY + rectHeight + 5)){

      result = true;
      rebote1 = true;
      
      return result;      
            
      //rebote2 = false;
      //strike = true;
    }
      
      
      if (y < (rectY + rectHeight + 5)){            // si la barra "atrapa" la pelota, no se toma como un rebote
        
        if(tieneVelocidad){                         // si tiene una velocidad, la pelota es valida y no cuenta como el punto
          
       
          Serial.println("tiene vel !");
          tieneVelocidad = false;
          ballY = rectY + rectHeight + 5;
          
        
          result = true;
          return result;
          
        }

         result = false;         
      }
  }
  
  return result;
}


boolean inPaddle2(int x, int y, int rectX, int rectY, int rectWidth, int rectHeight) {
  
  boolean result = false;


if ((x >= (rectX - 6) && x <= (rectX + rectWidth) + 6)){

    if(y == (rectY - 5)){

        result = true;
        rebote2 = true;
        
        return result;                
        
        //strike = true;
        //rebote1 = false;        
    }
     
    if(y > rectY - 5){

      if(tieneVelocidad){                 // si tiene una velocidad diferente a la inicial, la pelota es valida y no cuenta como el punto

        Serial.println("tiene vel !");

        tieneVelocidad = false;
        ballY = rectY -5 ;            

        result = true;
        return result;
      }

      result = false;
    }
  } 

return result;
  
}
 
