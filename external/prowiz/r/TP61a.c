/* testP61A_nopack() */
/* testP61A_pack () */
/* Rip_P61A() */
/* Depack_P61A() */


#include "globals.h"
#include "extern.h"


int16_t	 testP61A_nopack ( void )
{
  if ( PW_i < 7 )
  {
    return BAD;
  }
  PW_Start_Address = PW_i-7;

  /* number of pattern (real) */
  PW_m = in_data[PW_Start_Address+2];
  if ( (PW_m > 0x7f) || (PW_m == 0) )
  {
/*printf ( "#1 Start:%ld\n" , PW_Start_Address );*/
    return BAD;
  }
  /* PW_m is the real number of pattern */

  /* number of sample */
  PW_k = (in_data[PW_Start_Address+3]&0x3F);
  if ( (PW_k > 0x1F) || (PW_k == 0) || ((PW_k*6+PW_Start_Address+7)>=PW_in_size))
  {
/*printf ( "#2 Start:%ld\n" , PW_Start_Address );*/
    return BAD;
  }
  /* PW_k is the number of sample */

  /* test volumes */
  for ( PW_l=0 ; PW_l<PW_k ; PW_l++ )
  {
    if ( in_data[PW_Start_Address+7+PW_l*6] > 0x40 )
    {
/*printf ( "#3 Start:%ld\n" , PW_Start_Address );*/
      return BAD;
    }
  }

  /* test fines */
  for ( PW_l=0 ; PW_l<PW_k ; PW_l++ )
  {
    if ( in_data[PW_Start_Address+6+PW_l*6] > 0x0F )
    {
/*printf ( "#4 Start:%ld\n" , PW_Start_Address );*/
      return BAD;
    }
  }

  /* test sample sizes and loop start */
  PW_WholeSampleSize = 0;
  for ( PW_n=0 ; PW_n<PW_k ; PW_n++ )
  {
    PW_o = ( (in_data[PW_Start_Address+4+PW_n*6]*256) +
	     in_data[PW_Start_Address+5+PW_n*6] );
    if ( ((PW_o < 0xFFDF) && (PW_o > 0x8000)) || (PW_o == 0) )
    {
/*printf ( "#5 Start:%ld\n" , PW_Start_Address );*/
      return BAD;
    }
    if ( PW_o < 0xFF00 )
      PW_WholeSampleSize += (PW_o*2);

    PW_j = ( (in_data[PW_Start_Address+8+PW_n*6]*256) +
	     in_data[PW_Start_Address+9+PW_n*6] );
    if ( (PW_j != 0xFFFF) && (PW_j >= PW_o) )
    {
/*printf ( "#5,1 Start:%ld\n" , PW_Start_Address );*/
      return BAD;
    }
    if ( PW_o > 0xFFDF )
    {
      if ( (0xFFFF-PW_o) > PW_k )
      {
/*printf ( "#5,2 Start:%ld\n" , PW_Start_Address );*/
        return BAD;
      }
    }
  }

  /* test sample data address */
  PW_j = (in_data[PW_Start_Address]*256)+in_data[PW_Start_Address+1];
  if ( PW_j < (PW_k*6+4+PW_m*8) )
  {
/*printf ( "#6 Start:%ld\n" , PW_Start_Address );*/
    return BAD;
  }
  /* PW_j is the address of the sample data */


  /* test track table */
  for ( PW_l=0 ; PW_l<(PW_m*4) ; PW_l++ )
  {
    PW_o = ((in_data[PW_Start_Address+4+PW_k*6+PW_l*2]*256)+
            in_data[PW_Start_Address+4+PW_k*6+PW_l*2+1] );
    if ( (PW_o+PW_k*6+4+PW_m*8) > PW_j )
    {
/*printf ( "#7 Start:%ld (value:%ld)(where:%x)(PW_l:%ld)(PW_m:%ld)(PW_o:%ld)\n"
, PW_Start_Address
, (in_data[PW_Start_Address+PW_k*6+4+PW_l*2]*256)+in_data[PW_Start_Address+4+PW_k*6+PW_l*2+1]
, PW_Start_Address+PW_k*6+4+PW_l*2
, PW_l
, PW_m
, PW_o );*/
      return BAD;
    }
  }

  /* test pattern table */
  PW_l=0;
  PW_o=0;
  /* first, test if we dont oversize the input file */
  if ( (PW_Start_Address+PW_k*6+4+PW_m*8) > PW_in_size )
  {
/*printf ( "8,0 Start:%ld\n" , PW_Start_Address );*/
    return BAD;
  }
  while ( (in_data[PW_Start_Address+PW_k*6+4+PW_m*8+PW_l] != 0xFF) && (PW_l<128) )
  {
    if ( in_data[PW_Start_Address+PW_k*6+4+PW_m*8+PW_l] > (PW_m-1) )
    {
/*printf ( "#8,1 Start:%ld (value:%ld)(where:%x)(PW_l:%ld)(PW_m:%ld)(PW_k:%ld)\n"
, PW_Start_Address
, in_data[PW_Start_Address+PW_k*6+4+PW_m*8+PW_l]
, PW_Start_Address+PW_k*6+4+PW_m*8+PW_l
, PW_l
, PW_m
, PW_k );*/
      return BAD;
    }
    if ( in_data[PW_Start_Address+PW_k*6+4+PW_m*8+PW_l] > PW_o )
      PW_o = in_data[PW_Start_Address+PW_k*6+4+PW_m*8+PW_l];
    PW_l++;
  }
  /* are we beside the sample data address ? */
  if ( (PW_k*6+4+PW_m*8+PW_l) > PW_j )
  {
    return BAD;
  }
  if ( (PW_l == 0) || (PW_l == 128) )
  {
/*printf ( "#8.2 Start:%ld\n" , PW_Start_Address );*/
    return BAD;
  }
  PW_o += 1;
  /* PW_o is the highest number of pattern */


  /* test notes ... pfiew */
  PW_l += 1;
  for ( PW_n=(PW_k*6+4+PW_m*8+PW_l) ; PW_n<PW_j ; PW_n++ )
  {
    if ( (in_data[PW_Start_Address+PW_n]&0xff) == 0xff )
    {
      if ( (in_data[PW_Start_Address+PW_n+1]&0xc0) == 0x00 )
      {
	PW_n += 1;
	continue;
      }
      if ( (in_data[PW_Start_Address+PW_n+1]&0xc0) == 0x40 )
      {
	PW_n += 2;
	continue;
      }
      if ( (in_data[PW_Start_Address+PW_n+1]&0xc0) == 0xc0 )
      {
	if ( PW_n < ((in_data[PW_Start_Address+PW_n+2]*256)+in_data[PW_Start_Address+PW_n+3]-1) )
	  return BAD;
	PW_n += 3;
	continue;
      }
    }
    if ( (in_data[PW_Start_Address+PW_n]&0xff) == 0x7f )
    {
      continue;
    }
    
    /* no Fx nor FxArg */
    if ( (in_data[PW_Start_Address+PW_n]&0xf0) == 0xf0 )
    {
      if ( (in_data[PW_Start_Address+PW_n+1]&0x1F) > PW_k )
      {
/*printf ( "#9,1 Start:%ld (value:%ld) (where:%x) (PW_n:%ld) (PW_j:%ld)\n"
, PW_Start_Address
, in_data[PW_Start_Address+PW_n]
, PW_Start_Address+PW_n
, PW_n
, PW_j
 );*/
	return BAD;
      }
      PW_n += 2;
      continue;
    }
    if ( (in_data[PW_Start_Address+PW_n]&0xf0) == 0x70 )
    {
      if ( (in_data[PW_Start_Address+PW_n+1]&0x1F) > PW_k )
      {
/*printf ( "#9,2 Start:%ld (value:%ld) (where:%x) (PW_n:%ld) (PW_j:%ld)\n"
, PW_Start_Address
, in_data[PW_Start_Address+PW_n]
, PW_Start_Address+PW_n
, PW_n
, PW_j
 );*/
	return BAD;
      }
      PW_n += 1;
      continue;
    }
    /* no Note nor Sample number */
    if ( (in_data[PW_Start_Address+PW_n]&0xf0) == 0xe0 )
    {
      PW_n += 2;
      continue;
    }
    if ( (in_data[PW_Start_Address+PW_n]&0xf0) == 0x60 )
    {
      PW_n += 1;
      continue;
    }

    if ( (in_data[PW_Start_Address+PW_n]&0x80) == 0x80 )
    {
      if ( (((in_data[PW_Start_Address+PW_n]<<4)&0x10) | ((in_data[PW_Start_Address+PW_n+1]>>4)&0x0F)) > PW_k )
      {
/*printf ( "#9,3 Start:%ld (value:%ld) (where:%x) (PW_n:%ld) (PW_j:%ld)\n"
, PW_Start_Address
, in_data[PW_Start_Address+PW_n]
, PW_Start_Address+PW_n
, PW_n
, PW_j
 );*/
	return BAD;
      }
      PW_n += 3;
      continue;
    }

    if ( (((in_data[PW_Start_Address+PW_n]<<4)&0x10) | ((in_data[PW_Start_Address+PW_n+1]>>4)&0x0F)) > PW_k )
    {
/*printf ( "#9,4 Start:%ld (value:%ld) (where:%x) (PW_n:%ld) (PW_j:%ld)\n"
, PW_Start_Address
, in_data[PW_Start_Address+PW_n]
, PW_Start_Address+PW_n
, PW_n
, PW_j
 );*/
      return BAD;
    }
    PW_n += 2;
  }

  /* PW_WholeSampleSize is the whole sample data size */
  /* PW_j is the address of the sample data */
  return GOOD;
}


