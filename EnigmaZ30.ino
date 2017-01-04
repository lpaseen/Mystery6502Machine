/* 2017-01-02 Peter Sjoberg <peters-src AT techwiz DOT ca>
 *      added option to get output in letters instead of always numbers
 *      made printkey a little easier to read
 * 2017-01-03  Peter Sjoberg <peters-src AT techwiz DOT ca>
 *	Added code tables to use for decode/encode of things
 *	
 *
 *
 * BUG/worknotes: no way to de/encode without going through the ascii conversion.
 *      codebook 0 was intended to do it but the logic is broken since _every_ codebook code is encoded as two characters
 *	possible fix, add a status to each codebook or something. if size >127 it's encoded as one character?
 */


#define ringsettingkeyidx 3
#define rotorkeyidx 7
#define keyinoutidx 11

boolean outletter=false; // whatever we have letters or numeric output

byte enigmakey[] = {
  //1, 2, 3, 1, 6, 7, 8, 4, 9, 8, 6, 0, 0
  //1, 2, 3, 1, 1, 1, 1, 8, 8, 8, 8, 0, 0
  //1, 2, 3, 1, 6, 7, 8, 4, 9, 8, 9, 0, 0
  1, 2, 3, 4, 5, 6, 7, 8, 8, 8, 8, 0, 0
};

const byte rotorindex[] PROGMEM = {
  30, 0, 10, 20
};

const byte rotors[] PROGMEM = {
  9, 6, 4, 1, 8, 2, 7, 0, 3, 5,
  2, 5, 8, 4, 1, 0, 9, 7, 6, 3,
  4, 3, 5, 8, 1, 6, 2, 0, 7, 9,
  2, 5, 0, 7, 9, 1, 8, 3, 6, 4,
  7, 3, 5, 8, 2, 9, 1, 6, 4, 0,
  5, 4, 0, 9, 3, 1, 8, 7, 2, 6,
  7, 4, 6, 1, 0, 2, 5, 8, 3, 9,
};

