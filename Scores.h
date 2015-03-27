#ifndef ScoresH
#define ScoresH

#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>

class TScoreForm: public TForm
{
   __published:

    TButton *Bexit;
    TLabel *Llevel;
    TLabel *Lscore;
    TLabel *Lname;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TLabel *Label4;
    TLabel *Label5;
    TLabel *Label6;
    TLabel *Label7;
    TLabel *Label8;
    TLabel *Label9;
    TLabel *Label10;
    TLabel *Label11;
    TLabel *Label12;
    TLabel *Label13;
    TLabel *Label14;
    TLabel *Label15;
    TLabel *Label16;
    TLabel *Label17;
    TLabel *Label18;
    TLabel *Label19;
    TLabel *Label20;

    TLabel *n1;
    TLabel *s1;
    TLabel *n2;
    TLabel *s2;
    TLabel *n3;
    TLabel *s3;
    TLabel *n4;
    TLabel *s4;
    TLabel *n5;
    TLabel *s5;
    TLabel *n6;
    TLabel *s6;
    TLabel *n7;
    TLabel *s7;
    TLabel *n8;
    TLabel *s8;
    TLabel *n9;
    TLabel *s9;
    TLabel *n10;
    TLabel *s10;
    TLabel *n11;
    TLabel *s11;
    TLabel *n12;
    TLabel *s12;
    TLabel *n13;
    TLabel *s13;
    TLabel *n14;
    TLabel *s14;
    TLabel *n15;
    TLabel *s15;
    TLabel *n16;
    TLabel *s16;

    void __fastcall Read(TObject *Sender);

   private:

   public:

    __fastcall TScoreForm(TComponent *Owner);
};

extern TScoreForm *ScoreForm;

#endif 