/* DEBUG : Language specific headers go here */

/* DEBUG : Pointer conversion function here */

/* DEBUG : Language specific code here */

#define   SWIG_init     swig_init

#define   SWIG_name    "swig"
WRAPPER : double bar(int );

WRAPPER : int grok(double );

WRAPPER : double frob2(double );

WRAPPER : int grok2(int );

WRAPPER : int  a; 
WRAPPER : double  d; 
SWIG POINTER-MAPPING TABLE

/*
 * This table is used by the pointer type-checker
 */
static struct { char *n1; char *n2; void *(*pcnv)(void *); } _swig_mapping[] = {
    { "_signed_long","_long",0},
    { "_long","_unsigned_long",0},
    { "_long","_signed_long",0},
    { "_unsigned_long","_long",0},
    { "_signed_int","_int",0},
    { "_unsigned_short","_short",0},
    { "_signed_short","_short",0},
    { "_unsigned_int","_int",0},
    { "_short","_unsigned_short",0},
    { "_short","_signed_short",0},
    { "_int","_unsigned_int",0},
    { "_int","_signed_int",0},
{0,0,0}};


/* MODULE INITIALIZATION */

void swig_init() {
     ADD COMMAND    : bar --> double bar(int );
     ADD COMMAND    : grok --> int grok(double );
     ADD COMMAND    : frob2 --> double frob2(double );
     ADD COMMAND    : grok2 --> int grok2(int );
     ADD VARIABLE   : a --> int  a; 
     ADD VARIABLE   : d --> double  d; 
}  /* END INIT */
{
   int i;
   for (i = 0; _swig_mapping[i].n1; i++)
        SWIG_RegisterMapping(_swig_mapping[i].n1,_swig_mapping[i].n2,_swig_mapping[i].pcnv);
}