uint8_t bookused=0;
const char codebook[][99] PROGMEM = {
  {26,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'},
  {96,' ','!','"','#','$','%','&','\'','(',')','*','+',',','-','.','/','0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?','@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','^','_','`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','{','|','}','~','\n'}
  };

/*****************/
byte calculateRing(byte KeyIn,  byte RotorIdx)
{
  char t = 0;  // signed type

  t = KeyIn + enigmakey[rotorkeyidx + RotorIdx] + 1;

  if (t > 9) t = t - 10;

  t = t - enigmakey[ringsettingkeyidx + RotorIdx];

  if (t < 0) t = t + 10;

  return t;
}

/*****************/
void steprotor() {
  byte step[5];

  step[0] = 0;
  for (char i = 1; i < 4; i++) {
    step[i] = 0;
    if (calculateRing(0, i) == 9) {
      step[i] = 1;
    }
  }
  step[4] = 1;

  for (char i = 0; i < 4; i++) {
    if (step[i] || step[i + 1]) {
      enigmakey[rotorkeyidx + i]++;
      if (enigmakey[rotorkeyidx + i] > 9) {
        enigmakey[rotorkeyidx + i] = 0;
      }
    }
  }
}

/*****************/
byte enigma(byte KeyIn)
{
  char t = KeyIn;
  char nextrotor = -1;
  char currrotor = 3;
  char currrotortype;
  char reverserotor = 0;

  enigmakey[keyinoutidx] = KeyIn;

  steprotor();

  for (char i = 0; i < 7; i++) {

    t = calculateRing(t, currrotor);

    currrotortype = 0;
    if (currrotor != 0) {
      currrotortype = enigmakey[currrotor - 1];
    }

    currrotortype =  pgm_read_byte_near(rotorindex + currrotortype);

    t = pgm_read_byte_near(rotors + currrotortype + reverserotor + t);

    currrotortype = calculateRing(0, currrotor);
    t = t - currrotortype;
    if (t < 0) {
      t += 10;
    }

    currrotor += nextrotor;

    if (i == 2) {
      nextrotor = 1;
    }

    if (i == 3) {
      reverserotor = 40;
    }
  }

  enigmakey[keyinoutidx + 1 ] = t;

  return t;
}

/*****************/
void printkey() {
  /*
  for (char i = 0; i < 11; i++) {
    Serial.print('0');
    Serial.print(enigmakey[i]);
    if (i < 10) {
      Serial.print('+');
    }
  }
  Serial.println("");
  */
  
  Serial.print("Rotor:  ");
  for (char i = 0; i < 3; i++) {
    Serial.print(enigmakey[i]);
  }
  Serial.println();

  Serial.print("Ring : ");
  for (char i = 3; i < 7; i++) {
    Serial.print(enigmakey[i]);
  }
  Serial.println();

  Serial.print("Wheel: ");
  for (char i = 7; i < 11; i++) {
    Serial.print(enigmakey[i]);
  }
  Serial.println();
  Serial.print(F("output is "));
  if (outletter){
    Serial.println(F("letters"));
  }else{
    Serial.println(F("numeric"));
  }
  Serial.print(F("Codebook: "));
  Serial.println(bookused);

}

/*****************/
void printrotor() {
  for (char i = 0; i < 4; i++) {
    Serial.print(enigmakey[rotorkeyidx + i]);
  }
  Serial.println();
}

/*****************/
void printenigma() {
  for (char i = 0; i < 4; i++) {
    Serial.print(enigmakey[rotorkeyidx + i]);
  }

  Serial.print(' ');
  Serial.print(enigmakey[keyinoutidx]);
  Serial.print(' ');
  Serial.println(enigmakey[keyinoutidx + 1]);
}

/*****************/
void settype(int type) {
  for (byte i = 0; i < 3; i++) {
    enigmakey[2 - i] = type % 10;
    type = type / 10;
  }
}

/*****************/
void setrings(int rings) {
  for (byte i = 0; i < 4; i++) {
    enigmakey[ringsettingkeyidx + 3 - i] = rings % 10;
    rings = rings / 10;
  }
}

/*****************/
void setwheel(int wheel) {
  for (byte i = 0; i < 4; i++) {
    enigmakey[rotorkeyidx + 3 - i] = wheel % 10;
    wheel = wheel / 10;
  }
}

/*****************/
byte groups = 0;
void print(byte o)
{
  static boolean firstbyte=true;
  static uint8_t prevbyte;

  if (outletter){
    if (firstbyte){
      prevbyte=o;
      firstbyte=false;
      return;
    }else{
      firstbyte=true;
      prevbyte=prevbyte*10+o;
      if (prevbyte>pgm_read_byte(codebook[bookused])){
	prevbyte='?';
      }else{
	//	Serial.print(prevbyte,DEC);Serial.print(F("=>")); //PSDEBUG
	prevbyte=pgm_read_byte(codebook[bookused]+prevbyte);
      }
      Serial.print((char)prevbyte);
      //Serial.print(prevbyte,DEC);Serial.print(F(" ")); //PSDEBUG
      if (pgm_read_byte(codebook[bookused])>26){ // If it's more than just the alphabet it is assumed that space is included so we don't need to space it out
	groups=0;
      }
    }
  }else{
    Serial.print(o);
  }
  if (groups++ == 3) {
    Serial.print(' ');
    groups = 0;
  }
}

/*****************/
int encode(const char str[]) {

  int pos = 0;
  int i;
  char v;
  byte o;

  while(v = str[pos++]){
    for (i=1;i<pgm_read_byte(codebook[bookused]);i++){
      if (pgm_read_byte(codebook[bookused]+i) == v){
	break;
      };
    }
    if (pgm_read_byte(codebook[bookused]+i) == v){ //we had a match
      o = enigma(i / 10);
      print(o);
      o = enigma(i % 10);
      print(o);
    }else{
      Serial.print(F("no match for "));
      Serial.print(v,DEC);
      Serial.print(F(", i="));
      Serial.println(i,DEC);
    }
    if (pos>99){break;}; // emergency break - should never happen
  }
}

/*****************/
int decode(const char str[]) {
  int i = 0;
  char v;
  byte o;

  while(v = str[i++]){
    if ((v >= '0') && (v <= '9')) {
      o = enigma(v - '0');
      print(o);
    }
  }
}

/*****************/
void testencode(){
  char msg0[] = "TEST";
  char msg1[] = "Test";
  char code0[] = "6716 3894";
  char code1[] = "7259 2218";

  settype(123);
  setrings(4567);
  setwheel(4711);

  Serial.println();
  Serial.print(F("encoding "));
  
  if (bookused==0){
    Serial.print(msg0);Serial.print(F(" should become "));Serial.println(code0);
    outletter=false;
    encode(msg0);
    Serial.println();
    setwheel(4711);
    Serial.print(F("decode "));Serial.print(code0);Serial.print(F(" should become "));Serial.println(msg0);
    outletter=true;
    decode(code0);
  }else if (bookused==1){
    Serial.print(msg1);Serial.print(F(" should become "));Serial.println(code1);
    outletter=false;
    encode(msg1);
    Serial.println();
    setwheel(4711);
    Serial.print(F("decode "));Serial.print(code1);Serial.print(F(" should become "));Serial.println(msg1);
    outletter=true;
    decode(code1);
  }
} // testencode

/****************************************************************/
void setup() {
  // put your setup code here, to run once:

  Serial.begin(38400);

  settype(312);
  setrings(4257);
  setwheel(4711);

  printkey();
  
  testencode();
  Serial.println();
  Serial.println();
  outletter=false;
  printkey();
  Serial.println();
  Serial.println(F("enter \"?\" for help"));
}

byte KeyPressed = 0;
byte KeyIn = 0;

byte SetType = 0;
byte SetRing = 0;
byte SetWheel = 0;

/****************************************************************/
void loop() {
  static boolean selectbook=false;
  static boolean encodetext=false;
  
  // put your main code here, to run repeatedly:

  if (Serial.available())
    {
      KeyPressed = Serial.read();

      if (encodetext){ // we have some text to encode according to current codebook
	if (KeyPressed=='\0' || KeyPressed=='\n'){ // end of text (so we can never encode embedded line here!)
	  encodetext=false;
	}else{
	  char msg[2];msg[0]=KeyPressed;msg[1]='\0';
	  encode(msg);
	}
	return;
      }
      
      if (KeyPressed == '!') { // select rotors, "!123"
	SetType = 3;

	SetRing = 0;
	SetWheel = 0;
	groups = 0;
	Serial.println();
      }

      if (KeyPressed == '@') { // set ringstellung "@1234"
	SetRing = 4;

	SetType = 0;
	SetWheel = 0;
	groups = 0;
	Serial.println();
      }

      if (KeyPressed == '#') { // set start weel position "#5678"
	SetWheel = 4;

	SetType = 0;
	SetRing = 0;
	groups = 0;
	Serial.println();
      }

      if (KeyPressed == '$') { // print out current key settings
	groups = 0;
	Serial.println();
	printkey();
      }

      if (KeyPressed == 'b') { // select book
	selectbook=true;
      }

      if (KeyPressed == '%') { // select book
	encodetext=true;
	return;
      }

      if (KeyPressed == '?') { // help
	Serial.println(F("!123  // select rotors"));
	Serial.println(F("@1234 // set ringstellung"));
	Serial.println(F("#5678 // set wheel start pos"));
	Serial.println(F("$     // print out current key"));
	Serial.println(F("b1    // select codebook"));
	Serial.println(F("l     // select letter output"));
	Serial.println(F("n     // select numeric output"));
	Serial.println(F("t     // run built in test with this codebook"));
	Serial.println(F("%TEST // encode some alphanumeric text according to current codebook"));
	Serial.println(F("?     // this help"));
	return;
      }

      if (KeyPressed == 'l') { // set letter output
	outletter=true;
	groups = 0;
	Serial.println();
	Serial.println(F("letter output"));
      }

      if (KeyPressed == 't') { // show test
	testencode();
	Serial.println();
	return;
      }
      if (KeyPressed == 'n') { // set numeric output
	outletter=false;
	groups = 0;
	Serial.println();
	Serial.println(F("numeric output"));
      }

      if ((KeyPressed >= '0') && (KeyPressed <= '9')) {
	KeyIn = (KeyPressed - '0');
	if (selectbook){
	  if (KeyIn<sizeof(codebook)/sizeof(codebook[0])){
	    bookused=KeyIn;
	    groups=0;
	  }else{
	    Serial.println();
	    Serial.println();
	    Serial.print(F("Book number to high, valid is 0-"));
	    Serial.println(sizeof(codebook)/sizeof(codebook[0])-1);
	    Serial.println();
	  }
	  selectbook=false;
	  Serial.println();
	  printkey();
	  return;
	}
  
	if (SetType + SetRing + SetWheel == 0) {
	  print(enigma(KeyIn));
	}

	if (SetType)
	  {
	    enigmakey[3 - SetType] = KeyIn;
	    SetType--;
	    if (SetType == 0) {
	      printkey();
	    }
	  }

	if (SetRing)
	  {
	    enigmakey[ringsettingkeyidx + 4 - SetRing] = KeyIn;
	    SetRing--;
	    if (SetRing == 0) {
	      printkey();
	    }
	  }

	if (SetWheel)
	  {
	    enigmakey[rotorkeyidx + 4 - SetWheel] = KeyIn;
	    SetWheel--;
	    if (SetWheel == 0) {
	      printkey();
	    }
	  } // if SetWheel
      } // if 0<=key<9
    } // if serial available
} // loop()
