//biblioteki: we/wy uC; opo?nienia czasowe; LCD HD44780
#include <avr/io.h>
#include <util/delay.h>
#include "HD44780.h"

//zdefiniowanie wej?cia przetwornika ADC
#define ADCIN PA5 	

//zdefiniowanie zmiennych: pozycjonowanie, tryb1/2, start, iteracje i/j/l, liczba obrotow, //czujnikow odbiciowych, regulacja nastaw, kursor	
int sync = 0;			
int tryb1 = 0;		
int tryb2 = 0;		
int start = 0;		
int i = 0;					
int j = 0;				
int j1 = 0;
int l = 0;				
int kr = 5;			
int c1 = 0;			
int c2 = 0;			
int reg = 0;		
int regp = 0;		
int regl = 0;	
int s = 0;			
char kroki[3];			
char krokik[3];					
char predkosc[3];	
char predkosc1[4];	
char kroki1[3];				
char lk[3];

int main(void)
{

//zdefiniowanie rejestrow wej??/wyj??
	DDRB = 0x40; 				
	DDRD = 0xFC; 						
	DDRA = 0xFF;       
	DDRA &=~ (1<<ADCIN);    	        		

	//inicjalizacja wy?wietlacza
	LCD_Initalize();
	LCD_GoTo(4,1);      
 	LCD_WriteText("ISKIERNIK");
   	_delay_ms(1000);
	LCD_GoTo(0,2);
   	LCD_WriteText("Kaza & Krajewski");
   	_delay_ms(2000);
    	LCD_Clear();

//wyzerowanie lub podci?gni?cie pod "1" wej??/wyj??
	PORTB = 0x3F; 		
	PORTD = 0xC3;			
	PORTA = 0x00;		

//zdefiniowanie zmiennych cz?stotliwo?ci sygna?ow steruj?cych silnikami oraz najwa?niejsze //obliczenia
	float fz = 80;		
	float k = 50/12; 			
	float pn1 = 80;
	int ppn1 = round(pn1);
	float pn2 = pn1/k; 		
	int ppn2 = round(pn2);
	float pp = 20;		
	float fp = (pp/60)*400;			
	float fn1 = (pn1/60)*400;
	float fn2 = (pn2/60)*400;
	float f1 = 400;			 
	float p1 = (f1/400)*60; 	
	int pp1 = round(p1);
	float f2 = f1/k; 			
	float p2 = (f2/400)*60; 	
	int pp2 = round(p2);

//Uruchomienie ADC, wewn?trzne napiecie odniesienia, tryb pojedynczej konwersji, preskaler //128, wej?cie PIN5, wynik do prawej
	ADCSRA = (1<<ADEN)|(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);
	ADMUX  =  (1<<REFS1)|(1<<REFS0)|(1<<MUX2)|(1<<MUX0); 

//p?tla g?ownego programu
	while(1)
	{
		if(!(PINB & (1<<3)))				//przelacznik TRYB: AUTO / REKA
		{_delay_ms(10);tryb1 = 1;tryb2 = 0;LCD_Clear();s = 0;}

		if(!(PINB & (1<<4)))				//przelacznik TRYB: REKA / AUTO
		{_delay_ms(10);tryb2 = 1;tryb1 = 0;LCD_Clear();s = 0;}

//TRYB AUTO
		while(tryb2 == 1)											LCD_GoTo(5,0);      
			LCD_WriteText("Manual");
			LCD_GoTo(0,1);      
			LCD_WriteText("START");
			LCD_GoTo(0,2);      
			LCD_WriteText("POSITIONING");
			LCD_GoTo(0,3);      
			LCD_WriteText("SETTINGS");
//przycisk "DOWN"
			while(!(PIND &0x40))
			{
				_delay_ms(300);
				s = s - 1;
				if(s<0){s=2;}
			}
//przycisk "UP"
			while(!(PIND &0x80))
			{
				_delay_ms(300);
				s = s + 1;
				if(s>2){s=0;}
			}
//przemieszczanie kursora
			if(s==0)
			{
				LCD_GoTo(15,1);      
				LCD_WriteText("<");
				LCD_GoTo(15,2);      
				LCD_WriteText(" ");
				LCD_GoTo(15,3);      
				LCD_WriteText(" ");
			}
			if(s==1)
			{
				LCD_GoTo(15,1);      
				LCD_WriteText(" ");
				LCD_GoTo(15,2);      
				LCD_WriteText("<");
				LCD_GoTo(15,3);      
				LCD_WriteText(" ");
			}
			if(s==2)
			{
				LCD_GoTo(15,1);      
				LCD_WriteText(" ");
				LCD_GoTo(15,2);      
				LCD_WriteText(" ");
				LCD_GoTo(15,3);      
				LCD_WriteText("<");
			}
//podprogram wystartowania silnikow
			if(((s==0)&&(!(PIND &0x01))))
			{
				start = 1;
				i = 0;
				LCD_Clear();
				LCD_GoTo(0,0);      
			  	LCD_WriteText("Checking...");
				LCD_GoTo(0,2);      
			   	LCD_WriteText("1st motor:");
				LCD_GoTo(0,3);      
			 	LCD_WriteText("2nd motor:");
			  	_delay_ms(1000);
			}
			else
			{start = 0;i = 0;}
//podprogram pozycjonowania silnikow
			if(((s==1)&&(!(PIND &0x01)))) 					
			{
				sync = 1;
				i = 0;
				LCD_Clear();
				LCD_GoTo(0,0);      
			   	LCD_WriteText("Checking...");
				LCD_GoTo(0,2);      
			  	LCD_WriteText("1st motor:");
				LCD_GoTo(0,3);      
			   	LCD_WriteText("2nd motor:");
			    	_delay_ms(1000);
			}
			else
			{sync = 0;i = 0;}
//przej?cie do ustawie?
			if(((s==2)&&(!(PIND &0x01))))
			{
				_delay_ms(300);
				s = 0;
				reg = 1;
				LCD_Clear();
			}
			else
			{reg = 0;}
			if(!(PINB &0x08))	
//prze??cznik TRYB: AUTO / REKA
			{_delay_ms(10);tryb1 = 1;tryb2 = 0;LCD_Clear();s = 0;}

			while(reg == 1)
			{
				LCD_GoTo(4,0);      
			   	LCD_WriteText("Settings");
				LCD_GoTo(0,2);      
			  	LCD_WriteText("R.P.M.:    ");
			   	LCD_WriteText(predkosc1);
				LCD_GoTo(0,3);      
			    	LCD_WriteText("TURNS :    ");
			   	LCD_WriteText(kroki1);
//przej?cie do ustawie? pr?dko?ci obrotowej
				if(((s==0)&&(!(PIND &0x01))))
				{
					_delay_ms(300);
					regp = 1;
					LCD_Clear();
					LCD_GoTo(3,0);     
				    	LCD_WriteText("Set r.p.m.");
					LCD_GoTo(0,2);      
				   	LCD_WriteText("R.P.M.:    ");
				}
//przej?cie do ustawie? liczby obrotow
				if(((s==1)&&(!(PIND &0x01))))
				{
					_delay_ms(300);
					regl = 1;
					LCD_Clear();
					LCD_GoTo(0,0);    
				   	LCD_WriteText("Number of Turns");
					LCD_GoTo(0,2);
				  	LCD_WriteText("TURNS :   ");
				}
				while(regl == 1)
				{
					if(!(PIND &0x40))
					{_delay_ms(300);kr = kr + 10;}
					if(kr<20){kr=20;}
					if(!(PIND &0x80))
					{_delay_ms(300);kr = kr - 10;}
					if(kr>300){kr=300;}
					if(kr<99)
					{
						LCD_GoTo(12,2);
						itoa(kr, kroki1, 10);
						LCD_WriteText(kroki1);
						LCD_GoTo(14,2);
						LCD_WriteText(" ");
					}
					if(kr>99)
					{
						LCD_GoTo(12,2);
						itoa(kr, kroki1, 10);
						LCD_WriteText(kroki1);
					}
					if(!(PIND &0x01))
					{
						_delay_ms(500);
						regl = 0;
						LCD_Clear();
					}
				}
//opcje zmiany pr?dko?ci obrotowej silnikow (przetwornik ADC)
				while(regp == 1)
				{
					ADCSRA |= (1<<ADSC);
					while(ADCSRA & (1<<ADSC));
					fz = ADCW;
					f1 = 400 + (fz/2);
					f2 = f1/k;
					p1 = (f1/400)*60;
					pp1 = round(p1);
					_delay_ms(250);
				   	 if(pp1>99)
				    {
						LCD_GoTo(12,2);
						itoa(pp1, predkosc1, 10);
					  	LCD_WriteText(predkosc1);
				    }
				    if(pp1<100)
				    {
						LCD_GoTo(12,2);
						itoa(pp1, predkosc1, 10);
					  	LCD_WriteText(predkosc1);
					  	LCD_GoTo(14,2);
					    	LCD_WriteText(" ");
				    }
				    if(!(PIND &0x01))
				    {
				    	_delay_ms(500);
				    	regp = 0;
				    	LCD_Clear();
				    }
				}
				while(!(PIND &0x40))
				{
					_delay_ms(300);
					s = s - 1;
					if(s<0){s=2;}
				}
				while(!(PIND &0x80))
				{
					_delay_ms(300);
					s = s + 1;
					if(s>1){s=0;}
				}
//przemieszczanie kursora
				if(s==0)
				{
					LCD_GoTo(15,1); 
					LCD_WriteText(" ");
					LCD_GoTo(15,2);
					LCD_WriteText("<");
					LCD_GoTo(15,3);     
					LCD_WriteText(" ");
				}
				if(s==1)
				{
					LCD_GoTo(15,1);
					LCD_WriteText(" ");
					LCD_GoTo(15,2);      
					LCD_WriteText(" ");
					LCD_GoTo(15,3);      
					LCD_WriteText("<");
				}

				if(!(PIND &0x02))
			    {
			    	_delay_ms(300);
			    	reg = 0;
			    	LCD_Clear();
			    }
			}
//podprogram wystartowania silnikow
			while(start==1)
			{
				LCD_GoTo(0,0);      
			  	LCD_WriteText("Checking...");
				LCD_GoTo(0,2);      
			    	LCD_WriteText("1st motor:");
				LCD_GoTo(0,3);      
			    	LCD_WriteText("2nd motor:");

				while(i==0)		
				{
					TCCR1B = (1<<CS10)|(1<<WGM12); 	
					TCCR1A = (1<<COM1A0); 		
					OCR1A = (((F_CPU/fp)/2))-1; 
					TCCR3B = (1<<CS30)|(1<<WGM32);	
					TCCR3A = (1<<COM3A0); 		
					OCR3A = (((F_CPU/fp)/2))-1; 	
					PORTD |= (1<<PD2)|(1<<PD3);	
					i = 1;
				}
//wykrycie pozycji silnika 1. - czujnik 1.
				if(!(PINB &0x02)) 		
				{
					PORTD &= ~(1<<PD2); 		
					c1 = 1;
					LCD_GoTo(12,2);   
				    	LCD_WriteText("OK");
				}
//wykrycie pozycji silnika 2. - czujnik 2.
				if(!(PINB &0x04)) 	
				{
					PORTD &= ~(1<<PD3);	
					c2 = 1;
					LCD_GoTo(12,3);  
				    	LCD_WriteText("OK");
				}
//zako?czenie pozycjonowania silnikow
				if((c1 == 1) & (c2 == 1))	
				{
					_delay_ms(1000);
					c1 = 0;
					c2 = 0;
					i = 0;
					LCD_Clear();
                    			LCD_GoTo(3,1);
                    			LCD_WriteText("POSITIONING");
                   			LCD_GoTo(4,2);
                    			LCD_WriteText("COMPLETED");
					_delay_ms(1500);
					LCD_Clear();
//wpisanie ustawie? timerow
					while(i==0)	
					{
						PORTD |= (1<<PD2)|(1<<PD3);
						TCCR1B = (1<<CS10)|(1<<WGM12); 
						TCCR1A = (1<<COM1A0); 
						OCR1A = (((F_CPU/f1)/2))-1; 
						TCCR3B = (1<<CS30)|(1<<WGM32);
						TCCR3A = (1<<COM3A0); 	
						OCR3A = (((F_CPU/f2)/2))-1; 
						i = 1;
					}
//podprogram pracy silnikow
					while(j<kr)
					{
					    LCD_GoTo(5,0);   
					    LCD_WriteText("Manual");
					    LCD_GoTo(0,1);    
					    itoa(j, lk, 10);
					    LCD_WriteText("t.:  ");
					    LCD_WriteText(lk);
					    LCD_GoTo(8,1);
					    LCD_WriteText("/");
					    itoa(kr, kroki1, 10);
					    LCD_WriteText(kroki1);
					    LCD_GoTo(0,2);  
					    LCD_WriteText("rpm: ");
					    itoa(pp1, predkosc1, 10);
					    LCD_WriteText(predkosc1);
					    LCD_GoTo(9,2);
					    LCD_WriteText("rev/min");
					    LCD_GoTo(2,3);  
					    LCD_WriteText("--> STOP <--");
//zatrzymanie silnikow
						PORTD |= (1<<PD2)|(1<<PD3);	
						while(!(PIND &0x02))	
						{l = 1;}
							while(l == 1)
							{
								PORTD &= ~(1<<PD2); 	
								PORTD &= ~(1<<PD3);										_delay_ms(10);
//ponowne wystartowanie silnikow
								while(!(PIND &0x01))
								{
									l = 0;
									PORTD |= (1<<PD2)|(1<<PD3);
								}
//wyj?cie z podprogramu pracy silnikow
								while(!(PIND &0x02))
								{start = 0;i = 0;j = 5;l = 0;}
							}
//zliczanie liczby obrotow
						while(!(PINB &0x02))
						{
							_delay_ms(150);
							j=j+1;
						}
					}
//zako?czenie podprogramu pracy silnikow
					if(j==kr)
					{
						start = 0;
						i = 0;
						j = 0;
						PORTD &= ~(1<<PD2); 
						PORTD &= ~(1<<PD3);	
                        LCD_Clear();
                        LCD_GoTo(5,1);
                        LCD_WriteText("FINISH");
                        _delay_ms(1000);
                        LCD_Clear();
					}
				}
			}

			while(sync==1)	
			{
				while(i==0)	
				{
					PORTD |= (1<<PD2)|(1<<PD3);
					TCCR1B = (1<<CS10)|(1<<WGM12); 
					TCCR1A = (1<<COM1A0); 	
					OCR1A = (((F_CPU/fp)/2))-1; 
					TCCR3B = (1<<CS30)|(1<<WGM32);								TCCR3A = (1<<COM3A0); 									OCR3A = (((F_CPU/fp)/2))-1;									i = 1;
				}

				if(!(PINB &0x02)) 	
				{
					PORTD &= ~(1<<PD2); 	
					c1 = 1;
					LCD_GoTo(12,2);   
				   	LCD_WriteText("OK");
				}

				if(!(PINB &0x04)) 			
				{
					PORTD &= ~(1<<PD3);	
					c2 = 1;
					LCD_GoTo(12,3);    
				   	LCD_WriteText("OK");
				}

				if((c1 == 1) & (c2 == 1))	
				{
					_delay_ms(1000);
					sync = 0;
					c1 = 0;
					c2 = 0;
					i = 0;
                    LCD_Clear();
                    LCD_GoTo(3,1);
                    LCD_WriteText("POSITIONING");
                    LCD_GoTo(4,2);
                    LCD_WriteText("COMPLETED");
                    _delay_ms(1500);
                    LCD_Clear();
				}
			}
		}

		while(tryb1 == 1)	
		{
			LCD_GoTo(4,0);    
			LCD_WriteText("Automatic");
			LCD_GoTo(0,1);
			LCD_WriteText("START");
			LCD_GoTo(0,2);
			LCD_WriteText("POSITIONING");

			while(!(PIND &0x40))
			{
				_delay_ms(300);
				s = s - 1;
				if(s<0){s=2;}
			}
			while(!(PIND &0x80))
			{
				_delay_ms(300);
				s = s + 1;
				if(s>1){s=0;}
			}

			if(s==0)
			{
				LCD_GoTo(15,1);      
				LCD_WriteText("<");
				LCD_GoTo(15,2);      
				LCD_WriteText(" ");
				LCD_GoTo(15,3);      
				LCD_WriteText(" ");
			}
			if(s==1)
			{
				LCD_GoTo(15,1);     
				LCD_WriteText(" ");
				LCD_GoTo(15,2);      
				LCD_WriteText("<");
				LCD_GoTo(15,3);      
				LCD_WriteText(" ");
			}

			if(!(PINB &0x10))	
			{_delay_ms(10);tryb2 = 1;tryb1 = 0;LCD_Clear();s = 0;}

			if(((s==0)&&(!(PIND &0x01))))	
			{
				start = 1;
				i = 0;
				LCD_Clear();
				LCD_GoTo(0,0);      
			    	LCD_WriteText("Checking...");
				LCD_GoTo(0,2);    
			   	LCD_WriteText("1st motor:");
				LCD_GoTo(0,3);      
			    	LCD_WriteText("2nd motor:");
			   	_delay_ms(1000);
			}
			else
			{start = 0;i = 0;}

			if(((s==1)&&(!(PIND &0x01)))) 
			{
				sync = 1;
				i = 0;
				LCD_Clear();
				LCD_GoTo(0,0);      
			    	LCD_WriteText("Checking...");
				LCD_GoTo(0,2);      
			   	LCD_WriteText("1st motor:");
				LCD_GoTo(0,3);     
			    	LCD_WriteText("2nd motor:");
			   	_delay_ms(1000);
			}
			else
			{sync = 0;i = 0;}

			while(start==1)
			{
				LCD_GoTo(0,0);      
			   	LCD_WriteText("Checking...");
				LCD_GoTo(0,2);      
			    	LCD_WriteText("1st motor:");
				LCD_GoTo(0,3);      
			    	LCD_WriteText("2nd motor:");

				while(i==0)	
				{
					TCCR1B = (1<<CS10)|(1<<WGM12); 	
					TCCR1A = (1<<COM1A0); 		
					OCR1A = (((F_CPU/fp)/2))-1; 	
					TCCR3B = (1<<CS30)|(1<<WGM32);	
					TCCR3A = (1<<COM3A0); 									OCR3A = (((F_CPU/fp)/2))-1; 	
					PORTD |= (1<<PD2)|(1<<PD3);	 
					i = 1;
				}

				if(!(PINB &0x02)) 					{
					PORTD &= ~(1<<PD2); 										c1 = 1;
					LCD_GoTo(12,2);     
				    	LCD_WriteText("OK");
				}

				if(!(PINB &0x04)) 						{
					PORTD &= ~(1<<PD3);										c2 = 1;
					LCD_GoTo(12,3);    
				    	LCD_WriteText("OK");
				}

				if((c1 == 1) & (c2 == 1))	
				{
					_delay_ms(1000);
					c1 = 0;
					c2 = 0;
					i = 0;
					LCD_Clear();
                    			LCD_GoTo(3,1);
                    			LCD_WriteText("POSITIONING");
                    			LCD_GoTo(4,2);
                    			LCD_WriteText("COMPLETED");
					_delay_ms(1500);
					LCD_Clear();

					while(i==0)	
					{
						PORTD |= (1<<PD2)|(1<<PD3);									TCCR1B = (1<<CS10)|(1<<WGM12); 
						TCCR1A = (1<<COM1A0); 
						OCR1A = (((F_CPU/fn1)/2))-1; 
						TCCR3B = (1<<CS30)|(1<<WGM32);	
						TCCR3A = (1<<COM3A0); 	
						OCR3A = (((F_CPU/fn2)/2))-1; 
						i = 1;
					}

					while(j<100)
					{
						LCD_GoTo(4,0);    
					    	LCD_WriteText("Automatic");
						LCD_GoTo(0,1);   
						itoa(j, kroki, 10);
					    	LCD_WriteText("t. :  ");
					    	LCD_WriteText(kroki);
					    	LCD_GoTo(9,1);
					    	LCD_WriteText("/100");
						LCD_GoTo(0,2);  
					    	LCD_WriteText("rpm:  ");
					    	itoa(ppn1, predkosc, 10);
					    	LCD_WriteText(predkosc);
					   	LCD_GoTo(9,2);
					    	LCD_WriteText("rev/min");
						LCD_GoTo(2,3);   
					    	LCD_WriteText("--> STOP <--");

						PORTD |= (1<<PD2)|(1<<PD3);	
						while(!(PIND &0x02))	
						{l = 1;}
						while(l == 1)
							{
								PORTD &= ~(1<<PD2); 		
								PORTD &= ~(1<<PD3);	
								_delay_ms(10);

								while(!(PIND &0x01))
								{
									l = 0;
									PORTD |= (1<<PD2)|(1<<PD3);
								}

								while(!(PIND &0x02))
								{start = 0;i = 0;j = 100;l = 0;}
							}
						while(!(PINB &0x02))
						{
							_delay_ms(300);
							j=j+1;
						}
					}
					if(j==100)
					{
						start = 0;
						i = 0;
						j = 0;
						PORTD &= ~(1<<PD2); 			
						PORTD &= ~(1<<PD3);	
						_delay_ms(1000);
						j1 = j + 1;
						itoa((j1), krokik, 10);
						LCD_GoTo(5,1);
						LCD_WriteText(krokik);
                        LCD_Clear();
                        LCD_GoTo(5,1);
                        LCD_WriteText("FINISH");
                        _delay_ms(1000);
                        LCD_Clear();
					}
				}
			}

			while(sync==1)		
			{
				while(i==0)	
				{
					PORTD |= (1<<PD2)|(1<<PD3);
					TCCR1B = (1<<CS10)|(1<<WGM12); 
					TCCR1A = (1<<COM1A0); 	
					OCR1A = (((F_CPU/fp)/2))-1;
					TCCR3B = (1<<CS30)|(1<<WGM32);								TCCR3A = (1<<COM3A0); 									OCR3A = (((F_CPU/fp)/2))-1;
					i = 1;
				}

				if(!(PINB &0x02)) 		
				{
					PORTD &= ~(1<<PD2); 		
					c1 = 1;
					LCD_GoTo(12,2);   
				    	LCD_WriteText("OK");
				}

				if(!(PINB &0x04)) 	
				{
					PORTD &= ~(1<<PD3);	
					c2 = 1;
					LCD_GoTo(12,3);    
				    	LCD_WriteText("OK");
				}

				if((c1 == 1) & (c2 == 1))			
				{
					_delay_ms(1000);
					sync = 0;
					c1 = 0;
					c2 = 0;
					i = 0;
                    			LCD_Clear();
                    			LCD_GoTo(3,1);
                   			LCD_WriteText("POSITIONING");
                   			LCD_GoTo(4,2);
                   			LCD_WriteText("COMPLETED");
                    			_delay_ms(1500);
                    			LCD_Clear();
				}
			}
		}
	}
}
