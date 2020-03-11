#include "Wally.h"

/*********************************
 * 			Kinematics			 *
 *********************************/  
bool _originSet = false;

GVector2D forward(GVector2D polar){
  float La = polar.x;
  float Lb = polar.y;
  
  //Theorical
  //float alpha = acos((La*La+4*w*w-(Lb*Lb))/(2*La*2*-w));
  float beta = acos((Lb*Lb+4*w*w-(La*La))/(2*Lb*2*w));
  
  //Real
  GVector2D result = GVector2D(Lb * cos(beta) - w, Lb * sin(beta), polar.z);
  return result;
}

GVector2D backward(GVector2D pos){
  float x,y;

  x = pos.x;
  y = pos.y;

  //Theorical
  float alpha = atan(y / (w-x)); 
  float beta  = atan(y / (w+x)); 
  float dLa = (y / sin(alpha));
  float dLb = (y / sin(beta ));
  GVector2D result = GVector2D(dLa, dLb, pos.z);
 return result;
}


void Wally::init() {    
  Botly::init();
  _originSet = false;
  while(!_originSet) isIRDataReceived();
}

void Wally::origin(){
    polar.x = polar.y = 0; //Useless ?
    cartesian.z = polar.z = PI/2;
    cartesian.x = 0;
    cartesian.y = paperGap + paperSize/2;
    polar = backward(cartesian);
    _originSet = true;

    Serial.print("Heading : ");
    Serial.println(cartesian.z * RAD_TO_DEG);
    Serial.println("  Cartesian ");
    Serial.print("  "); Serial.print(cartesian.x);Serial.print(" : "); Serial.println(cartesian.y);
    Serial.println("  Polar ");
    Serial.print("  "); Serial.print(polar.x);Serial.print(" : "); Serial.println(polar.y);
    
}

void Wally::avancer(float distance){
  turnGoDegree(0, distance);
}

void Wally::reculer(float distance){
  turnGoDegree(0, -distance);
}

float normalizeAngle(float angle){
  while(int(angle) >= 2*PI){
    angle = int(angle * RAD_TO_DEG) - 360;
    angle /= RAD_TO_DEG;
  }
  while(int(angle) <= 0){
    angle = int(angle * RAD_TO_DEG) + 360;
    angle /= RAD_TO_DEG;
  }
  return angle;
}


void Wally::tournerDroite(float angleDegree){
  float angle = DEG_TO_RAD * angleDegree;
  polar.z -= angle;
  polar.z = normalizeAngle(polar.z);
  cartesian.z = polar.z;
  
  if(polar.z >= 0 && polar.z <= PI/2) tone(_pinBuzzer, 1046,200);
  else if(polar.z > PI/2 && polar.z <= PI) tone(_pinBuzzer, 1318,200);
  else if(polar.z > PI && polar.z <= 3*PI/2) tone(_pinBuzzer, 1568,200);
  else if(polar.z > 3*PI/2 && polar.z <= 2*PI) tone(_pinBuzzer, 1975,200);
}

void Wally::tournerGauche(float angleDegree){
  float angle = DEG_TO_RAD * angleDegree;
  polar.z += angle;
  polar.z = normalizeAngle(polar.z);
  cartesian.z = polar.z;

  if(polar.z >= 0 && polar.z <= PI/2) tone(_pinBuzzer, 1046,200);
  else if(polar.z > PI/2 && polar.z <= PI) tone(_pinBuzzer, 1318,200);
  else if(polar.z > PI && polar.z <= 3*PI/2) tone(_pinBuzzer, 1568,200);
  else if(polar.z > 3*PI/2 && polar.z <= 2*PI) tone(_pinBuzzer, 1975,200);
}



void Wally::turnGoDegree(float angle, float ligne){
  angle = angle * DEG_TO_RAD ; // Passage en radians
  turnGo(angle, ligne);
}

