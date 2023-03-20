#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

extern "C" {                        //For getchar function works properly
  int putchar(int c) {              //(not necessary if printf (or sprintf) function is not used)
    Serial.write((uint8_t)c);
    return c;
  }
}
void setup() {                      //MSP430
  Serial.begin(9600);

}
int shiftRows (uint16_t input) //swap nibble (2 and 4)
{   

    uint8_t secondNibble = (input >> 8) & 0b1111;
    uint8_t fourthNibble = (input >> 0) & 0b1111;
    uint16_t output = (input & 0b1111000011110000) | (fourthNibble << 8) | secondNibble;
    return output;
   
}  
   
int inverseSubNibble (int input)
{
    
    const int inverseSbox[4][4] = {{10, 5,  9, 11},
                                   { 1, 7,  8, 15},
                                   { 6, 0,  2,  3},
                                   {12, 4, 13, 14}};
    
    //In order to select an index from inverse S-Box,
    //I must evaluate the input as a binary value.  
    
    int inputBinary[16];                //Array to hold binary array
    for (int i = 15; i >= 0; i--)       //Converting the input to binary array
    {
    inputBinary[i] = input & 1;
    input >>= 1;
    }

    int inverseSboxOutput[16 / 4];      //Array to hold inverse S-Box output
    for (int i = 0; i < 16; i += 4)     //Getting the outputs by inserting
    {                                   //the inputBinary array into the inverse S-Box
        int row = (inputBinary[i] * 2) + inputBinary[i + 1];
        int col = (inputBinary[i + 2] * 2) + inputBinary[i + 3];      //Finding whic inverse S-Box index it is
        inverseSboxOutput[i / 4] = inverseSbox[row][col];        //Getting inverse S-Box output
    }
    
    char inverseSboxOutputStr[5];              //S-Box outpus are hexadecimal values 
    for (int i = 0; i < 4; i++)                //Here I convert them to a string
    {                                          //(then to integer value)             
        sprintf(inverseSboxOutputStr + i, "%x", inverseSboxOutput[i]);
    }
    int finalOutput = strtol(inverseSboxOutputStr, NULL, 16);        //Here convert them to integer

    return finalOutput;
}

int subNibble (int input)
{   
    const int sbox[4][4] = {{ 9,  4, 10, 11},
                            {13,  1,  8,  5},
                            { 6,  2,  0,  3},
                            {12, 14, 15,  7}};

    //In order to select an index from S-Box,
    //I must evaluate the input as a binary value.
   
    int inputBinary[16];            //Array to hold binary array
    for (int i = 15; i >= 0; i--)   //Converting the input to binary array
    {
    inputBinary[i] = input & 1;
    input >>= 1;
    }

    int sboxOutput[16 / 4];                     //Array to hold S-Box output
    for (int i = 0; i < 16; i += 4)             //Getting the outputs by inserting
    {                                           //the inputBinary array into the S-Box
        int row = (inputBinary[i] * 2) + inputBinary[i + 1];
        int col = (inputBinary[i + 2] * 2) + inputBinary[i + 3];   //Finding whic S-Box index it is
        sboxOutput[i / 4] = sbox[row][col];     //Getting S-Box output
    }
    
    
    char sboxOutputStr[5];                          //S-Box outpus are hexadecimal values 
    for (int i = 0; i < 4; i++)                     //Here I convert them to a string
    {                                               //(then to integer value)                                                       
        sprintf(sboxOutputStr + i, "%x", sboxOutput[i]);
    }
    int finalOutput = strtol(sboxOutputStr, NULL, 16);              //Here convert them to integer
    return finalOutput;
}



