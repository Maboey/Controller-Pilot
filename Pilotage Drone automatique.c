/*===========================================================================*=
   CFPT - Projet : Pilotage manette
   Auteur        : Martin Manuel
   Date creation : 29.02.20
  =============================================================================
   Descriptif: Ce Logiciel vient remplacer deux joysticks d'une télécommande
	par 4 PWM (2 par joystick). dans un premier temps dans l'idée de piloter un
	drone en connectant les 4 signaux PWM à la place des joysticks sur la 
	télécommande.
	(Les PWM nécessitent un filtre RC afin de générer une tension analogique 
	continue proportionelle au PWM et pas un signal carré)
=*===========================================================================*/
#include <reg51f380.h>     // registres 51f38C
#include "InterruptPriorities.h"

// ==== Fonctions prototypes ==================================================
void ClockInit ();      // init. clock systeme
void PortInit ();       // init. config des ports
void TimerInit ();	   	// init. timers

// direction joysticks
void Joystick (unsigned char gauche, unsigned char droite); 

// ==== Fonctions prototypes ==================================================
#define LOADVALUE 20

// directions drone
#define HAUT 0
#define BAS 1
#define GAUCHE 2
#define DROITE 3
#define CALIBRATION 4
#define REPOS	5
#define CLICKON 6
#define CLICKOFF 7

// ==== Variables Globales ====================================================
bit flagTimer = 0;

//--------- x% Joysticks Potentiometres --------------
unsigned char Joystick_Gauche_Vertical = 50, Joystick_Gauche_Horizontal = 50,
	Joystick_Droite_Vertical = 50, Joystick_Droite_Horizontal = 10;

