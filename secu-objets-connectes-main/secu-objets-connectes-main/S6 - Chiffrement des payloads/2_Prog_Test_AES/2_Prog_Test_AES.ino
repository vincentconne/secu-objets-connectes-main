#include <AES.h>
AES aes ;
int key_length = 128;

byte key_128[] = 
{
   0x8f, 0xab, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

byte my_iv[] = 
{
   0x00, 0x00, 0x00, 0x00, 0x00, 0xfa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
};

// Debug function => print value in HEX format
void Print_Debug_HEX(String title, byte* data, int data_length)
{
 Serial.print(title) ;
 for(int i = 0; i< data_length; ++i)
 {
  byte val = data[i];
  Serial.print (val>>4, HEX) ; Serial.print (val&15, HEX) ; Serial.print (" ") ; 
 }
 Serial.println() ; 
}

void loop () 
{
 
}
void setup ()
{
 Serial.begin(9600);
 while(!Serial);
 
 Serial.println("Essai Chiffrement AES - Séance 6 - Sécurité des IoT") ;
 Serial.println("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%") ;
 Serial.println("");
 Serial.println("----------------- Chiffrement AES -----------------") ;
 
 byte succ = aes.set_key (key_128,key_length) ;
 Serial.print("Affectation de la clef : "); (succ == SUCCESS) ? Serial.println("OK") : Serial.println("KO");
 String plain_data = "Le cours de sécurité des IoT est super !";
 int plain_length = plain_data.length();
 int nb_block_data = (plain_length+1)/N_BLOCK+1; 
 
 byte* plain = new byte[nb_block_data*N_BLOCK];
 byte* cipher = new byte[nb_block_data*N_BLOCK];
 byte* check = new byte[nb_block_data*N_BLOCK];
 
 plain_data.getBytes(plain,plain_data.length()+1);
 // Chiffrement des données
 byte iv [N_BLOCK];
 for (byte i = 0 ; i < N_BLOCK ; i++)
   iv[i] = my_iv[i] ;
 succ = aes.cbc_encrypt (plain, cipher, nb_block_data, iv) ;
 Serial.print("Chiffrement AES        : "); (succ == SUCCESS) ? Serial.println("OK") : Serial.println("KO");
 // Dechiffrement des données
 for (byte i = 0 ; i < 16 ; i++)
   iv[i] = my_iv[i] ;
 succ = aes.cbc_decrypt(cipher, check, nb_block_data, iv) ;
 Serial.print("Dechiffrement AES      : "); (succ == SUCCESS) ? Serial.println("OK") : Serial.println("KO");
 Serial.print ("Le message : '") ; Serial.print (plain_data) ; Serial.print ("' possède ") ; Serial.print (plain_length) ;Serial.print (" octets, il faudra donc ") ; Serial.print (nb_block_data) ;Serial.println(" blocs de 16 octets.") ; 
 String cipher_data = (char *)(cipher); 
 Serial.println("");
 
 Serial.println("-------------- Sortie en hexadecimal --------------") ;
 Print_Debug_HEX("Initialisation Vector : ", my_iv,   N_BLOCK);
 Print_Debug_HEX("AES Key               : ", key_128, key_length>>3); 
 Print_Debug_HEX("Message Original      : ", plain,   nb_block_data*N_BLOCK);
 Print_Debug_HEX("Message Chiffré       : ", cipher,  nb_block_data*N_BLOCK); 
 Print_Debug_HEX("Message déchiffré     : ", check,   nb_block_data*N_BLOCK);
 Serial.println() ;

 delete[] plain; 
 delete[] cipher; 
 delete[] check; 
}