/******************/
/* packed samples */
/******************/
int16_t	 testP61A_pack ( void )
{
  if ( PW_i < 11 )
  {
    return BAD;
  }
  PW_Start_Address = PW_i-11;

  /* number of pattern (real) */
  PW_m = in_data[PW_Start_Address+2];
  if ( (PW_m > 0x7f) || (PW_m == 0) )
  {
/*printf ( "#1 Start:%ld\n" , PW_Start_Address );*/
    return BAD;
  }
  /* PW_m is the real number of pattern */

  /* number of sample */
  PW_k = in_data[PW_Start_Address+3];
  if ( (PW_k&0x40) != 0x40 )
  {
/*printf ( "#2,0 Start:%ld\n" , PW_Start_Address );*/
    return BAD;
  }
  PW_k &= 0x3F;
  if ( (PW_k > 0x1F) || (PW_k == 0) )
  {
/*printf ( "#2,1 Start:%ld (PW_k:%ld)\n" , PW_Start_Address,PW_k );*/
    return BAD;
  }
  /* PW_k is the number of sample */

  /* test volumes */
  if ( (PW_Start_Address+11+(PW_k*6))>PW_in_size)
    return BAD;
  for ( PW_l=0 ; PW_l<PW_k ; PW_l++ )
  {
    if ( in_data[PW_Start_Address+11+PW_l*6] > 0x40 )
    {
/*printf ( "#3 Start:%ld\n" , PW_Start_Address );*/
      return BAD;
    }
  }

  /* test fines */
  for ( PW_l=0 ; PW_l<PW_k ; PW_l++ )
  {
    if ( (in_data[PW_Start_Address+10+PW_l*6]&0x3F) > 0x0F )
    {
/*printf ( "#4 Start:%ld\n" , PW_Start_Address );*/
      return BAD;
    }
  }

  /* test sample sizes and loop start */
  PW_WholeSampleSize = 0;
  for ( PW_n=0 ; PW_n<PW_k ; PW_n++ )
  {
    PW_o = ( (in_data[PW_Start_Address+8+PW_n*6]*256) +
	     in_data[PW_Start_Address+9+PW_n*6] );
    if ( ((PW_o < 0xFFDF) && (PW_o > 0x8000)) || (PW_o == 0) )
    {
/*printf ( "#5 Start:%ld\n" , PW_Start_Address );*/
      return BAD;
    }
    if ( PW_o < 0xFF00 )
      PW_WholeSampleSize += (PW_o*2);

    PW_j = ( (in_data[PW_Start_Address+12+PW_n*6]*256) +
	     in_data[PW_Start_Address+13+PW_n*6] );
    if ( (PW_j != 0xFFFF) && (PW_j >= PW_o) )
    {
/*printf ( "#5,1 Start:%ld\n" , PW_Start_Address );*/
      return BAD;
    }
    if ( PW_o > 0xFFDF )
    {
      if ( (0xFFFF-PW_o) > PW_k )
      {
/*printf ( "#5,2 Start:%ld\n" , PW_Start_Address );*/
        return BAD;
      }
    }
  }

  /* test sample data address */
  PW_j = (in_data[PW_Start_Address]*256)+in_data[PW_Start_Address+1];
  if ( PW_j < (PW_k*6+8+PW_m*8) )
  {
/*printf ( "#6 Start:%ld\n" , PW_Start_Address );*/
    return BAD;
  }
  /* PW_j is the address of the sample data */


  /* test track table */
  for ( PW_l=0 ; PW_l<(PW_m*4) ; PW_l++ )
  {
    PW_o = ((in_data[PW_Start_Address+8+PW_k*6+PW_l*2]*256)+
            in_data[PW_Start_Address+8+PW_k*6+PW_l*2+1] );
    if ( (PW_o+PW_k*6+8+PW_m*8) > PW_j )
    {
/*printf ( "#7 Start:%ld (value:%ld)(where:%x)(PW_l:%ld)(PW_m:%ld)(PW_o:%ld)\n"
, PW_Start_Address
, (in_data[PW_Start_Address+PW_k*6+8+PW_l*2]*256)+in_data[PW_Start_Address+8+PW_k*6+PW_l*2+1]
, PW_Start_Address+PW_k*6+8+PW_l*2
, PW_l
, PW_m
, PW_o );*/
      return BAD;
    }
  }

  /* test pattern table */
  PW_l=0;
  PW_o=0;
  /* first, test if we dont oversize the input file */
  if ( (PW_k*6+8+PW_m*8) > PW_in_size )
  {
/*printf ( "8,0 Start:%ld\n" , PW_Start_Address );*/
    return BAD;
  }
  while ( (in_data[PW_Start_Address+PW_k*6+8+PW_m*8+PW_l] != 0xFF) && (PW_l<128) )
  {
    if ( in_data[PW_Start_Address+PW_k*6+8+PW_m*8+PW_l] > (PW_m-1) )
    {
/*printf ( "#8,1 Start:%ld (value:%ld)(where:%x)(PW_l:%ld)(PW_m:%ld)(PW_k:%ld)\n"
, PW_Start_Address
, in_data[PW_Start_Address+PW_k*6+8+PW_m*8+PW_l]
, PW_Start_Address+PW_k*6+8+PW_m*8+PW_l
, PW_l
, PW_m
, PW_k );*/
      return BAD;
    }
    if ( in_data[PW_Start_Address+PW_k*6+8+PW_m*8+PW_l] > PW_o )
      PW_o = in_data[PW_Start_Address+PW_k*6+8+PW_m*8+PW_l];
    PW_l++;
  }
  if ( (PW_l == 0) || (PW_l == 128) )
  {
/*printf ( "#8.2 Start:%ld\n" , PW_Start_Address );*/
    return BAD;
  }
  PW_o += 1;
  /* PW_o is the highest number of pattern */


  /* test notes ... pfiew */
  PW_l += 1;
  for ( PW_n=(PW_k*6+8+PW_m*8+PW_l) ; PW_n<PW_j ; PW_n++ )
  {
    if ( (in_data[PW_Start_Address+PW_n]&0xff) == 0xff )
    {
      if ( (in_data[PW_Start_Address+PW_n+1]&0xc0) == 0x00 )
      {
	PW_n += 1;
	continue;
      }
      if ( (in_data[PW_Start_Address+PW_n+1]&0xc0) == 0x40 )
      {
	PW_n += 2;
	continue;
      }
      if ( (in_data[PW_Start_Address+PW_n+1]&0xc0) == 0xc0 )
      {
	PW_n += 3;
	continue;
      }
    }
    if ( (in_data[PW_Start_Address+PW_n]&0xff) == 0x7f )
    {
      continue;
    }
    
    /* no Fx nor FxArg */
    if ( (in_data[PW_Start_Address+PW_n]&0xf0) == 0xf0 )
    {
      if ( (in_data[PW_Start_Address+PW_n+1]&0x1F) > PW_k )
      {
/*printf ( "#9,1 Start:%ld (value:%ld) (where:%x) (PW_n:%ld) (PW_j:%ld)\n"
, PW_Start_Address
, in_data[PW_Start_Address+PW_n]
, PW_Start_Address+PW_n
, PW_n
, PW_j
 );*/
	return BAD;
      }
      PW_n += 2;
      continue;
    }
    if ( (in_data[PW_Start_Address+PW_n]&0xf0) == 0x70 )
    {
      if ( (in_data[PW_Start_Address+PW_n+1]&0x1F) > PW_k )
      {
/*printf ( "#9,1 Start:%ld (value:%ld) (where:%x) (PW_n:%ld) (PW_j:%ld)\n"
, PW_Start_Address
, in_data[PW_Start_Address+PW_n]
, PW_Start_Address+PW_n
, PW_n
, PW_j
 );*/
	return BAD;
      }
      PW_n += 1;
      continue;
    }
    /* no Note nor Sample number */
    if ( (in_data[PW_Start_Address+PW_n]&0xf0) == 0xe0 )
    {
      PW_n += 2;
      continue;
    }
    if ( (in_data[PW_Start_Address+PW_n]&0xf0) == 0x60 )
    {
      PW_n += 1;
      continue;
    }

    if ( (in_data[PW_Start_Address+PW_n]&0x80) == 0x80 )
    {
      if ( (((in_data[PW_Start_Address+PW_n]<<4)&0x10) | ((in_data[PW_Start_Address+PW_n+1]>>4)&0x0F)) > PW_k )
      {
/*printf ( "#9,1 Start:%ld (value:%ld) (where:%x) (PW_n:%ld) (PW_j:%ld)\n"
, PW_Start_Address
, in_data[PW_Start_Address+PW_n]
, PW_Start_Address+PW_n
, PW_n
, PW_j
 );*/
	return BAD;
      }
      PW_n += 3;
      continue;
    }

    if ( (((in_data[PW_Start_Address+PW_n]<<4)&0x10) | ((in_data[PW_Start_Address+PW_n+1]>>4)&0x0F)) > PW_k )
    {
/*printf ( "#9,1 Start:%ld (value:%ld) (where:%x) (PW_n:%ld) (PW_j:%ld)\n"
, PW_Start_Address
, in_data[PW_Start_Address+PW_n]
, PW_Start_Address+PW_n
, PW_n
, PW_j
 );*/
      return BAD;
    }
    PW_n += 2;
  }


  /* PW_WholeSampleSize is the whole sample data size */
  /* PW_j is the address of the sample data */
  return GOOD;
}



