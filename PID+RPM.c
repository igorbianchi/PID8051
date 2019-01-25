sbit P1_0 = 0x90;

sbit P0_0 = 0x80;

sbit P3_0 = 0x0B0;
sbit P3_1 = 0x0B1;



#include <reg51.h>


int i;
int j;
int k;
int contador;
int nums[10]={0,1,2,3,4,5,6,7,8,9};  //vetor de 9 digitos para o 7 segmentos
int digito[3] = {0,0,0}; // vetor para armazenar os 3 digitos do display da simulacao

void delay(void);
void ex0_isr (void) interrupt 0
{
     contador++;   // conta quantas vezes a interupção é chamada, ou seja, quantas rotações o motor está fazendo
}

void passaDigito(void);

void main(void)
 {
   TMOD = 0x01; // ativa timer 0 no modo 1
   TR0 = 1; // liga o timer 0

   while (1){
         IT0 = 1; // ativa interrupção sensível a borda
         EX0 = 1; // ativa interrupção externa
         EA = 1; //ativa interupções
				for(i=0; i < 10; i++){ // repetição para o timer, 500ms, é o tempo necessário para pegar as amostras do motor
					TH0 = 0x3C;
					TL0 = 0x0AF; // carrega o timer 0 com 50ms
					TR0 = 1;
          while(TF0 == 0){
					}
					TF0 = 0;
					}
         EX0 = 0;
         passaDigito(); // funcao para enviar os valores para os displays
				 contador = 0;
					//seleciona o sentido de rotação do motor
					if(P0_0 == 1){
						 P3_0 = 1;
						 P3_1 = 0;
					}
					else{
						P3_0 = 0;
						P3_1 = 1;
					}
   }
}

 void delay(){
 int h;
 int g;
   for(h=0;h<20000;h++);
		for(g=0; h<20000;g++);

 }
 
 void passaDigito(){
			int aux;
	 // cada rotação completa do motor envia 30 pulsos de interrupção
	 // contador tem armazenado a quantidade de pulsos em 500ms
	 // se para 1 pulso por rotação em 1s fazemos *60 o contador
	 // para 30 pulsos por rotação em 0,5s temos *4 o contador
	 // o .35 é fator de atraso das instruções
			contador = contador * 4.35;
	 // pega os valores de cada digito do contador
		  if(contador>=100){
				digito[0] = contador/100;
				contador %= 100;
				digito[1] = contador/10;
				contador %= 10; 
				digito[2] = contador;
			}else if(contador >= 10){
				digito[0] = 0;
				digito[1] = contador/10;
				contador %= 10; 
				digito[2] = contador;		
			}else{
				digito[0] = 0;
				digito[1] = 0;
				digito[2] = contador;
			}
				// valores em BCD
				aux = digito[0]<<4;   //deloca 4 bits para esquerda o valor
				P1 = aux + digito[1]; // passa o valor para a porta desejada junto com o digito do meio
				P2 = digito[2];
				delay();
      
		
 }
      