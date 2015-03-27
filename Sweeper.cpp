#include <vcl\vcl.h>
#pragma hdrstop

USEFORM("Bomb.cpp", MainForm);
USERES("Sweeper.res");
USEFORM("Scores.cpp", ScoreForm);


WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
   try
   {
      Application->Initialize();
      Application->Title = "Bomb Squad";
      Application->HelpFile = "C:\\Allprog\\Games\\Sweeper\\Bomb Squad.hlp";
      Application->CreateForm(__classid(TMainForm) , &MainForm);
      Application->CreateForm(__classid(TScoreForm) , &ScoreForm);
      Application->Run();
   }
   catch (Exception &exception)
   {
      Application->ShowException(&exception);
   }

   return 0;
}