void Rip_P61A ( void )
{
  /* PW_j is the number of sample */
  /* PW_WholeSampleSize is the whole sample data size */

  OutputSize = PW_j + PW_WholeSampleSize;

  CONVERT = GOOD;
  Save_Rip ( "The Player 6.1A module", P61A );
  
  if ( Save_Status == GOOD )
    PW_i += 12;  /* 7 should do but call it "just to be sure" :) */
}



/*
 *   The_Player_6.1a.c   1998 (c) Asle / ReDoX
 *
 * The Player 6.1a to Protracker.
 *
 * Note: As for version 5.0A and 6.0A, it's a REAL mess !.
 *      It's VERY badly coded, I know. Just dont forget it was mainly done
 *      to test the description I made of P61a format.
 *      I certainly wont dare to beat Gryzor on the ground :). His Prowiz IS
 *      the converter to use !!!. Though, using the official depacker could
 *      be a good idea too :).
 *
 * update:28/11/99
 *   - removed fopen() and all similar functions
 *   - Speed and Size (?) optimizings
 *
 * update:03/04/00
 *   - some code went away ????. reput it back :)
 *     pointed out by Thomas Neumann .. thx
 * update:26 nov 2003
 *   - used htonl() so that use of addy is now portable on 68k archs
*/

#define ON 1
#define OFF 2