//------------- Joysticks Buttons ------------------
bit joystick_Gauche_Click = 0, joystick_Droite_Click = 0;

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*                                  MAIN                                       */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
void main ()
{
	//--- Variables locales ----------------------------------------------------
	unsigned char periodePWM = 0; //(va de 0 - 100)
	unsigned int compteurSeconde = 0; 
	unsigned char compteurMiliSeconde = 0, compteurAction;
	bit run = 1, flagSeconde;
	

	
	//--------- PWM Joysticks ----------------------------
												 //			 +---- PWM Joystick Gauche Vertical
												 //			 |+--- PWM Joystick Gauche Horizontal
												 //			 ||+-- PWM Joystick Droite Vertical
												 //			 |||+- PWM Joystick Droite Horizontal
												 //			 ||||
	unsigned char pwm = 0; // xxxx xxxx 4 PWM


	
	//--- Initialisation uC-----------------------------------------------------
	PCA0MD &= ~0x40;     // WDTE = 0 (disable watchdog timer)
	ClockInit ();        // init. clock systeme
	PortInit ();         // init. config des ports 
	TimerInit ();				 // init. timers
	TR0 = 1;						 // lance le timer 0
	
	//----Boucle infinie--------------------------------------------------------	
	while (run)
	{
		if(flagSeconde)
		{
			flagSeconde = 0;
			switch (compteurAction)
      {
				// test
				case 0 : Joystick (HAUT,HAUT);
      		break;
      	case 1 : Joystick (BAS,BAS);
      		break;
      	case 2 : Joystick (GAUCHE,GAUCHE);
      		break;
				case 3 : Joystick (DROITE,DROITE);
      		break;
				case 4 : Joystick (REPOS,REPOS);
      		break;
      	case 5 : Joystick (CALIBRATION,CALIBRATION);
      		break;
				case 6 : Joystick (CLICKON,CLICKON);
      		break;
				case 7 : Joystick (CLICKOFF,CLICKOFF);
      		break;
				case 8 : compteurAction = 0;
      		break;
				
				/* // Demarrage drone
      	case 0 : Joystick (HAUT,REPOS);
      		break;
      	case 1 : Joystick (BAS,REPOS);
      		break;
      	case 2 : Joystick (CALIBRATION,CALIBRATION);
      		break;
				case 3 : Joystick (CLICKON,REPOS); // décollage drone
      		break;
				case 4 : Joystick (CLICKOFF,REPOS); 
      		break;
				case 5 : run = 0; // met fin au programme après toutes les actions
      		break;*/
      }
			compteurAction ++;
			
		}
		//chaque 10us
		if(flagTimer)
		{
			//----- compteur base de temps ----
			
			if(compteurMiliSeconde <= 100)
			{
				compteurMiliSeconde++;
			}
			else
			{
				compteurMiliSeconde = 0;
				compteurSeconde ++;
			}
			
			if(compteurSeconde > 1000)
			{
				compteurSeconde = 0;
				flagSeconde = 1;
			}

			
			// --- mise a jour periode PWM ----
			periodePWM ++;
			if(periodePWM>100)
			{
				periodePWM = 0;
			}
			
			// --- mise a jour valeurs PWM ----
			if(Joystick_Gauche_Vertical >= periodePWM)
			{
				pwm |= 0x08; //PWM Joystick Gauche Vertical force a 1
			}
			else
			{
				pwm &= 0xF7; //PWM Joystick Gauche Vertical force a 0
			}
			
			if(Joystick_Gauche_Horizontal >= periodePWM)
			{
				pwm |= 0x04; //PWM Joystick Gauche Vertical force a 1
			}
			else
			{
				pwm &= 0xFB; //PWM Joystick Gauche Vertical force a 0
			}
			
			if(Joystick_Droite_Vertical >= periodePWM)
			{
				pwm |= 0x02; //PWM Joystick Gauche Vertical force a 1
			}
			else
			{
				pwm &= 0xFD; //PWM Joystick Gauche Vertical force a 0
			}
			
			if(Joystick_Droite_Horizontal >= periodePWM)
			{
				pwm |= 0x01; //PWM Joystick Gauche Vertical force a 1
			}
			else
			{
				pwm &= 0xFE; //PWM Joystick Gauche Vertical force a 0
			}
			
			
			// on met à jour la valeur des boutons des joysticks et du pwm
			if(joystick_Gauche_Click)
			{
				P0 = (pwm |= 0x10);
			}
			else
			{
				P0 = (pwm &= 0xEF);
			}
			if(joystick_Droite_Click)
			{
				P0 = (pwm |= 0x20);
			}
			else
			{
				P0 = (pwm &= 0xDF);
			}
			
		}
		
	}// End while (1)
}   /*%%%%%%%%%%%%%%%%%%%%%%%%%%%% END MAIN %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%Routine d'initialisation uC%%%%%%%%%%%%%%%%%%%%%%%%%%%


/*---------------------------------------------------------------------------*-
   Joystick ()
  -----------------------------------------------------------------------------
   Descriptif: fonction simulant le fonctionnement de deux joysticks
   Entree    : gauche (HAUT,BAS,GAUCHE,DROITE,CALIBRATIONDRONE,CLICKON,CLICKOFF)
							 droite (HAUT,BAS,GAUCHE,DROITE,CALIBRATIONDRONE,CLICKON,CLICKOFF)
   Sortie    : --
-*---------------------------------------------------------------------------*/
void Joystick (unsigned char gauche, unsigned char droite)
{
	switch (gauche)
  {
  	case HAUT : Joystick_Gauche_Vertical = 100; 
  		break;
  	case BAS : Joystick_Gauche_Vertical = 0;
  		break;
  	case GAUCHE : Joystick_Gauche_Horizontal = 0;
  		break;
		case DROITE : Joystick_Gauche_Horizontal = 100;
  		break;
		case CALIBRATION : Joystick_Gauche_Horizontal = 100; Joystick_Gauche_Vertical = 0;
  		break;
		case REPOS : Joystick_Gauche_Horizontal = 50; Joystick_Gauche_Vertical = 50;
  		break;
		case CLICKON : joystick_Gauche_Click = 1;
  		break;
		case CLICKOFF : joystick_Gauche_Click = 0;
  		break;
  }
	switch (droite)
  {
  	case HAUT : Joystick_Droite_Vertical = 100; 
  		break;
  	case BAS : Joystick_Droite_Vertical = 0;
  		break;
  	case GAUCHE : Joystick_Droite_Horizontal = 0;
  		break;
		case DROITE : Joystick_Droite_Horizontal = 100;
  		break;
		case CALIBRATION : Joystick_Droite_Horizontal = 0; Joystick_Droite_Vertical = 0;
  		break;
		case REPOS : Joystick_Droite_Horizontal = 50; Joystick_Droite_Vertical = 50;
  		break;
		case CLICKON : joystick_Droite_Click = 1;
  		break;
		case CLICKOFF : joystick_Droite_Click = 0;
  		break;
  }
}

/*---------------------------------------------------------------------------*-
   FonctionDInterruptionDuTimer ()
  -----------------------------------------------------------------------------
   Descriptif: Fonction effectuée quand le timer 0 déclenche son interruption
   Entree    : --
   Sortie    : --
-*---------------------------------------------------------------------------*/
void FonctionDInterruptionDuTimer() interrupt INTERRUPT_TIMER0
{
	flagTimer = 1;
}

/*---------------------------------------------------------------------------*-
   TimerInit ()
  -----------------------------------------------------------------------------
   Descriptif: Initialisation du timers 0 
      
   Entree    : --
   Sortie    : --
-*---------------------------------------------------------------------------*/

