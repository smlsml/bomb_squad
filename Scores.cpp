#include <vcl\vcl.h>
#pragma hdrstop

#include "Bomb.h"
#include "Scores.h"

#pragma resource "*.dfm"

TScoreForm *ScoreForm;

__fastcall TScoreForm::TScoreForm(TComponent *Owner1): TForm(Owner1){}

void __fastcall TScoreForm::Read(TObject *Sender)
{
   n1->Caption=MainForm->hs[0].as_name;
   s1->Caption=MainForm->hs[0].i_score;

   n2->Caption=MainForm->hs[1].as_name;
   s2->Caption=MainForm->hs[1].i_score;

   n3->Caption=MainForm->hs[2].as_name;
   s3->Caption=MainForm->hs[2].i_score;

   n4->Caption=MainForm->hs[3].as_name;
   s4->Caption=MainForm->hs[3].i_score;

   n5->Caption=MainForm->hs[4].as_name;
   s5->Caption=MainForm->hs[4].i_score;

   n6->Caption=MainForm->hs[5].as_name;
   s6->Caption=MainForm->hs[5].i_score;

   n7->Caption=MainForm->hs[6].as_name;
   s7->Caption=MainForm->hs[6].i_score;

   n8->Caption=MainForm->hs[7].as_name;
   s8->Caption=MainForm->hs[7].i_score;

   n9->Caption=MainForm->hs[8].as_name;
   s9->Caption=MainForm->hs[8].i_score;

   n10->Caption=MainForm->hs[9].as_name;
   s10->Caption=MainForm->hs[9].i_score;

   n11->Caption=MainForm->hs[10].as_name;
   s11->Caption=MainForm->hs[10].i_score;

   n12->Caption=MainForm->hs[11].as_name;
   s12->Caption=MainForm->hs[11].i_score;

   n13->Caption=MainForm->hs[12].as_name;
   s13->Caption=MainForm->hs[12].i_score;

   n14->Caption=MainForm->hs[13].as_name;
   s14->Caption=MainForm->hs[13].i_score;

   n15->Caption=MainForm->hs[14].as_name;
   s15->Caption=MainForm->hs[14].i_score;

   n16->Caption=MainForm->hs[15].as_name;
   s16->Caption=MainForm->hs[15].i_score;
}

