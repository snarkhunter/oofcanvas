/* DEBUG : Language specific headers go here */

/* DEBUG : Pointer conversion function here */

/* DEBUG : Language specific code here */

#define   SWIG_init     swig_init

#define   SWIG_name    "swig"
WRAPPER : int foo(int ,double ,char *);

WRAPPER : int bar(double ,char *,char ,void *);

C++ CLASS DECLARATION : class Foo
WRAPPER : void grok(Foo *);

WRAPPER : void ref(String &);

C++ CLASS START : class Foo  ========================================

        C++ CONST     : (int ) LAGER = [None]
        C++ CONST     : (int ) ALE = [None]
        C++ CONST     : (int ) STOUT = [None]
        MEMBER FUNC   : void test(double ,Foo::SWIG );

        MEMBER FUNC   : void test2(double ,Foo::SWIG );

C++ CLASS END ===================================================

SWIG POINTER-MAPPING TABLE

/*
 * This table is used by the pointer type-checker
 */
static struct { char *n1; char *n2; void *(*pcnv)(void *); } _swig_mapping[] = {
    { "_signed_long","_long",0},
    { "_class_Foo","_Foo",0},
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
    { "_Foo","_class_Foo",0},
{0,0,0}};


/* MODULE INITIALIZATION */

void swig_init() {
     ADD COMMAND    : foo --> int foo(int ,double ,char *);
     ADD COMMAND    : bar --> int bar(double ,char *,char ,void *);
     ADD COMMAND    : grok --> void grok(Foo *);
     ADD COMMAND    : ref --> void ref(String &);

     // C++ CLASS START : class Foo
     ADD C++ CONST  : LAGER --> (int ) = [None]
     ADD C++ CONST  : ALE --> (int ) = [None]
     ADD C++ CONST  : STOUT --> (int ) = [None]
     ADD MEMBER FUN : test --> void test(double ,Foo::SWIG );
     ADD MEMBER FUN : test2 --> void test2(double ,Foo::SWIG );
     // C++ CLASS END 

}  /* END INIT */
{
   int i;
   for (i = 0; _swig_mapping[i].n1; i++)
        SWIG_RegisterMapping(_swig_mapping[i].n1,_swig_mapping[i].n2,_swig_mapping[i].pcnv);
}