void TimerInit ()
{
  TR0 = 0;       // stop le timer TR0 mettre avant la boucle while
  ET0 = 0;       // Inhibe l'interruption du timer 0  
 //---mode de ofnctionnement du timer----------------------------------------*/   
  TMOD &= 0xF0;
              // ++++----- Timer1-->meme schema que pour timer 0   
              // ||||+---- timer0 gate control 
              // |||||     0: start soft
              // |||||     1: start externe   
              // |||||+--- Counter or timer select--> 1: counter | 0: timer
              // ||||||++- timer0 mode select  
              // ||||||||  00: mode0 13-bit Counter/timer
              // ||||||||  01: mode1 16-bit Counter/timer
              // ||||||||  10: mode2 8-bit Counter/timer with auto-reload
              // ||||||||  11: mode3 deux 8-bit Counter/timer(pas utilise)
  TMOD  |=0x02;//00000010
  
  CKCON &= ~0x03;// masque les bit pas utilise
              // ++++----- timer 2 et 3 clock select (pas utilise)   
              // ||||+---- clock1 select-->  1: sysclock | 0: prescaled clock 
              // |||||+--- clock0 select-->  1: sysclock | 0: prescaled clock   
              // ||||||++- Predivision du clock   
              // |||||||| 00: Sysclock / 12 | 01: Sysclock / 4 
              // |||||||| 10: Sysclock / 48 | 11: Extclock / 8 
  CKCON |=0x01;//00000001
  
  TH0 = TL0 = LOADVALUE;
  
  TF0 = 0; //clear une interruption residuelle
  EA = 1; // autorisation générale des interruptions
  ET0 = 1; //autorise interuption du timer 0
}

/*---------------------------------------------------------------------------*-
   ClockInit ()
  -----------------------------------------------------------------------------
   Descriptif: Initialisation du mode de fonctionnement du clock systeme 
         choix : SYSCLK : oscillateur HF interne a 48 MHz

   Entree    : --
   Sortie    : --
-*---------------------------------------------------------------------------*/

void ClockInit()
{  
                     // +--------- clock interne LF
                     // | (1 : oscillateur LF : enable)
                     // | (0 : oscillateur LF: desable)
                     // |+-------- en lecture seule 1 : signal que oscillateur 
                     // ||         interne fonctionne a sa valeur de prog.
                     // ||++++---- reglage fin de la frequence de l'osc. LF
                     // ||||||++-- choix du diviseur :
                     // ||||||||       (00 : Osc LF /8 -> f = 10 KHz)
                     // ||||||||       (01 : Osc LF /4 -> f = 20 KHz)
                     // ||||||||       (10 : Osc LF /2 -> f = 40 KHz)
                     // ||||||||       (11 : Osc LF /1 -> f = 80 KHz)
   OSCLCN = 0x00;    // 00000000 

                     // +--------- non utilise
                     // |+++------ Selection du clock USB 
                     // ||||           (010 : Oscil ext. : limiter la conso.)
                     // ||||+----- clock out select
                     // |||||          (0 : sortie sysclk non synchronisee)
                     // |||||          (1 : sortie sysclk synchronisee)
                     // |||||+++-- choix du clock systeme
                     // ||||||||       (000 : Oscil interne 48/4  --> 1.5, 3, 6
                     // ||||||||              ou 12 MHz selon le choix du 
                     // ||||||||              diviseur dans OSCICN
                     // ||||||||       (001 : Oscil externe = x  MHz)
                     // ||||||||       (010 : Oscil interne HF 48/2 = 24 MHz)
                     // ||||||||       (011 : Oscil interne HF 48/1 = 48 MHz)    
                     // ||||||||       (100 : Oscil interne LF = 80 KHz max)   
                     // ||||||||       (101 a 111 : reserves)   
   CLKSEL = 0x23;    // 00100011  

                     // +--------- clock interne HF
                     // |              (1 : oscillateur HF : enable)
                     // |              (0 : oscillateur HF: desable)
                     // |+-------- en lecture seule 1 : signal que oscillateur 
                     // ||              interne fonctionne a sa valeur de prog.
                     // ||+------- 1 : suspend l'oscillateur interne
                     // |||+++---- non utilises
                     // ||||||++-- choix du diviseur :
                     // ||||||||       (00 : 12/8 -> f =  1.5 MHz)
                     // ||||||||       (01 : 12/4 -> f =  3   MHz)
                     // ||||||||       (10 : 12/2 -> f =  6   MHz)
                     // ||||||||       (11 : 12/1 -> f = 12   MHz)
   OSCICN = 0x83;    // 10000011 

   FLSCL = 0x90;     // A utiliser si le clock system est a 48 MHz

} // ClockInit ----------------------------------------------------------------

/*---------------------------------------------------------------------------*-
   PortInit ()
  -----------------------------------------------------------------------------
   Descriptif: Initialisation du mode de fonctionnement des ports 
   Entr�e    : --
   Sortie    : --
-*---------------------------------------------------------------------------*/
void PortInit () 
{
   P0MDOUT = 0xFF;      // port P0 en sortie numerique (push-pull)
   XBR1   |= 0x40;      // autorise le fonctionnement du crossbar

}// PortInit ----------------------------------------------------------------