int inverseMixColumn(int input) 
{
  int m[4][16] = {{0, 1, 2,  3, 4,  5,  6,  7, 8,  9, 10, 11, 12, 13, 14, 15},
            /*2*/ {0, 2, 4,  6, 8, 10, 12, 14, 3,  1,  7,  5, 11,  9, 15, 13}, 
            /*4*/ {0, 4, 8, 12, 3,  7, 11, 15, 6,  2, 14, 10,  5,  1, 13,  9},
            /*9*/ {0, 9, 1,  8, 2, 11,  3, 10, 4, 13,  5, 12,  6, 15,  7, 14}};
            
  int Sbinary[16] = {0};
  int MixColmnHexaVersion[4];               //Converting integer inputt to binary and stored Sbinary
  for (int i = 15; i >= 0; i--) 
    {
    Sbinary[i] = input & 1;
    input >>= 1;
    }

  int M09, M12, M02, M19, M29, M32, M22, M39;       //The first digit represents i in S[i],
                                                    //the second number is represent which number it is multiplied by.
  int inputN[4];  
  for (int i = 0; i < 16; i = i + 4) {              //Convert Sbinnary array to hexa, but in nibble format
    inputN[i/4] = (Sbinary[i] * 8) + (Sbinary[i + 1] * 4) + (Sbinary[i + 2] * 2) + (Sbinary[i + 3] * 1);
  }
  
  
  M09 = m[3][inputN[0]];  // S0
  M12 = m[1][inputN[1]];
  
  M02 = m[1][inputN[0]];  // S1
  M19 = m[3][inputN[1]]; 
  
  M29 = m[3][inputN[2]];  // S2
  M32 = m[1][inputN[3]];
  
  M22 = m[1][inputN[2]];  // S3
  M39 = m[3][inputN[3]];
  
  
  MixColmnHexaVersion[0] = M09 ^ M12;
  MixColmnHexaVersion[1] = M02 ^ M19;
  MixColmnHexaVersion[2] = M29 ^ M32;
  MixColmnHexaVersion[3] = M22 ^ M39;

 
 char MixColmOutputStr[5];                          //S-Box outpus are hexadecimal values 
    for (int i = 0; i < 4; i++)                     //Here I convert them to a string
    {                                               //(then to integer value)        
        sprintf(MixColmOutputStr + i, "%x", MixColmnHexaVersion[i]);
    }
    
    int finalOutput = strtol(MixColmOutputStr, NULL, 16);   //Here convert them to integer
    return finalOutput;

}

int KeyExpand1 (int key0)           //key0 = first key (we detrmine this)
{                                                       //key0 = w0w1 
    uint8_t w0 = key0 >> 8;                             //key1 = w2w3
    uint8_t w1 = key0 & 0xff;
    uint8_t A0 =  ((w1 & 0b11110000) >> 4) | ((w1 & 0b00001111) << 4);
    uint8_t A = subNibble(A0);                        
    uint8_t RC1 = 0x80;     //constant
    uint8_t w2 = w0 ^ RC1 ^ A;
    uint8_t w3 = w2 ^ w1;
    uint16_t key1 = (w2 << 8) | w3;
  return key1;
}

int KeyExpand2 (int key1)
{
    uint8_t w2 = key1 >> 8;                            //key1 = w2w3
    uint8_t w3 = key1 & 0xff;                          //key2 = w4w5
    uint8_t temp =  ((w3 & 0b11110000) >> 4) | ((w3 & 0b00001111) << 4);
    uint8_t B = subNibble(temp);                          
    uint8_t RC2 = 0x30;     //constant                  
    uint8_t w4 = w2 ^ RC2 ^ B;
    uint8_t w5 = w3 ^ w4;
    uint16_t key2 = (w4 << 8) | w5;
    return key2;
}

void loop()
{
    //ROUND 0
    uint16_t chipherText = 0x4628;
    printf("chipherText : %04x\n", chipherText); 
    uint16_t key0 = 0x1515;
    printf("key0 : %04x\n", key0);
    
    //KEY EXPANDS

    uint16_t key1 = KeyExpand1 ( key0);
    printf("key1 : %04x\n", key1);
    uint16_t key2 = KeyExpand2 ( key1);
    printf("key2 : %04x\n", key2);
    
    //ROUND 0
    
    uint16_t state0 = chipherText ^ key2;
    
    //ROUND 1
    
    uint16_t shiftState0 = shiftRows(state0);   
    uint16_t subShiftState0 = inverseSubNibble(shiftState0);
    uint16_t addKeyState0 = subShiftState0 ^ key1;
    uint16_t state1 = inverseMixColumn(addKeyState0);

    //ROUND 2
    
    uint16_t shiftState1 = shiftRows(state1);
    uint16_t shiftSubState1 = inverseSubNibble(shiftState1);
    uint16_t state2 = shiftSubState1 ^ key0;    
    uint16_t plainText = state2;
    
    printf("plainText : %04x\n", state2);
 
}
