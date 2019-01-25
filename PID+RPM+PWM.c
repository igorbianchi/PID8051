sbit P1_0 = 0x90; // o endereço do pino 1_0 passa a se chamar P1_0



sbit P2_4 = 0x0A4;

//bits da chave, incremento e decremento definidos nas portas P0.0, P0.1 E P0.2
sbit chave = 0x80;
sbit Inc = 0x81;
sbit Dec = 0X82;
sbit LED0 = 0x83;
sbit LED1 = 0x84;
sbit CARGA0 = 0x85;
sbit CARGA1 = 0x86;
sbit CARGA2 = 0x87;

// bits do PWM definidos nas portas P3.0 e P3.1
sbit PWM0 = 0x0B0;
sbit PWM1 = 0x0B1;

#include <reg51.h>
#include <math.h>

// parametros do pid
float Kp = 0.5, Kd = 0.1, Ki = 0.2;
int contador, direcao, timerCounter, speed, carga, erro = 0, erroAnterior = 0, integral = 0, derivativo;
int nums[10]={0,1,2,3,4,5,6,7,8,9};  //vetor de 9 digitos para o 7 segmentos
int digito[3] = {0,0,0}; // vetor para armazenar os 3 digitos do display da simulacao

// setpoint do motor 200RPM
int setPoint = 500;


void delay(int);
void passaDigito(void);
void acionaPWM(int,int);
void PID(void);

void ex0_isr (void) interrupt 0{ // interrupção externa que corresponde aos pulsos do motor
     contador++;   // conta quantas vezes a interupção é chamada, ou seja, quantas rotações o motor está fazendo
}

void timer0(void) interrupt 1{ //interrupçao do timer 0, a cada 50ms ela é chamada
		TR0 = 0;
		TH0 = 0x3C;
		TL0 = 0x0AF; // carrega o timer 0 com 50ms
		TF0 = 0;
		if(timerCounter < 9){//são contadas amostras das rotações do motor a cada 0.5s 
				timerCounter++;
		}
		else{ // quando dá os 500ms, a função de passagem de valor para os 7-seg é chamada
			timerCounter = 0;
			EX0 = 0;
			passaDigito();
			//PID();
			contador = 0;
			EX0 = 1;
		}
		TR0 = 1;
}



void main(void)
 {
   TMOD = 0x01; // ativa timer 0 no modo 1
	 TH0 = 0x3C;
	 TL0 = 0x0AF; // carrega o timer 0 com 50ms
   TR0 = 1; // liga o timer 0
	 IT0 = 1; // ativa interrupção sensível a borda
   EX0 = 1; // ativa interrupção externa
   EA = 1; //ativa interupções	 
	 ET0 = 1; //ativa interrupção para o timer 0

   while (1){ 		 
		  // o sentido de rotação é definido dependendo da posição da chave
			if(chave == 0){
					direcao = 0;
					LED0 = 0;
				  LED1 = 1;
			}
			else{
				direcao =1;
				LED0 = 1;
				LED1 = 0;
			}
		  //botoes para simular carga no motor e mostrar o PID funcionando
			if(!Inc)
      // incrementa velocidade
         speed = speed > 0 ? speed - 4 : 0;
      if(!Dec)
      // decrementa velocidade
         speed = speed < 1000 ? speed + 4 : 1000;
			
			//envia a direção definida e a velocidade
      acionaPWM(direcao, speed);
   }
}

 void delay(int h){
   for(;h>0;h--);

 }
 
 // calcula a velocidade de acordo com PID
 void PID(){
			erro = setPoint - contador;
			integral = integral + erro;
			derivativo = erro - erroAnterior;
			erroAnterior = erro;
			speed = Kp*erro + Ki*integral + Kd*derivativo;
}			
			
 
 
 // função que oscila a saída dos pinos
 // aumentando o delay, criam-se pulsos de menor largura e consequentemente velocidade menor
 // para máxima velocidade a saída deve permanecer sempre em 1
  void acionaPWM(int direcao, int velocidade){
      if (direcao == 1){
				 PWM1=0;
         PWM0=1;
         delay(velocidade);
         PWM0=0;
         delay(setPoint-velocidade);
      }
      else{
				 PWM0 = 0;
         PWM1=1;
         delay(velocidade);
         PWM1=0;
         delay(setPoint-velocidade);
      }
 }
 
 void passaDigito(){
			int aux;
	 // cada rotação completa do motor envia 30 pulsos de interrupção
	 // contador tem armazenado a quantidade de pulsos em 500ms
	 // se para 1 pulso por rotação em 1s fazemos *60 o contador
	 // para 30 pulsos por rotação em 0,5s temos *4 o contador
			contador = contador * 4;
		  PID();
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
 }
      
