/*

  The circuit:
 * LCD RS pin to digital pin 7
 * LCD Enable pin to digital pin 8
 * LCD D4 pin to digital pin 9
 * LCD D5 pin to digital pin 10
 * LCD D6 pin to digital pin 11
 * LCD D7 pin to digital pin 12
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 */
#define EN_PANTALLA_INICIO 0
#define EN_CARRERA 2
#define EN_PANTALLA_VICTORIA 3

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

//pines donde están conectados los sensores de ranura
int pin_sensor_paso_1 = 2;
int pin_sensor_paso_2 = 3;
int pin_buzzer = 13;
int pin_boton_start = 6;
int pin_boton_mas_vueltas = 4;
int pin_boton_menos_vueltas = 5;
//variables que guardan las vueltas de los coches
int vueltas_coche_1 = -1;
int vueltas_coche_2 = -1;

int valor_sensor_paso_1;
int valor_sensor_paso_2;

unsigned long tiempo_minimo_vuelta= 1500;//milisegundos, tiene que haber un tiempo minimo de vuelta para evitar falsos positivos
unsigned long tiempo_maximo_vuelta = 300000; //el tiempo máximo de vuelta es de 5 minutos
int max_vueltas = 999;
float tiempo_vuelta_1 = tiempo_maximo_vuelta;
float tiempo_vuelta_2 = tiempo_maximo_vuelta;
unsigned long instante_paso_1 = 0;
unsigned long instante_paso_2 = 0;
unsigned long tiempo_actual_1; //tiempo entre que pasa señal por los sensores no es el tiempo definitivo de vuelta
unsigned long tiempo_actual_2;

float mejor_vuelta_1 = tiempo_maximo_vuelta;
float mejor_vuelta_2 = tiempo_maximo_vuelta;

boolean coche1_dentro=false;
boolean coche2_dentro=false;

int posicion_lcd_vueltas_1 = 5;
int posicion_lcd_vueltas_2 = 5;
int posicion_lcd_mejor_1 = 11;
int posicion_lcd_mejor_2 = 11;

int estado_boton_start = 0;
int estado_boton_mas_vueltas = 0;
int estado_boton_menos_vueltas = 0;
int estado_programa = 0;

int vueltas_limite = 30;
int incremento_vueltas_al_pulsar = 1;

int coche_vencedor = 0;

String digitos_tiempo_vuelta_1;
String digitos_tiempo_vuelta_2;
String digitos_mejor_vuelta_1;
String digitos_mejor_vuelta_2;

void setup() {
  pinMode(pin_buzzer,OUTPUT);//initialize the buzzer pin as an output
  pinMode(pin_boton_start,INPUT); //Pin donde está conectado el pulsador
  pinMode(pin_boton_mas_vueltas,INPUT); //Pin donde está conectado el pulsador
  pinMode(pin_boton_menos_vueltas,INPUT); //Pin donde está conectado el pulsador
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.print("MicroElectronica");
  lcd.setCursor(0,1);
  lcd.print("R.Blanco");
  delay(1000);
  dibujaInicio();
  estado_programa = EN_PANTALLA_INICIO;

}

void dibujaInicio(){
  lcd.clear();
  lcd.print("Vueltas:");
  lcd.setCursor(9,0);
  lcd.print("- ");
  lcd.print(vueltas_limite);
  lcd.print(" +");
  lcd.setCursor(0,1);
  lcd.print("Pulsa 'Start'");
}

void empiezaCarrera(){
  inicializaVariables();
  cuentaAtras();
  instante_paso_1=millis();
  instante_paso_2=millis();
}

void inicializaVariables(){
  vueltas_coche_1 = -1;
  vueltas_coche_2 = -1;
  tiempo_vuelta_1 = tiempo_maximo_vuelta;
  tiempo_vuelta_2 = tiempo_maximo_vuelta;
  instante_paso_1 = 0;
  instante_paso_2 = 0;
  mejor_vuelta_1 = tiempo_maximo_vuelta;
  mejor_vuelta_2 = tiempo_maximo_vuelta;
  coche1_dentro=false;
  coche2_dentro=false;
  coche_vencedor = 0;
  //Serial.println("Variables inicializadas");
}