void Wally::turnGo(float angle, float ligne){

  if(angle > 0 && angle < PI){
    tournerGauche( angle );
  }
  else if( angle >= PI ){
	  tournerDroite( 2*PI - angle );
  }
  else if( angle < 0 ){
    tournerDroite( - angle );
  }
  else{
    stop(100);
  }
 
  if( ligne > 0 ){
    avant(ligne);
  }
  else if( ligne < 0 ){
    arriere(-ligne);
  }
  else{
    stop(100);
  }
}


void Wally::avant(long distance){
  //ABS
  cartesian.x -= cos(polar.z)*distance;
  cartesian.y -= sin(polar.z)*distance;
  GVector2D temp = polar;
  polar = backward(cartesian);

  temp = polar - temp;

	Steppers->moveTo(temp.x*_mmToStep/10, -temp.y*_mmToStep/10);
	Steppers->runSpeedToPosition();//Blockling...
	Steppers->setPositions();
}

void Wally::arriere(long distance){
  //ABS
  cartesian.x -= cos(polar.z)*distance;
  cartesian.y -= sin(polar.z)*distance;
  GVector2D temp = polar;
  polar = backward(cartesian);

  temp = polar - temp;

	Steppers->moveTo(-temp.x*_mmToStep/10, temp.y*_mmToStep/10 );
	Steppers->runSpeedToPosition();//Blockling...
	Steppers->setPositions();
}

//Left pulley
void Wally::gauche(long pas){
  Steppers->moveTo(0, pas);
  Steppers->runSpeedToPosition();//Blockling...
	Steppers->setPositions();
}

//Right pulley
void Wally::droite(long pas){
  Steppers->moveTo(pas, 0);
  Steppers->runSpeedToPosition();//Blockling...
	Steppers->setPositions();
}

void Wally::stop(long temps){
  if(temps == 0) while(true) isIRDataReceived();
  else delay(temps);
}

void Wally::execRequest(int input){
	switch (input)
	{
    case 0x24000FF:
      /* A */
      avancer(50);
      Serial.println("Forward +50 : ");
      Serial.println("  Cartesian ");
      Serial.print("  "); Serial.print(cartesian.x);Serial.print(" : "); Serial.println(cartesian.y);
      Serial.println("  Polar ");
      Serial.print("  "); Serial.print(polar.x);Serial.print(" : "); Serial.println(polar.y);
      break;
    case 0x24040BF:
      /* B */
      tournerDroite(90);
      Serial.print("Heading - 90 =");
      Serial.println(cartesian.z * RAD_TO_DEG);

      break;
    case 0x240807F:
      /* C */
      reculer(50);
      Serial.println("Backward +10 : ");
      Serial.println("  Cartesian ");
      Serial.print("  "); Serial.print(cartesian.x);Serial.print(" : "); Serial.println(cartesian.y);
      Serial.println("  Polar ");
      Serial.print("  "); Serial.print(polar.x);Serial.print(" : "); Serial.println(polar.y);
      break;
    case 0x240C03F:
      /* D */
      tournerGauche(90);
      Serial.print("Heading + 90 =");
      Serial.println(cartesian.z * RAD_TO_DEG);
      break;
    case 0x24022DD:
      /* E */
      origin();
      break;
    case 0x240A25D:
      /* F */
      droite(-500);
      break;
    case 0x24030CF:
      /* G */
      gauche(500);
      break;
    case 0x24058A7:
      /* H */
      droite(500);
      break;
    case 0x240708F:
      /* I */
      gauche(-500);
      break;
    default:
    tone(_pinBuzzer, 440, 100);
    tone(_pinBuzzer, 220, 100);
    tone(_pinBuzzer, 440, 100);
      break;
	}
}

void Wally::isIRDataReceived(){
	if (irrecv.decode(&results)) {
		//Serial.println(results.value, HEX);
    tone(_pinBuzzer, 800, 100);
		execRequest(results.value);
		irrecv.resume(); // Receive the next value
    }
}