void Depack_P61A ( void )
{
  uint8_t c1,c2,c3,c4,c5,c6;
  int32_t	 Max;
  uint8_t *Whatever;
  signed char *SmpDataWork;
  uint8_t PatPos = 0x00;
  uint8_t PatMax = 0x00;
  uint8_t Nbr_Sample = 0x00;
  uint8_t poss[37][2];
  uint8_t Track_Data[512][256];
  uint8_t SmpSizes[31][2];
  uint8_t PACK[31];
/*  uint8_t DELTA[31];*/
  uint8_t GLOBAL_DELTA=OFF;
  uint8_t GLOBAL_PACK=OFF;
  int32_t	 Track_Address[128][4];
  int32_t	 Track_Data_Address = 0;
  int32_t	 Sample_Data_Address = 0;
  int32_t	 WholeSampleSize = 0;
  int32_t	 i=0,j,k,l,a,b,z,w;
  int32_t	 SampleSizes[31];
  int32_t	 SampleAddresses[32];
  int32_t	 Unpacked_Sample_Data_Size;
  int32_t	 Where=PW_Start_Address;
  FILE *out;

  if ( Save_Status == BAD )
    return;

  BZERO ( Track_Address , 128*4*4 );
  BZERO ( Track_Data , 512*256 );
  BZERO ( SampleSizes , 31*4 );
  BZERO ( SampleAddresses , 32*4 );
  BZERO ( SmpSizes , 31*2 );
  for ( i=0 ; i<31 ; i++ )
  {
    PACK[i] = OFF;
/*    DELTA[i] = OFF;*/
  }

  fillPTKtable(poss);

  sprintf ( Depacked_OutName , "%d.mod" , Cpt_Filename-1 );
  out = PW_fopen ( Depacked_OutName , "w+b" );

  /* read sample data address */
  Sample_Data_Address = (in_data[Where]*256)+in_data[Where+1];
  Where+=2;

  /* read Real number of pattern */
  PatMax = in_data[Where++];

  /* read number of samples */
  Nbr_Sample = in_data[Where++];
  if ( (Nbr_Sample&0x80) == 0x80 )
  {
    /*printf ( "Samples are saved as delta values !\n" );*/
    GLOBAL_DELTA = ON;
  }
  if ( (Nbr_Sample&0x40) == 0x40 )
  {
    /*printf ( "some samples are packed !\n" );*/
    /*printf ( "\n! Since I could not understand the packing method of the\n"*/
    /*         "! samples, neither could I do a depacker .. . mission ends here :)\n" );*/
    GLOBAL_PACK = ON;
    return;
  }
  Nbr_Sample &= 0x3F;

  /* read unpacked sample data size */
  if ( GLOBAL_PACK == ON )
  {
    Unpacked_Sample_Data_Size = (in_data[Where]*256*256*256)+
                                (in_data[Where]*256*256)+
                                (in_data[Where]*256)+
                                 in_data[Where];
  }

  /* write title */
  Whatever = (uint8_t *) malloc ( 1024 );
  BZERO ( Whatever , 1024 );
  fwrite ( Whatever , 20 , 1 , out );

  /* sample headers stuff */
  for ( i=0 ; i<Nbr_Sample ; i++ )
  {
    /* write sample name */
    fwrite ( Whatever , 22 , 1 , out );

    /* sample size */
    SmpSizes[i][0] = in_data[Where];
    SmpSizes[i][1] = in_data[Where+1];
    j = (SmpSizes[i][0]*256)+SmpSizes[i][1];
    if ( j > 0xFF00 )
    {
      SampleSizes[i] = SampleSizes[0xFFFF-j];
      SmpSizes[i][0] = SmpSizes[0xFFFF-j][0];
      SmpSizes[i][1] = SmpSizes[0xFFFF-j][1];
/*fprintf ( debug , "!%2ld!" , 0xFFFF-j );*/
      SampleAddresses[i+1] = SampleAddresses[0xFFFF-j+1];/* - SampleSizes[i]+SampleSizes[0xFFFF-j];*/
    }
    else
    {
      SampleAddresses[i+1] = SampleAddresses[i]+SampleSizes[i-1];
      SampleSizes[i] = j*2;
      WholeSampleSize += SampleSizes[i];
    }
    j = SampleSizes[i]/2;
    fwrite ( &SmpSizes[i][0] , 1 , 1 , out );
    fwrite ( &SmpSizes[i][1] , 1 , 1 , out );

    /* finetune */
    c1 = in_data[Where+2];
    if ( (c1&0x40) == 0x40 )
      PACK[i]=ON;
    c1 &= 0x3F;
    fwrite ( &c1 , 1 , 1 , out );

    /* vol */
    fwrite ( &in_data[Where+3] , 1 , 1 , out );

    /* loop start */
/*fprintf ( debug , "loop start : %2x, %2x " , c1,c2 );*/
    if ( (in_data[Where+4]==0xFF) && (in_data[Where+5]==0xFF) )
    {
      Whatever[53]=0x01;
      fwrite ( &Whatever[50] , 4 , 1 , out );
/*fprintf ( debug , " <--- no loop! (%2x,%2x)\n" ,c3,c4);*/
      Where += 6;
      continue;
    }
    fwrite ( &in_data[Where+4] , 2 , 1 , out );
    l = j - ((in_data[Where+4]*256)+in_data[Where+5]);
/*fprintf ( debug , " -> size:%6ld  lstart:%5d  -> lsize:%ld\n" , j,c1*256+c2,l );*/

    /* use of htonl() suggested by Xigh !.*/
    w = htonl(l);
    c1 = *((uint8_t *)&w+2);
    c2 = *((uint8_t *)&w+3);
    fwrite ( &c1 , 1 , 1 , out );
    fwrite ( &c2 , 1 , 1 , out );

    Where += 6;
  }

  /* go up to 31 samples */
  Whatever[129] = 0x01;
  while ( i != 31 )
  {
    fwrite ( &Whatever[100] , 30 , 1 , out );
    i += 1;
  }
  /*printf ( "Whole sample size : %ld\n" , WholeSampleSize );*/

/*fprintf ( debug , "Where after sample headers : %x\n" , ftell ( in ) );*/

  /* read tracks addresses per pattern */
  for ( i=0 ; i<PatMax ; i++ )
  {
/*fprintf ( debug , "\npattern %ld : " , i );*/
    for ( j=0 ; j<4 ; j++ )
    {
      Track_Address[i][j] = (in_data[Where]*256)+in_data[Where+1];
      Where += 2;
/*fprintf ( debug , "%6ld, " , Track_Address[i][j] );*/
    }
  }
/*fprintf ( debug , "\n\nwhere after the track addresses : %x\n\n" , ftell ( in ));*/


  /* pattern table */
/*fprintf ( debug , "\nPattern table :\n" );*/
  BZERO ( Whatever , 1024 );
  for ( PatPos=0 ; PatPos<128 ; PatPos++ )
  {
    c1 = in_data[Where++];
    if ( c1 == 0xFF )
      break;
    Whatever[PatPos] = c1;
/*fprintf ( debug , "%2x, " , Whatever[PatPos] );*/
  }

  /* write size of pattern list */
  fwrite ( &PatPos , 1 , 1 , out );
/*fprintf ( debug , "\nsize of the pattern table : %d\n\n" , PatPos );*/

  /* write noisetracker byte */
  c1 = 0x7f;
  fwrite ( &c1 , 1 , 1 , out );

  /* write pattern table */
  fwrite ( Whatever , 128 , 1 , out );

  /* write ptk's ID */
  Whatever[0] = 'M';
  Whatever[1] = '.';
  Whatever[2] = 'K';
  Whatever[3] = '.';
  fwrite ( Whatever , 4 , 1 , out );

/*fprintf ( debug , "\n\nbefore reading track data : %x\n" , ftell ( in ) );*/
  Track_Data_Address = Where;

  /* rewrite the track data */

  /*printf ( "sorting and depacking tracks data ... " );*/
  for ( i=0 ; i<PatMax ; i++ )
  {
/*fprintf ( debug , "\n\npattern %ld\n" , i );*/
    Max = 63;
    for ( j=0 ; j<4 ; j++ )
    {
      Where = Track_Address[i][j]+Track_Data_Address;
/*fprintf ( debug , "track %ld (at:%ld)\n" , j,ftell ( in ) );*/
      for ( k=0 ; k<=Max ; k++ )
      {
        c1 = in_data[Where++];
/*fprintf ( debug , "%2ld: %2x, " , k , c1 );*/

	/* case no Fx nor FxArg  (3 bytes) */
	if ( ((c1&0x70) == 0x70) && (c1 != 0xFF) && (c1!=0x7F) )
	{
          c2 = in_data[Where++];
/*fprintf ( debug , "%2x, " , c2 );*/
          c6 = ((c1<<4)&0xf0)|((c2>>4)&0x0e);
          Track_Data[i*4+j][k*4]   = (c2&0x10) | (poss[c6/2][0]);
          Track_Data[i*4+j][k*4+1] = poss[c6/2][1];
          Track_Data[i*4+j][k*4+2] = ((c2<<4)&0xf0);

          if ( (c1 & 0x80) == 0x80 )
          {
            c3 = in_data[Where++];
            if ( c3 < 0x80 )
            {
/*fprintf ( debug , "%2x  <--- bypass %d rows !\n" , c3 , c3 );*/
              k += c3;
              continue;
            }
/*fprintf ( debug , "%2x  <--- repeat current row %d times\n" , c3 , c3-0x80 );*/
            c4=c3-0x80;
            for ( l=0 ; l<c4 ; l++ )
            {
              k += 1;
/*fprintf ( debug , "*%2ld: %2x, %2x\n" , k , c1,c2 );*/
              Track_Data[i*4+j][k*4]   = (c2&0x10) | (poss[c6/2][0]);
              Track_Data[i*4+j][k*4+1] = poss[c6/2][1];
              Track_Data[i*4+j][k*4+2] = ((c2<<4)&0xf0);
            }
          }
/*fprintf ( debug , "\n" );*/
          continue;
        }
	/* end of case no Fx nor FxArg */

	/* case no Sample number nor Relative not number */
	if ( ((c1&0x70) == 0x60) && (c1 != 0xFF) )
	{
          c2 = in_data[Where++];
/*fprintf ( debug , "%2x, " , c2 );*/

          c6 = c1&0x0f;
          if ( c6 == 0x08 )
            c1 -= 0x08;
          Track_Data[i*4+j][k*4+2] = (c1&0x0f);
          if ( (c6==0x05) || (c6==0x06) || (c6==0x0a) )
            c2 = (c2 > 0x7f) ? ((0x100-c2)<<4) : c2;
          Track_Data[i*4+j][k*4+3] = c2;

          if ( c6 == 0x0D )
          {
/*fprintf ( debug , " <-- PATTERN BREAK !, track ends\n" );*/
            Max = k;
            k = 9999l;
            continue;
          }
          if ( c6 == 0x0B )
          {
/*fprintf ( debug , " <-- PATTERN JUMP !, track ends\n" );*/
            Max = k;
            k = 9999l;
            continue;
          }
          if ( (c1 & 0x80) == 0x80 )
          {
            c3 = in_data[Where++];
            if ( c3 < 0x80 )
            {
/*fprintf ( debug , "%2x  <--- bypass %d rows !\n" , c3 , c3 );*/
              k += c3;
              continue;
            }
/*fprintf ( debug , "%2x  <--- repeat current row %d times\n" , c3 , c3-0x80 );*/
            c4=c3-0x80;
            for ( l=0 ; l<c4 ; l++ )
            {
              k += 1;
/*fprintf ( debug , "*%2ld: %2x, %2x\n" , k , c1,c2 );*/
              Track_Data[i*4+j][k*4+2] = (c1&0x0f);
              Track_Data[i*4+j][k*4+3] = c2;
            }
          }
/*fprintf ( debug , "\n" );*/
          continue;
        }
	/* end of case no Sample number nor Relative not number */

	if ( ((c1&0x80) == 0x80) && (c1!=0xFF) )
	{
          c2 = in_data[Where++];
          c3 = in_data[Where++];
          c4 = in_data[Where++];
/*fprintf ( debug , "%2x, %2x, " , c2,c3);*/
          c1 = c1&0x7F;
          Track_Data[i*4+j][k*4]   = ((c1<<4)&0x10) | (poss[c1/2][0]);
          Track_Data[i*4+j][k*4+1] = poss[c1/2][1];
          c6 = c2&0x0f;
          if ( c6 == 0x08 )
            c2 -= 0x08;
          Track_Data[i*4+j][k*4+2] = c2;
          if ( (c6==0x05) || (c6==0x06) || (c6==0x0a) )
            c3 = (c3 > 0x7f) ? ((0x100-c3)<<4) : c3;
          Track_Data[i*4+j][k*4+3] = c3;
          if ( c6 == 0x0D )
          {
/*fprintf ( debug , " <-- PATTERN BREAK !, track ends\n" );*/
            Max = k;
            k = 9999l;
            continue;
          }
          if ( c6 == 0x0B )
          {
/*fprintf ( debug , " <-- PATTERN JUMP !, track ends\n" );*/
            Max = k;
            k = 9999l;
            continue;
          }

          if ( c4 < 0x80 )
          {
/*fprintf ( debug , "%2x  <--- bypass %d rows !\n" , c4 , c4 );*/
            k += c4;
            continue;
          }
/*fprintf ( debug , "%2x  <--- repeat current row %d times\n" , c4 , c4-0x80 );*/
          c4=c4-0x80;
          for ( l=0 ; l<c4 ; l++ )
          {
            k += 1;
/*fprintf ( debug , "*%2ld: %2x, %2x, %2x\n" , k , c1,c2,c3 );*/
            Track_Data[i*4+j][k*4]   = ((c1<<4)&0x10) | (poss[c1/2][0]);
            Track_Data[i*4+j][k*4+1] = poss[c1/2][1];
            c6 = c2&0x0f;
            if ( c6 == 0x08 )
              c2 -= 0x08;
            Track_Data[i*4+j][k*4+2] = c2;
            if ( (c6==0x05) || (c6==0x06) || (c6==0x0a) )
              c3 = (c3 > 0x7f) ? ((0x100-c3)<<4) : c3;
            Track_Data[i*4+j][k*4+3] = c3;
          }
          continue;
        }


	if ( (c1&0x7F) == 0x7F )
	{
          if ( (c1&0x80) == 0x00 )
          {
/*fprintf ( debug , "  <--- bypass 1 row !\n" );*/
            /*k += 1;*/
            continue;
          }
          c2 = in_data[Where++];
          if ( c2 < 0x40 )
          {
/*fprintf ( debug , "%2x  <--- bypass %d rows !\n" , c2 , c2 );*/
            k += c2;
            continue;
          }
/*fprintf ( debug , "%2x, " , c2 );*/
          c2 -= 0x40;
          c3 = in_data[Where++];
/*fprintf ( debug , "%2x, " , c3 );*/
          z = c3;
          if ( c2 >= 0x80 )
          {
            c2 -= 0x80;
            c4 = in_data[Where++];
/*fprintf ( debug , "%2x, " , c4 );*/
            z = (c3*256)+c4;
          }
/*fprintf ( debug , " <--- repeat %2d lines some %ld bytes before\n" , c2,z );*/
          a = Where;
	  c5 = c2;
          Where -= z;
	  for ( l=0 ; (l<=c5)&&(k<=Max) ; l++,k++ )
	  {
            c1 = in_data[Where++];
/*fprintf ( debug , "#%2ld: %2x, " , l , c1 );*/

            /* case no Fx nor FxArg  (3 bytes) */
            if ( ((c1&0x70) == 0x70) && (c1 != 0xFF) && (c1!=0x7F))
            {
              c2 = in_data[Where++];
/*fprintf ( debug , "%2x, " , c2 );*/
              c6 = ((c1<<4)&0xf0)|((c2>>4)&0x0e);
              Track_Data[i*4+j][k*4]   = (c2&0x10) | (poss[c6/2][0]);
              Track_Data[i*4+j][k*4+1] = poss[c6/2][1];
              Track_Data[i*4+j][k*4+2] = ((c2<<4)&0xf0);

              if ( (c1 & 0x80) == 0x80 )
              {
                c3 = in_data[Where++];
                if ( c3 < 0x80 )
                {
/*fprintf ( debug , "%2x  <--- bypass %d rows !\n" , c3 , c3 );*/
                  k += c3;
                  continue;
                }
/*fprintf ( debug , "%2x  <--- repeat current row %d times\n" , c3 , c3-0x80 );*/
                c4=c3-0x80;
                for ( b=0 ; b<c4 ; b++ )
                {
                  k += 1;
/*fprintf ( debug , "*%2ld: %2x, %2x\n" , k , c1,c2 );*/
                  Track_Data[i*4+j][k*4]   = (c2&0x10) | (poss[c6/2][0]);
                  Track_Data[i*4+j][k*4+1] = poss[c6/2][1];
                  Track_Data[i*4+j][k*4+2] = ((c2<<4)&0xf0);
                }
              }
/*fprintf ( debug , "\n" );*/
              continue;
            }
            /* end of case no Fx nor FxArg */

            /* case no Sample number nor Relative not number */
            if ( ((c1&0x60) == 0x60) && (c1 != 0xFF) && (c1!=0x7F) )
            {
              c2 = in_data[Where++];
/*fprintf ( debug , "%2x, " , c2 );*/
              c6 = c1&0x0f;
              if ( c6 == 0x08 )
                c1 -= 0x08;
              Track_Data[i*4+j][k*4+2] = (c1&0x0f);
              if ( (c6==0x05) || (c6==0x06) || (c6==0x0a) )
                c2 = (c2 > 0x7f) ? ((0x100-c2)<<4) : c2;
              Track_Data[i*4+j][k*4+3] = c2;

              if ( c6 == 0x0D )
              {
/*fprintf ( debug , " <-- PATTERN BREAK !, track ends\n" );*/
                Max = k;
                k = 9999l;
                continue;
              }
              if ( c6 == 0x0B )
              {
/*fprintf ( debug , " <-- PATTERN JUMP !, track ends\n" );*/
                Max = k;
                k = 9999l;
                continue;
              }

              if ( (c1 & 0x80) == 0x80 )
              {
                c3 = in_data[Where++];
                if ( c3 < 0x80 )
                {
/*fprintf ( debug , "%2x  <--- bypass %d rows !\n" , c3 , c3 );*/
                  k += c3;
                  continue;
                }
/*fprintf ( debug , "%2x  <--- repeat current row %d times\n" , c3 , c3-0x80 );*/
                c4=c3-0x80;
                for ( b=0 ; b<c4 ; b++ )
                {
                  k += 1;
/*fprintf ( debug , "*%2ld: %2x, %2x\n" , k , c1,c2 );*/
                  Track_Data[i*4+j][k*4+2] = (c1&0x0f);
                  Track_Data[i*4+j][k*4+3] = c2;
                }
              }
/*fprintf ( debug , "\n" );*/
              continue;
            }
            /* end of case no Sample number nor Relative not number */

            if ( ((c1&0x80) == 0x80) && (c1!=0xFF) && (c1 != 0x7F))
	    {
              c2 = in_data[Where++];
              c3 = in_data[Where++];
              c4 = in_data[Where++];
/*fprintf ( debug , "%2x, %2x, " , c2,c3);*/
              c1 = c1&0x7f;
              Track_Data[i*4+j][k*4]   = ((c1<<4)&0x10) | (poss[c1/2][0]);
              Track_Data[i*4+j][k*4+1] = poss[c1/2][1];
              c6 = c2&0x0f;
              if ( c6 == 0x08 )
                c2 -= 0x08;
	      Track_Data[i*4+j][k*4+2] = c2;
              if ( (c6==0x05) || (c6==0x06) || (c6==0x0a) )
                c3 = (c3 > 0x7f) ? ((0x100-c3)<<4) : c3;
	      Track_Data[i*4+j][k*4+3] = c3;
              if ( c6 == 0x0D )
              {
                Max = k;
                k = l = 9999l;
/*fprintf ( debug , " <-- PATTERN BREAK !, track ends\n" );*/
                continue;
              }
              if ( c6 == 0x0B )
              {
                Max = k;
                k = l = 9999l;
/*fprintf ( debug , " <-- PATTERN JUMP !, track ends\n" );*/
                continue;
              }

              if ( c4 < 0x80 )
              {
/*fprintf ( debug , "%2x  <--- bypass %d rows !\n" , c4 , c4 );*/
                /*l += c4;*/
                k += c4;
                continue;
              }
/*fprintf ( debug , "%2x  <--- repeat current row %d times\n" , c4 , c4-0x80 );*/
              c4=c4-0x80;
              /*l += (c4-1);*/
              for ( b=0 ; b<c4 ; b++ )
              {
                k += 1;
/*fprintf ( debug , "*%2ld: %2x, %2x, %2x\n" , k , c1,c2,c3 );*/
                Track_Data[i*4+j][k*4]   = ((c1<<4)&0x10) | (poss[c1/2][0]);
                Track_Data[i*4+j][k*4+1] = poss[c1/2][1];
                c6 = c2&0x0f;
                if ( c6 == 0x08 )
                  c2 -= 0x08;
                Track_Data[i*4+j][k*4+2] = c2;
                if ( (c6==0x05) || (c6==0x06) || (c6==0x0a) )
                  c3 = (c3 > 0x7f) ? ((0x100-c3)<<4) : c3;
                Track_Data[i*4+j][k*4+3] = c3;
              }
              continue;
	    }
            if ( (c1&0x7F) == 0x7F )
            {
              if ( (c1&0x80) == 0x00 )
              {
/*fprintf ( debug , "  <--- bypass 1 row !\n" );*/
                /*k += 1;*/
                continue;
              }
              c2 = in_data[Where++];
              if ( c2 < 0x40 )
              {
/*fprintf ( debug , "%2x  <--- bypass %d rows !\n" , c2 , c2 );*/
                k += c2;
                continue;
              }
              continue;
            }

            c2 = in_data[Where++];
            c3 = in_data[Where++];
/*fprintf ( debug , "%2x, %2x" , c2,c3 );*/
	    Track_Data[i*4+j][k*4]   = ((c1<<4)&0x10) | (poss[c1/2][0]);
	    Track_Data[i*4+j][k*4+1] = poss[c1/2][1];
            c6 = c2&0x0f;
            if ( c6 == 0x08 )
              c2 -= 0x08;
	    Track_Data[i*4+j][k*4+2] = c2;
            if ( (c6==0x05) || (c6==0x06) || (c6==0x0a) )
              c3 = (c3 > 0x7f) ? ((0x100-c3)<<4) : c3;
	    Track_Data[i*4+j][k*4+3] = c3;
/*fprintf ( debug , "\n" );*/
	  }
          Where = a;
/*fprintf ( debug , "\n" );*/
          k -= 1;
	  continue;
	}

        c2 = in_data[Where++];
        c3 = in_data[Where++];
/*fprintf ( debug , "%2x, %2x" , c2,c3 );*/
	Track_Data[i*4+j][k*4]   = ((c1<<4)&0x10) | (poss[c1/2][0]);
	Track_Data[i*4+j][k*4+1] = poss[c1/2][1];
        c6 = c2&0x0f;
        if ( c6 == 0x08 )
          c2 -= 0x08;
	Track_Data[i*4+j][k*4+2] = c2;
        if ( (c6==0x05) || (c6==0x06) || (c6==0x0a) )
          c3 = (c3 > 0x7f) ? ((0x100-c3)<<4) : c3;
	Track_Data[i*4+j][k*4+3] = c3;
        if ( c6 == 0x0D )
        {
/*fprintf ( debug , " <-- PATTERN BREAK !, track ends\n" );*/
          Max = k;
          k = 9999l;
          continue;
        }
        if ( c6 == 0x0B )
        {
/*fprintf ( debug , " <-- PATTERN JUMP !, track ends\n" );*/
          Max = k;
          k = 9999l;
          continue;
        }

/*fprintf ( debug , "\n" );*/
      }
    }
  }
  /*printf ( "ok\n" );*/

  /* write pattern data */

  /*printf ( "writing pattern data ... " );*/
  for ( i=0 ; i<PatMax ; i++ )
  {
    BZERO ( Whatever , 1024 );
    for ( j=0 ; j<64 ; j++ )
    {
      for ( k=0 ; k<4 ; k++ )
      {
        Whatever[j*16+k*4]   = Track_Data[k+i*4][j*4];
        Whatever[j*16+k*4+1] = Track_Data[k+i*4][j*4+1];
        Whatever[j*16+k*4+2] = Track_Data[k+i*4][j*4+2];
        Whatever[j*16+k*4+3] = Track_Data[k+i*4][j*4+3];
      }
    }
    fwrite ( Whatever , 1024 , 1 , out );
  }
  free ( Whatever );
  /*printf ( "ok\n" );*/


  /* read and write sample data */

  /*printf ( "writing sample data ... " );*/
/*fprintf ( debug , "\n\nSample shit:\n" );*/
  for ( i=0 ; i<Nbr_Sample ; i++ )
  {
    Where = PW_Start_Address + Sample_Data_Address + SampleAddresses[i+1];
/*fprintf ( debug , "%2ld: read %-6ld at %ld\n" , i , SampleSizes[i] , ftell ( in ));*/
    SmpDataWork = (signed char *) malloc ( SampleSizes[i] );
    BZERO ( SmpDataWork , SampleSizes[i] );
    for ( j=0 ; j<SampleSizes[i] ; j++ ) SmpDataWork[j] = in_data[Where++];
    if ( GLOBAL_DELTA == ON )
    {
      c1=0x00;
      for ( j=1 ; j<SampleSizes[i] ; j++ )
      {
        c2 = SmpDataWork[j];
        c2 = 0x100-c2;
	c3 = c2 + c1;
        SmpDataWork[j] = c3;
        c1 = c3;
      }
    }
    fwrite ( SmpDataWork , SampleSizes[i] , 1 , out );
    free ( SmpDataWork );
  }
  /*printf ( "ok\n" );*/

  /* crap for GLOBAL_DELTA is taken cared of elsewhere */
  if ( GLOBAL_DELTA == ON )
    Crap ( " The Player 6.1A  " , GOOD , BAD , out );
  else
    Crap ( " The Player 6.1A  " , BAD , BAD , out );

  fclose ( out );

  printf ( "done\n" );
  return; /* useless ... but */
}