void cuentaAtras(){
  lcd.clear();
  lcd.print("TRES!");
  pita(pin_buzzer,100);
  delay(1000);
  lcd.clear();
  lcd.print("     DOS!!");
  pita(pin_buzzer,100);
  delay(1000);
  lcd.clear();
  lcd.print("          UNO!!!");
  pita(pin_buzzer,100);
  delay(1000);
  lcd.clear();
  lcd.print("   ADELANTE!!!");
  pita(pin_buzzer,200);
  delay(1000);
  lcd.clear();
}

void dibujaVictoria(int coche){
  lcd.setCursor(0,0);
  lcd.print("Gana el coche");
  lcd.setCursor(15,0);
  lcd.print(coche);

  lcd.setCursor(0,1);
  lcd.print("Mejor: ");
  lcd.setCursor(7 ,1);
  if(coche == 1){
    lcd.print(mejor_vuelta_1/1000);
  }else if(coche == 2){
    lcd.print(mejor_vuelta_2/1000);
  }

}

/*
 * Funcion LOOP
 */
void loop() {

  estado_boton_start=digitalRead(pin_boton_start);
  estado_boton_mas_vueltas=digitalRead(pin_boton_mas_vueltas);
  estado_boton_menos_vueltas=digitalRead(pin_boton_menos_vueltas);

  if(estado_boton_start==1){
    //si estamos en la pantalla de inicio comienza la carrera
    if(estado_programa == EN_PANTALLA_INICIO){
      estado_programa = EN_CARRERA;
      //Serial.println("EN_CARRERA");
      delay(500);
      empiezaCarrera();
    } else if(estado_programa == EN_CARRERA){//si estamos en carrera volvemos a la pantalla de inicio
      estado_programa = EN_PANTALLA_INICIO;
      //Serial.println("EN_PANTALLA_INICIO");
      delay(500);
      dibujaInicio();
    } else if(estado_programa == EN_PANTALLA_VICTORIA){
      estado_programa = EN_PANTALLA_INICIO;
      //Serial.println("EN_PANTALLA_INICIO");
      delay(500);
      dibujaInicio();
    }
  }

  if(estado_boton_mas_vueltas==1){
    if(estado_programa == EN_PANTALLA_INICIO){
      vueltas_limite+=incremento_vueltas_al_pulsar;
      if(vueltas_limite>999){
        vueltas_limite = 999;
      }
      dibujaInicio();
      delay(150);
    }
  }

  if(estado_boton_menos_vueltas==1){
    if(estado_programa == EN_PANTALLA_INICIO){
      vueltas_limite-=incremento_vueltas_al_pulsar;
      if(vueltas_limite<1){
        vueltas_limite = 1;
      }
      dibujaInicio();
      delay(150);
    }
  }

  if(estado_programa == EN_PANTALLA_VICTORIA){
    dibujaVictoria(coche_vencedor);
  }

  //////////////////////////////////
  // Código en carrera
  if(estado_programa == EN_CARRERA){
        valor_sensor_paso_1=digitalRead(pin_sensor_paso_1);//leemos la señal del sensor de paso 1
        valor_sensor_paso_2=digitalRead(pin_sensor_paso_2);//leemos la señal del sensor de paso 2

        if(valor_sensor_paso_1==HIGH) {
          if(vueltas_coche_1<0){
            vueltas_coche_1=0; //esto es para establecer el primer paso
            instante_paso_1=millis();
          } else{
            tiempo_actual_1 = millis()-instante_paso_1;
            if((tiempo_actual_1 > tiempo_minimo_vuelta) && !coche1_dentro){ //si se cumple el tiempo minimo de vuelta y el coche habia salido
              tiempo_vuelta_1 = tiempo_actual_1;
              if(mejor_vuelta_1 > tiempo_vuelta_1){
                mejor_vuelta_1 = tiempo_vuelta_1;
              }
              coche1_dentro = true;
              vueltas_coche_1++;
              pita(pin_buzzer,50);
              instante_paso_1=millis();
              lcd.clear();//limpiamos la pantalla, solo ante un cambio
              if(vueltas_coche_1>=vueltas_limite){
                coche_vencedor=1;
                estado_programa = EN_PANTALLA_VICTORIA;
                lcd.clear();
              }
            }
          }
        }else{
          coche1_dentro = false;
          //Serial.println("COCHE 1 FUERA");
        }

        if(valor_sensor_paso_2==HIGH){
          if(vueltas_coche_2<0){
            vueltas_coche_2=0; //esto es para establecer el primer paso
            instante_paso_2=millis();
          } else{
            tiempo_actual_2 = millis()-instante_paso_2;
            if((tiempo_actual_2 > tiempo_minimo_vuelta) && !coche2_dentro){ //si se cumple el tiempo minimo de vuelta y el coche habia salido
              tiempo_vuelta_2 = tiempo_actual_2;
              if(mejor_vuelta_2 > tiempo_vuelta_2){
                mejor_vuelta_2 = tiempo_vuelta_2;
              }
              coche2_dentro = true;
              vueltas_coche_2++;
              pita(pin_buzzer,50);
              instante_paso_2=millis();
              lcd.clear();//limpiamos la pantalla, solo ante un cambio
              if(vueltas_coche_2>=vueltas_limite){
                coche_vencedor=2;
                estado_programa = EN_PANTALLA_VICTORIA;
                lcd.clear();
              }
            }
          }
        }else{
          coche2_dentro = false;
          //Serial.println("COCHE 2 FUERA");
        }

        //dibujamos las vueltas de ambos coches si al menos han pasado por la salida
        /////////////////
        // Controlamos el dibujado de las vueltas del coche 1
        if(vueltas_coche_1==0){
          lcd.setCursor(0, 0);//posicionamos el cursor en la columna 0 fila 0
          lcd.print("000   0.00");
        }
        if(vueltas_coche_1>0 && vueltas_coche_1<10){
          lcd.setCursor(0,0);
          lcd.print("00");//ponemos 2 ceros delante de las unidades
          lcd.setCursor(2, 0);//posicionamos el cursor en la columna 0 fila 0
          lcd.print(vueltas_coche_1);
        }else if(vueltas_coche_1>=10 && vueltas_coche_1<100){
          lcd.setCursor(0,0);
          lcd.print("0");//ponemos un cero delante de las decenas
          lcd.setCursor(1, 0);//posicionamos el cursor en la columna 0 fila 0
          lcd.print(vueltas_coche_1);
        }else if(vueltas_coche_1>=100 && vueltas_coche_1<1000){
          lcd.setCursor(0,0);
          lcd.print(vueltas_coche_1);
        }
        // Fin dibujado vueltas del coche 1
        /////////////////

        /////////////////
        // Controlamos el dibujado de las vueltas del coche 2
        if(vueltas_coche_2==0){
          lcd.setCursor(0, 1);//posicionamos el cursor en la columna 0 fila 1
          lcd.print("000   0.00");
        }
        if(vueltas_coche_2>0 && vueltas_coche_2<10){
          lcd.setCursor(0,1);
          lcd.print("00");//ponemos 2 ceros delante de las unidades
          lcd.setCursor(2, 1);//posicionamos el cursor en la columna 2 fila 1
          lcd.print(vueltas_coche_2);
        }else if(vueltas_coche_2>=10 && vueltas_coche_2<100){
          lcd.setCursor(0,1);
          lcd.print("0");//ponemos un cero delante de las decenas
          lcd.setCursor(1, 1);//posicionamos el cursor en la columna 1 fila 1
          lcd.print(vueltas_coche_2);
        }else if(vueltas_coche_2>=100 && vueltas_coche_2<1000){
          lcd.setCursor(0,1);
          lcd.print(vueltas_coche_2);
        }
        // Fin dibujado vueltas del coche 1
        /////////////////

        //dibujamos el tiempo de la ultima vuelta de cada coche, si ha dado al menos una vuelta
        if(vueltas_coche_1>0 && !coche1_dentro){
          if(tiempo_vuelta_1<10000){
            lcd.setCursor(posicion_lcd_vueltas_1+1, 0);
            lcd.print(tiempo_vuelta_1/1000);
          }else if(tiempo_vuelta_1>=10000 && tiempo_vuelta_1<100000){
            lcd.setCursor(posicion_lcd_vueltas_1, 0);
            lcd.print(tiempo_vuelta_1/1000);
          }if(tiempo_vuelta_1>=100000){
            //lo paso a string y me quedo con los primeros 4 digitos
            digitos_tiempo_vuelta_1 = String(tiempo_vuelta_1/1000);
            digitos_tiempo_vuelta_1 = digitos_tiempo_vuelta_1.substring(0,3);
            lcd.setCursor(posicion_lcd_vueltas_1, 0);
            lcd.print(digitos_tiempo_vuelta_1);
            lcd.print("s");
          }
        }
        if(vueltas_coche_2>0 && !coche2_dentro){
          if(tiempo_vuelta_2<10000){
            lcd.setCursor(posicion_lcd_vueltas_2+1, 1);
            lcd.print(tiempo_vuelta_2/1000);
          }else if(tiempo_vuelta_2>=10000 && tiempo_vuelta_2<100000){
            lcd.setCursor(posicion_lcd_vueltas_2, 1);
            lcd.print(tiempo_vuelta_2/1000);
          }if(tiempo_vuelta_2>=100000){
            //lo paso a string y me quedo con los primeros 4 digitos
            digitos_tiempo_vuelta_2 = String(tiempo_vuelta_2/1000);
            digitos_tiempo_vuelta_2 = digitos_tiempo_vuelta_2.substring(0,3);
            lcd.setCursor(posicion_lcd_vueltas_2, 1);
            lcd.print(digitos_tiempo_vuelta_2);
            lcd.print("s");
          }
        }
        //dibujamos la vuelta rápida de cada coche, si ha dado mas de una vuelta
        if(vueltas_coche_1>1 && !coche1_dentro){
          if(mejor_vuelta_1<10000){
            lcd.setCursor(posicion_lcd_mejor_1+1, 0);
            lcd.print(mejor_vuelta_1/1000);
          }else if(mejor_vuelta_1>=10000 && mejor_vuelta_1<100000){
            lcd.setCursor(posicion_lcd_mejor_1, 0);
            lcd.print(mejor_vuelta_1/1000);
          }if(mejor_vuelta_1>=100000){
            //lo paso a string y me quedo con los primeros 4 digitos
            digitos_mejor_vuelta_1 = String(mejor_vuelta_1/1000);
            digitos_mejor_vuelta_1 = digitos_mejor_vuelta_1.substring(0,3);
            lcd.setCursor(posicion_lcd_mejor_1, 0);
            lcd.print(digitos_mejor_vuelta_1);
            lcd.print("s");
          }
        }
        if(vueltas_coche_2>1 && !coche2_dentro){
          if(mejor_vuelta_2<10000){
            lcd.setCursor(posicion_lcd_mejor_2+1, 1);
            lcd.print(mejor_vuelta_2/1000);
          }else if(mejor_vuelta_2>=10000 && mejor_vuelta_2<100000){
            lcd.setCursor(posicion_lcd_mejor_2, 1);
            lcd.print(mejor_vuelta_2/1000);
          }if(mejor_vuelta_2>=100000){
            //lo paso a string y me quedo con los primeros 4 digitos
            digitos_mejor_vuelta_2 = String(mejor_vuelta_2/1000);
            digitos_mejor_vuelta_2 = digitos_mejor_vuelta_2.substring(0,3);
            lcd.setCursor(posicion_lcd_mejor_2, 1);
            lcd.print(digitos_mejor_vuelta_2);
            lcd.print("s");
          }
        }

  }//fin de if(estado_programa == EN_CARRERA){
  // Fin de código de en carrera
  ////////////////////////////////////////

}//fin de loop()

//TODO implementarlas sin delay
void pita(int pin, int duration){

   for(int i=0;i<duration;i++)
   {
    digitalWrite(pin,HIGH);
    delay(1);//wait for 1ms
    digitalWrite(pin,LOW);
    delay(1);//wait for 1ms
    }

}


