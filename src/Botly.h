#ifndef Botly_h
#define Botly_h
#define LIBRARY_VERSION	0.36

#define SCOTT_V4 1 // Version de base de Botly
#define BOTLY_V1 2 // Version DIY de Botly

#include "BotlySteppers.h"


/*****************************************************
 *      	        Constantes utiles                *
 *            Attention calcul� seulement            *
 *   pour les roues et les moteurs du Botly v0.35)   *
 *****************************************************/

#define MM_TO_STEP 26.076
#define RAD_TO_STEP 1210
#define DELTA_ARC 47.5



/*********************
	 D�pendance
*********************/
#include <Servo.h>
#include <Arduino.h>
#include <IRremote.h>

#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#include "BotlySteppers.h"

void pin2_isr();

class Botly{
public:

  Servo crayon;
  IRsend irsend; //send with pin 13
  decode_results results;

  int pin = 9;
  IRrecv irrecv = new IRrecv(pin);

  Botly();

  Botly(int version);

  void init();

  void run();

  void stop(long temps);

  void stop();

  void gauche(long pas);

  void tournerGauche(long angleDegree);

  void droite(long pas);

  void tournerDroite(long angleDegree);

  void avant(long pas);

  void avancer(long distanceMillimeter);

  void arriere(long pas);

  void reculer(long distanceMillimeter);

  void setSpeed(float vitesse);

  void setSpeed(float vitesseD, float vitesseG);

  void logSpeed();

  void turnGo(float angle, int ligne);

  void turnGoDegree(float angle, int ligne);

  void polygone(unsigned int nbrCote, unsigned int longueur);

  void rectangle(unsigned int largeur, unsigned int longueur);

  void carre(unsigned int longueur);

  void cercle(unsigned int diametre);

  void arc(float rayon,float angle);

  void leverCrayon();

  void poserCrayon();

  void bougerCrayon(int angle);

  void isIRDataReceived();

  void initIRcom();

  void sonyCode(byte data);

  bool proximite();

  int mesureBatterie();

  void sleepNow();

  void sleepWakeup();

private:

  int _pinServo = 11 ;
  int _pinTsop = 9;
  int _pinIrEmetteur = 13 ;
  int _pinMesureBatterie = A5;
  BotlySteppers *Steppers;


  int tpsEcoule = 0 ;
  int tpsTop = 0 ;



  //Cst crayon
  int _bas = -35;
  int _haut = 10;

  int servoAction = 0;

};
#endif
