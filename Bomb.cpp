/*
   Bomb Squad Source File By Steve Lawson © 1999 (Last Revised 11/09/1999 04:45pm)

   [HISTORY]
   -11/09/99 Came to fix a bug with mouse input on win95, decided to fix up code
             Fixed mouse input
             Changed code to match my new style

   -04/27/99 Took out some debug lines, final check

   -04/19/99 Made tick check faster, sync'ed timer, made numbers diff colors

   -04/18/99 Included a few missed features, fixed some other stuff

   -04/15/99 Finally fixed the high score file

   -04/13/99 Found and fixed a few huge bugs, added double down mouse event

   -04/03/99 Finished up code, fixed a few bugs, last checks

   -03/10/99 Bug fixes, Cleaned up code, added comments

   -03/02/99 Got it to work right, Added high score, bug fixes

   -02/05/99 Started file. Layed out basic input, graphics, and main recursive function


   [THINGS TO DO]

   -Add an options form to change colors, box size, and lives
   -Move difficulty settings to that options menu
   -Fix the drawing of icons(bombs,flags,eks) to draw correctly in squares larger than 15x15
   -Add that insanely cool,long,hard, yet faster blowup animation algo

*/

/*---------------------------------------------------------------------------------------------------------------------\
| I N C L U D E S /////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

#include <vcl\vcl.h>

#pragma hdrstop

#include "Bomb.h"
#include "Scores.h"
#include <fstream.h>

#pragma resource "*.dfm"
#pragma resource "Extra.RES"

/*---------------------------------------------------------------------------------------------------------------------\
| T M A I N F O R M  C O N S T R U C T E R ////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

TMainForm *MainForm;

__fastcall TMainForm::TMainForm(TComponent *Owner): TForm(Owner)
{
}

/*---------------------------------------------------------------------------------------------------------------------\
| S E T U P ///////////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::Setup(TObject *Sender)
{
   all = 0;
   diff = 1;
   level = 0;
   lives = 4;
   update = 9;
   gwidth = 15;
   gheight = 15;
   squares = 15;
   allcheck = 0;

   bgs = true;
   Gstate_gamestate = GAMEOFF;
   Gb_rightmouseon = false;
   Gb_leftmouseon = false;
   Done->Visible = false;
   Timer->Enabled = false;
   EnterName->Visible = false;

   warncolor = clRed;
   highcolor = clBlue;
   maincolor = clLtGray;
   darkmaincolor = clDkGray;
   lightcolor = clWhite;
   darkcolor = clDkGray;

   bg = new Graphics::TBitmap;
   temp = new Graphics::TBitmap;

   MainForm->Caption = "Bomb Squad!";
   Canvas->Font->Style << fsBold << fsStrikeOut;
   Readgame(Sender);
   GameOver(Sender);
}

/*---------------------------------------------------------------------------------------------------------------------\
| N E W  G A M E //////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::NewGame(TObject *Sender)
{                          //New Game, Reset Everything
   if(Timer->Tag != 1) GameOver(Sender);

   Done->Tag = -1;
   Timer->Tag = 0;

   Mpause->Enabled = false;

   switch(level)
   {
      case 0: gwidth=10; gheight=10; break;
      case 1: gwidth=15; gheight=15; break;
      case 2: gwidth=20; gheight=20; break;
      case 3: gwidth=25; gheight=25; break;
   }

   flags = bombcount = (gwidth * gheight) / (10 - diff);

   switch(diff)
   {
      case 1: counter = 5 * bombcount; break;
      case 2: counter = 4 * bombcount; break;
      case 3: counter = 2 * bombcount; break;
      case 4: counter = bombcount;   break;
   }

   holdf = flags;
   holdc = counter;

   //Resize Window Components
   ClientWidth = (gwidth * squares) + 11;
   ClientHeight =(gheight * squares) + 32;
   Time->Left = ClientWidth - Time->Width - 2;
   Go->Left = 3 + (ClientWidth / 2) - (Go->Width / 2);
   Panel1->Width = ClientWidth;
   Panel1->Height = ClientHeight - 21;
   Buffer->Width = Panel1->ClientWidth;
   Buffer->Height = Panel1->ClientHeight;
   bg->Width = Buffer->Width;
   bg->Height = Buffer->Height;

   //Fix 'Buffer' rendering area
   Buffer->Picture->Bitmap->Assign(bg);

   QuickNew(Sender);
}

/*---------------------------------------------------------------------------------------------------------------------\
| Q U I C K  N E W ////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::QuickNew(TObject *Sender)
{                          //New game same level, for faster restart
   if(lives <= 0)
   {
      GameOver(Sender);
      return;
   }

   DefocusControl(Go , false);

   srand(time(0));

   lives--;
   update = 8;
   counter = holdc;
   all = gwidth * gheight;
   flags = bombcount = holdf;
   allcheck = all;

   MainForm->Caption = AnsiString(lives) + " Trys Left";

   Time->Caption = AnsiString(holdc);
   Score->Caption = AnsiString(holdf);
   Time->Repaint();
   Score->Repaint();

   //Bg Image Loading, Handling
   bg->LoadFromResourceName( (int)HInstance , AnsiString((rand() % 8) + 1) + "b");
   Buffer->Canvas->StretchDraw( Rect(0 , 0 , Buffer->Width , Buffer->Height) , bg);
   bg->Width = Buffer->Width;
   bg->Height = Buffer->Height;
   bg->Canvas->Draw(0 , 0 , Buffer->Picture->Bitmap);

   //Painting Screen, Drawing 3D Boxes(Fast, Hopefully)
   Buffer->Canvas->Brush->Color = maincolor;
   Buffer->Canvas->FillRect(Rect(0 , 0 , Buffer->Width , Buffer->Height));

   for(int z = 0; z <= gwidth; z++)
   {
      Buffer->Canvas->Pen->Color = lightcolor;
      Buffer->Canvas->MoveTo((z * squares) + 1 , 0);
      Buffer->Canvas->LineTo((z * squares) + 1 , (gheight * squares) + 1);

      Buffer->Canvas->Pen->Color = darkcolor;
      Buffer->Canvas->MoveTo((z * squares) - 1 , 0);
      Buffer->Canvas->LineTo((z * squares) - 1 , (gheight * squares) + 1);
   }
   for(int z = 0; z <= gheight; z++)
   {
      Buffer->Canvas->Pen->Color = lightcolor;
      Buffer->Canvas->MoveTo(0 , (z * squares) + 1);
      Buffer->Canvas->LineTo((gwidth * squares) + 1 , (z * squares) + 1);

      Buffer->Canvas->Pen->Color = darkcolor;
      Buffer->Canvas->MoveTo(0 , ( z * squares) - 1);
      Buffer->Canvas->LineTo((gwidth * squares) + 1 , (z * squares) - 1);
   }
   for(int z=0; z<=gwidth; z++)
   {
      Buffer->Canvas->Pen->Color = clBlack;
      Buffer->Canvas->MoveTo( z * squares , 0);
      Buffer->Canvas->LineTo( z * squares , (gheight * squares) + 1);
   }
   for(int z = 0; z <= gheight; z++)
   {
      Buffer->Canvas->Pen->Color = clBlack;
      Buffer->Canvas->MoveTo(0 , z * squares);
      Buffer->Canvas->LineTo((gwidth * squares) + 1 , z * squares);
   }

   for(int z = 0; z < gwidth; z++)
   {
      for(int y = 0; y < gheight; y++)
      {
         grid[z][y] = 11;
      }
   }

   while(bombcount)
   {
      int q = (rand() % gwidth) , p = (rand() % gheight);

      if(grid[q][p] != 12)
      {
         grid[q][p] = 12;
         bombcount--;
      }
   }

   test = 0;
   Unp(Sender);
   Go->OnClick = QuickNew;
   Timer->OnTimer = Tick;
   Timer->Interval = 100;
   Timer->Enabled = false;
   Measy->Enabled = false;
   Mmed->Enabled = false;
   Mhard->Enabled = false;
   Msane->Enabled = false;
   Mback->Enabled = true;
   Mpause->Enabled = true;
   Gstate_gamestate = PLAYING;
}

/*---------------------------------------------------------------------------------------------------------------------\
| G A M E  O V E R ////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::GameOver(TObject *Sender)
{                          //Opening Pic...Reset Level values ect
   test = 0;
   level = 0;
   lives = 4;
   Gstate_gamestate = GAMEOFF;

   Panel1->Show();
   Buffer->Show();

   Timer->Enabled = false;
   Measy->Enabled = true;
   Mmed->Enabled = true;
   Mhard->Enabled = true;
   Msane->Enabled = true;

   Coords->Caption = "";
   Score->Caption = "GAME";
   Time->Caption = "OVER";

   bg->LoadFromResourceName( (int)HInstance , "Title" );
   Buffer->Canvas->StretchDraw( Rect(0 , 0 , Buffer->Width , Buffer->Height) , bg);

   Unp(Sender);
   Mpause->Enabled = false;
   OnClick = NULL;
   Go->OnClick = NewGame;
}

/*---------------------------------------------------------------------------------------------------------------------\
| M D O W N ///////////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::Mdown(TObject *Sender , TMouseButton Button , TShiftState Shift , int X , int Y)
{
   switch(Gstate_gamestate)
   {
      case BLOWUP:  QuickNew(Sender);
      case PAUSE:
      case GAMEOFF:
      return;
   }

   if(!Timer->Enabled)
   {
      Timer->Enabled = true;
   }

   curx = X / squares;
   cury = Y / squares;

   if(Button == mbLeft)
   {
      Gb_leftmouseon = true;
   }

   if(Button == mbRight)
   {
      Gb_rightmouseon = true;
   }

   if(Gb_leftmouseon && Gb_rightmouseon)
   {
      if(grid[curx][cury] > 9 || grid[curx][cury] == 0) return;

      test = 0;
      lastx = curx;
      lasty = cury;

      for(int a = -1; a <= 1; a++)
      {
         for(int b = -1; b <= 1; b++)
         {
            if(grid[curx + a][cury + b] > 8 && (a != 0 || b != 0))
            {
               UnCover((curx + a) * squares , (cury + b) * squares , 15);
            }

            if(grid[curx + a][cury + b] == 10 || grid[curx + a][cury + b] == 9)
            {
               test++;
            }
         }
      }

      for(int a = -1; a <= 1; a++)
      {
         for(int b = -1; b <= 1; b++)
         {
            if(test == grid[curx][cury] && (grid[curx + a][cury + b] == 11 || grid[curx + a][cury + b] == 12))
            {
               UnCover((curx + a) * squares , (cury + b) * squares , 16);
            }
         }
      }

      return;
   }

   if(grid[curx][cury] < 9)
   {
      return;
   }

   if(Button == mbRight)
   {
      if(grid[curx][cury] == 9)
      {
         grid[curx][cury] = 11;
         Cover(curx * squares , cury * squares);
         flags++;
         Score->Caption = AnsiString(flags);
         return;
      }
      if(grid[curx][cury] == 10)
      {
         grid[curx][cury] = 12;
         Cover(curx * squares , cury * squares);
         flags++;
         Score->Caption = AnsiString(flags);
         return;
      }
      if(grid[curx][cury] == 11)
      {
         if(flags == 0) return;
         grid[curx][cury] = 9;
         UnCover(curx * squares , cury * squares , 9);
         flags--;
         Score->Caption = AnsiString(flags);
         return;
      }
      if(grid[curx][cury] == 12)
      {
         if(flags == 0) return;
         grid[curx][cury] = 10;
         UnCover(curx * squares , cury * squares , 9);
         flags--;
         Score->Caption = AnsiString(flags);
         return;
      }
   }

   if(grid[curx][cury] >= 11)
   {
      lastx = X / squares;
      lasty = Y / squares;
      UnCover(curx * squares , cury * squares , 0);
   }
}

/*---------------------------------------------------------------------------------------------------------------------\
| M M O V E ///////////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::Mmove(TObject *Sender, TShiftState Shift , int X , int Y)
{                          // Mouse moved
   if(Gstate_gamestate == GAMEOFF || Gstate_gamestate == PAUSE)
   {
      return;
   }

   curx = X / squares;
   cury = Y / squares;

   Coords->Caption = AnsiString((curx + 1)) + " X " + AnsiString((cury + 1));

   if(Gb_leftmouseon && !Gb_rightmouseon)
   {
      if(curx != lastx || cury != lasty)
      {
         if(grid[lastx][lasty] >= 11)
         {
            Cover(lastx * squares , lasty * squares);
         }

         lastx = curx;
         lasty = cury;

         if(grid[curx][cury] >= 11)
         {
            UnCover(curx * squares , cury * squares , BLANK);
         }
      }
   }
}

/*---------------------------------------------------------------------------------------------------------------------\
| M U P ///////////////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::Mup(TObject *Sender , TMouseButton Button , TShiftState Shift , int X , int Y)
{                          //Mouse Button Up...Bomb?-Game Over   No Bomb?-Run Calc
   if(Gstate_gamestate == PAUSE) Unp(Sender);
   if(Gstate_gamestate == GAMEOFF || (!Gb_leftmouseon && !Gb_rightmouseon)) return;

   curx = X / squares;
   cury = Y / squares;

   if(Gb_leftmouseon && Gb_rightmouseon)
   {
      Gb_leftmouseon = false;
      Gb_rightmouseon = false;

      for(int a = -1; a <= 1; a++)
      {
         for(int b = -1; b <= 1; b++)
         {
            if(grid[lastx + a][lasty + b] <= 8) UnCover((lastx + a) * squares , (lasty + b) * squares , grid[lastx + a][lasty + b]);
            else if(grid[lastx + a][lasty + b] <= 10) UnCover((lastx + a) * squares,(lasty + b) * squares , 9);
            else if(lastx == curx && lasty == cury && test == grid[lastx][lasty])
            {
               if(grid[lastx + a][lasty + b] == 12)
               {
                  ShowBombs();
                  Timer->Enabled = false;
                  ShowMessage("            Boom!            ");
                  SetupBlow();
                  return;
               }
               Calc(lastx + a , lasty + b);
            }
            else Cover((lastx + a) * squares , (lasty + b) * squares);
         }
      }

      test = 0;
      return;
   }

   if(Button == mbRight)
   {
      Gb_rightmouseon = false;
      return;
   }

   Gb_leftmouseon = false;

   if(grid[curx][cury] == 12)
   {
      ShowBombs();
      UnCover(curx * squares , cury * squares , 14);
      Timer->Enabled = false;
      ShowMessage("            Boom!            ");
      SetupBlow();
   }
   else if(grid[curx][cury] == 11)
   {
      //ShowBombs();
      Calc(curx , cury);
   }
}

/*---------------------------------------------------------------------------------------------------------------------\
| C A L C /////////////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::Calc(int x, int y)
{               //Recursive Function, Test all 8 touching covered squares

   if(grid[x][y] != 11 || x < 0 || x >= gwidth || y < 0 || y >= gheight) return;

   int count = 0;
   all--;
   allcheck = (all - (holdf - flags));
   //Score->Caption="'"+AnsiString(allcheck)+"'";

   for(int a = -1; a <= 1; a++)
   {
      for(int b = -1; b <= 1; b++)
      {
         if(x + a >= 0 && x + a < gwidth && y + b >= 0 && y + b < gheight && (grid[x + a][y + b] == 12 || grid[ x + a ][ y + b ] == 10))
         {
            count++;
         }
      }
   }

   if(count == 0)
   {
      grid[x][y] = 0;
      UnCover(x * squares , y * squares , 0);

      Calc(x - 1 , y);
      Calc(x , y - 1);
      Calc(x + 1 , y);
      Calc(x , y + 1);
      Calc(x - 1 , y - 1);
      Calc(x - 1 , y + 1);
      Calc(x + 1 , y - 1);
      Calc(x + 1 , y + 1);
   }
   else
   {
      grid[x][y] = count;
      UnCover(x * squares , y * squares , count);
   }
}

/*---------------------------------------------------------------------------------------------------------------------\
| C O V E R ///////////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::Cover(int x, int y)
{                          //Draws Single 3D Square...Increment by 1, cuz of borders
   x++; y++;

   Buffer->Canvas->Brush->Color = maincolor;
   Buffer->Canvas->FillRect( Rect(x , y , x + squares - 1 , y + squares-1) );

   Buffer->Canvas->Pen->Color = darkcolor;
   Buffer->Canvas->MoveTo( x + squares - 2 , y + squares - 2);
   Buffer->Canvas->LineTo( x + squares - 2 , y - 1);
   Buffer->Canvas->MoveTo( x + squares - 2 , y + squares - 2);
   Buffer->Canvas->LineTo( x - 1 , y + squares - 2);

   Buffer->Canvas->Pen->Color = lightcolor;
   Buffer->Canvas->MoveTo(x , y);
   Buffer->Canvas->LineTo(x + squares - 1 , y);
   Buffer->Canvas->MoveTo(x , y);
   Buffer->Canvas->LineTo(x , y + squares - 2);

   Buffer->Canvas->Brush->Color = clBlack;
   Buffer->Canvas->FrameRect( Rect( x - 1 , y - 1 , x + squares , y + squares) );
}

/*---------------------------------------------------------------------------------------------------------------------\
| U N C O V E R ///////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::UnCover(int x , int y , int flag)
{                          //Draws uncovered square...using flag (see bomb.h)
int cx,cy;
   switch(flag)
   {
      case 0:  //Bg Square
         if(bgs) Buffer->Canvas->CopyRect( Rect(x , y , x + squares , y + squares) , bg->Canvas , Rect(x , y , x + squares , y + squares));
         else
         {
            Buffer->Canvas->Brush->Color = darkmaincolor;
            Buffer->Canvas->FillRect( Rect(x , y , x + squares, y + squares) );
            Buffer->Canvas->Brush->Color = clBlack;
            Buffer->Canvas->FrameRect( Rect( x , y , x + squares + 1 , y + squares + 1) );
         }

         cx = x / squares, cy = y / squares;

         if( cx - 1 > 0 && grid[cx - 1][cy] > 0 && grid[cx - 1][cy] < 9)
         {
           UnCover( (cx - 1) * squares , cy * squares , 17);
           //ShowMessage("arg! x -");
         }

         if(cy - 1 > 0 && grid[cx][cy - 1] > 0 && grid[cx][cy - 1] < 9)
         {
           UnCover( cx * squares , (cy - 1) * squares , 17);
           //ShowMessage("arg! y -");
         }
      break;

      case 9: //Flag
         Cover(x , y);
         Little->Draw(Buffer->Canvas , x + 1 , y + 1 , 1);
      break;

      case 12: //Bomb
         Buffer->Canvas->Brush->Color = maincolor;
         Buffer->Canvas->FillRect( Rect( x + 1 , y + 1 , x + squares , y + squares));
         Little->Draw(Buffer->Canvas , x + 1 , y + 1 , 0);
      break;

      case 13: //Eks
         Buffer->Canvas->Brush->Color = maincolor;
         Buffer->Canvas->FillRect( Rect( x + 1 , y + 1 , x + squares , y + squares));
         Little->Draw(Buffer->Canvas , x + 1 , y + 1 , 2);
      break;

      case 14: //Bomb(red bg)
         Buffer->Canvas->Brush->Color = clRed;
         Buffer->Canvas->FillRect( Rect( x + 1 , y + 1 , x + squares , y + squares));
         Little->Draw(Buffer->Canvas , x + 1 , y + 1 , 0);
      break;

      case 15: //blank highcolor(red)
         Buffer->Canvas->Brush->Color = highcolor;
         Buffer->Canvas->FillRect( Rect(x + 1 , y + 1 , x + squares , y + squares) );
      break;

      case 16: //blank warncolor(blue)
         Buffer->Canvas->Brush->Color = warncolor;
         Buffer->Canvas->FillRect( Rect( x + 1 , y + 1 , x + squares , y + squares) );
      break;

      case 17: //Black Frame
         Buffer->Canvas->Brush->Color = clBlack;
         Buffer->Canvas->FrameRect( Rect( x , y , x + squares + 1 , y + squares + 1) );
      break;

      default: //Numbers 1..8
         Buffer->Canvas->Brush->Color = clBlack;
         Buffer->Canvas->FrameRect( Rect( x , y , x + squares + 1 , y + squares + 1) );
         Buffer->Canvas->Brush->Color = darkmaincolor;
         Buffer->Canvas->FillRect( Rect(x + 1 , y + 1 , x + squares , y + squares) );
         Buffer->Canvas->Font->Size = 10;

         switch(flag)
         {
            case 1: Buffer->Canvas->Font->Color =  clBlack;  break;
            case 2: Buffer->Canvas->Font->Color =  clBlue;   break;
            case 3: Buffer->Canvas->Font->Color =  clRed;    break;
            case 4: Buffer->Canvas->Font->Color =  clGreen;  break;
            case 5: Buffer->Canvas->Font->Color =  clPurple; break;
            case 6: Buffer->Canvas->Font->Color =  clMaroon; break;
            case 7: Buffer->Canvas->Font->Color =  clNavy;   break;
            case 8: Buffer->Canvas->Font->Color =  clTeal;   break;
            default: Buffer->Canvas->Font->Color = clBlack;  break;
         }

         Buffer->Canvas->TextRect( Rect( x + 1 , y + 1 , x + squares , y + squares) , x + 5 , y , flag);
      break;
   }
}

/*---------------------------------------------------------------------------------------------------------------------\
| S E T U P  B L O W  /////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::SetupBlow()
{                          //so BlowUp() can be cancelable
   Mpause->Enabled = false;
   Mback->Enabled = false;
   Gstate_gamestate = BLOWUP;
   count=0 , delay = 100;

   temp->Width = bg->Width;
   temp->Height = bg->Height;
   temp->Canvas->Draw(0 , 0 , Buffer->Picture->Bitmap);
   Buffer->Canvas->Draw(0 , 0 , temp);

   for(int z = 0; z < gwidth; z++)
   {
      for(int y = 0; y < gheight; y++)
      {
         grid[z][y] = 100 + (1 * (rand() % 30));
      }
   }

   //Sets up Interval...so Focus is passes to main(other funcs can be called)
   //So that the explosion can be bypassed by hitting any key

   Timer->Interval = 1;
   Timer->OnTimer = BlowUp;
   Timer->Enabled = true;
}

/*---------------------------------------------------------------------------------------------------------------------\
| B L O W  U P ////////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::BlowUp(TObject *Sender)
{                          //Cool(Hopefully) Losing Animation(Slow Algo!!!)
   if(count > (gwidth * gheight))
   {
      if(lives <= 0) GameOver(Sender);
      else QuickNew(Sender);
      return;
   }

   Timer->Interval = 1;

   if(count == (gwidth * gheight)) count++;

   //Copy current squares ect... to bg buffer
   Buffer->Canvas->Draw(0 , 0 , temp);

   for(int z = 0; z < gwidth; z++)
   {
      for(int y = 0; y < gheight; y++)
      {
         if(grid[z][y] == delay) grid[z][y] = 0;

         Boom->Draw(Buffer->Canvas , (z * squares) - (30 - (squares / 2)) , (y * squares) - (30 - (squares / 2)) , grid[z][y]);

         switch(grid[z][y])
         {
            case 0: grid[z][y]++; break;
            case 1: grid[z][y]++; break;
            case 2: grid[z][y]=5; break;
            case 3: grid[z][y]=6; break;
            case 4: grid[z][y]--; break;
            case 5: grid[z][y]--; break;
         }

         if(grid[z][y] == 6)
         {
            count++;
            grid[z][y] = 7;
            temp->Canvas->CopyRect( Rect(z * squares , y * squares , (z * squares) + squares , (y * squares) + squares) , bg->Canvas , Rect( z * squares , y * squares , (z * squares) + squares , ( y * squares) + squares) );
         }
      }
   }
   delay++;
}

/*---------------------------------------------------------------------------------------------------------------------\
| T I C K /////////////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::Tick(TObject *Sender)
{                          //Exec every 1000 ms and test counter = 0 and if all bombs found
   int num = 0;
   //bool done;

   if(update <= 0)
   {
      update = 8;
      counter--;
      Time->Caption = AnsiString(counter);

      if(counter == 0)
      {
         UnCover(curx * squares , cury * squares , 14);
         ShowBombs();
         Timer->Enabled = false;
         ShowMessage("Times Up, Boom!                ");
         SetupBlow();
         return;
      }
   }
   else update--;

   if(flags == 0)
   {
      for(int z = 0; z < gwidth; z++)
      {
         for(int y = 0; y < gheight; y++)
         {
            if(grid[z][y] == 12)
            {
               num++;
            }
         }
      }

      if(num == 0)
      {
         //Important!!!! Disable timer!  To stop infinite looping
         Timer->Enabled = false;
         if(update >= 5) counter++;

         for(int z = 0; z < gwidth; z++)
         {
            for(int y = 0; y < gheight; y++)
            {
               Calc(z , y);
            }
         }

         Time->Caption = AnsiString(counter);
         TestHigh(counter);
      }
   }
   else if(allcheck == flags)
   {
      for(int z = 0; z < gwidth; z++)
      {
         for(int y = 0; y < gheight; y++)
         {
            if(grid[z][y] == 11)
            {
               //done = false;
               return;
            }
            if(grid[z][y] == 12)
            {
               num++;
               //done = true;
            }
         }
      }

      if(num == allcheck && num == flags)
      {
         for(int z = 0; z < gwidth; z++)
         {
            for(int y = 0; y < gheight; y++)
            {
               if(grid[z][y] == 12) UnCover(z * squares , y * squares , 9);
            }
         }

         //Important!!!! Disable timer!  To stop infinite looping

         Timer->Enabled = false;
         if(update >= 5) counter++;

         Score->Caption = 0;
         Time->Caption = AnsiString(counter);
         TestHigh(counter);
      }
   }
}

/*---------------------------------------------------------------------------------------------------------------------\
| S A V E  G A M E  ///////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::Savegame(TObject *Sender)
{                          //Encodes and saves the highscores
   ofstream output;

   output.open("Scores.bss");
   output << "[Bomb Squad High Score File]" << endl;
   output << "Do not edit this file, doing so will reset the scores!\n>" << endl;

   for(int a = 0; a < 16; a++)
   {
      int hold = hs[a].as_name.Length() + hs[a].i_score + hs[a].i_level;
      output << "!" << char( (hold - (90 * (hold / 90))) + 33 ) << char( (hold / 90) + 33 );
      output << ":" << hs[a].as_name << ":" << hs[a].i_level << ":" << hs[a].i_score << endl;
   }

   output.close();
}

/*---------------------------------------------------------------------------------------------------------------------\
| R E A D  G A M E ////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::Readgame(TObject *Sender)
{                          //Reads and decodes highscores
   int check = 0;
   ifstream input;
   char ok1 = '!' , ok2 = '!' , trash = '!';

   input.open("Scores.bss");

   do
   {
      input >> trash;
      check++;
   }
   while(trash != '>' && check < 250);

   if(check >= 250)
   {
      EraseHigh();
      input.close();
      input.clear();
      return;
   }

   for(int z = 0; z < 16; z++)
   {
      check = 0;
      hs[z].as_name = "";
      input >> trash;

      if(trash != '!')
      {
         EraseHigh();
         return;
      }

      input >> ok1;
      input >> ok2;
      input >> trash;
      input >> trash;

      do
      {
         hs[z].as_name += trash;
         input >> trash;
         check++;
      }
      while(trash != ':' && check < 200);

      if(check >= 200)
      {
         EraseHigh();
         input.close();
         input.clear();
         return;
      }

      input >> hs[z].i_level;
      input >> trash;
      input >> hs[z].i_score;

      if( (hs[z].as_name.Length() + hs[z].i_score + hs[z].i_level) != ( (ok1 - 33) + (90 * (ok2 - 33)) ) )
      {
         //ShowMessage("Doh:" + AnsiString(hs[z].score) + ":" + AnsiString(hs[z].level) + ":" + AnsiString(hs[z].name.Length()) + " Check:" + AnsiString( ( (ok1 - 33) + (90 * (ok2 - 33)) ) ) );
         EraseHigh();
         z = 100;  //escape forloop
      }
   }

   input.close();
   input.clear();
}

/*---------------------------------------------------------------------------------------------------------------------\
| E R A S E  H I G H //////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::EraseHigh()
{                          //Resets highscores
   for(int t = 0; t < 16; t++)
   {
      hs[t].as_name = "NoScorePosted";
      hs[t].i_level = t;
      hs[t].i_score = 0;
   }

   Savegame(this);
}

/*---------------------------------------------------------------------------------------------------------------------\
| T E S T  H I G H ////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::TestHigh(int time)
{                          //Checks to see if the var 'time' beats out the current time
   int check = ((diff - 1) * 4) + level;

   if(hs[check].i_score > holdc - time || hs[check].i_score == 0)
   {
      Gstate_gamestate = GAMEOFF;
      Go->Enabled = false;
      Timer->Enabled = false;
      Mback->Enabled = false;
      Mpause->Enabled = false;
      temp->Width = Buffer->Width;
      temp->Height = Buffer->Height;
      temp->Canvas->Draw(0 , 0 , Buffer->Picture->Bitmap);
      EnterName->Visible = true;
      EnterName->Left = 5 + (Buffer->Width / 2) - (EnterName->Width / 2);
      EnterName->Top = 5 + (Buffer->Height / 2) - (EnterName->Height / 2);

      Buffer->Canvas->Brush->Color = clBlack;
      Buffer->Canvas->FillRect(Rect(0 , 0 , Buffer->Width , Buffer->Height));
      Buffer->Canvas->Font->Size = 8;
      Buffer->Canvas->Font->Color = clGray;
      Buffer->Canvas->FillRect( Rect( EnterName->Left - 7 , EnterName->Top - 20 , EnterName->Left + EnterName->Width - 3 , EnterName->Top + EnterName->Height - 2 ) );
      Buffer->Canvas->TextRect( Rect( EnterName->Left - 7 , EnterName->Top - 20 , EnterName->Left + EnterName->Width - 3 , EnterName->Top + EnterName->Height - 2 ) , EnterName->Left - 5 , EnterName->Top - 20 , "!!NEW HIGH SCORE!!" );

      ActiveControl = EnterName;
      EnterName->SelStart = 0;
      EnterName->SelLength = 15;
      Coords->Caption = "";
      hs[check].i_score = holdc - time;
      Done->Tag = check;
   }
   else DoneHigh(this);
}

/*---------------------------------------------------------------------------------------------------------------------\
| D O N E  H I G H ////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::DoneHigh(TObject *Sender)
{                          // Writes highscore and gives a winning msg
   AnsiString doh;

   if(Done->Tag >= 0)
   {
      Buffer->Canvas->Draw(0 , 0 , temp);
      Gstate_gamestate = PLAYING;
      Go->Enabled = true;
      Mback->Enabled = true;
      Mpause->Enabled = true;
      EnterName->Visible = false;
      Done->Visible = false;
      hs[Done->Tag].as_name = EnterName->Text;

      for(int q = 0; q < hs[Done->Tag].as_name.Length(); q++)
      {
         if(hs[Done->Tag].as_name[q] == ' ') hs[Done->Tag].as_name.Delete(q , 1);
      }

      Savegame(Sender);
      doh = "\n\n!!New High Score!!\n" + hs[Done->Tag].as_name + " With A Time Of " + AnsiString(hs[Done->Tag].i_score);
   }

   if(level == 3 && diff == 4) ShowMessage("You Are The GrandMaster...\nYou have totally beat my game down!\nPlease refer to the help section 'Master'" + doh);
   else if(level == 3) ShowMessage("You Are The Man!  Try the next difficulty level!" + doh);
   else ShowMessage("You Win!! On To The Next Level..." + doh);

   level++;
   lives++;

   Timer->Tag = 1;
   Done->Tag = -1;

   if(level == 4) GameOver(Sender);
   else NewGame(Sender);
}

/*---------------------------------------------------------------------------------------------------------------------\
| R E P ///////////////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::Rep(TObject *Sender)
{                          // Pauses if not already paused
   if(Gstate_gamestate == GAMEOFF || Gstate_gamestate == PAUSE) return;

   int m1 , m2;

   Gstate_gamestate = PAUSE;
   Go->Enabled = false;
   Timer->Enabled = false;
   Mback->Enabled = false;
   Mpause->OnClick = Unp;
   Mpause->Caption = "&Unpause";
   Coords->Caption = "PAUSED";
   temp->Width = Buffer->Width;
   temp->Height = Buffer->Height;
   temp->Canvas->Draw(0 , 0 , Buffer->Picture->Bitmap);
   Buffer->Canvas->Draw(0 , 0 , bg);
   Buffer->Canvas->Font->Color = clGray;
   Buffer->Canvas->Brush->Color = clBlack;
   Buffer->Canvas->Font->Size = 12;

   //Buffer->Canvas->FillRect(Rect(0 , 0 , Buffer->Width , Buffer->Height));

   m1 = Buffer->Canvas->TextWidth("GAME IS PAUSED");
   m2 = Buffer->Canvas->TextWidth("CLICK TO RESUME");
   Buffer->Canvas->TextOut(((Buffer->Width / 2) - (m1 / 2)) , (Buffer->Height / 2) - 30 , "GAME IS PAUSED");
   Buffer->Canvas->TextOut(((Buffer->Width / 2) - (m2 / 2)) , (Buffer->Height / 2) - 10 , "CLICK TO RESUME");
}

/*---------------------------------------------------------------------------------------------------------------------\
| U N P  //////////////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::Unp(TObject *Sender)
{                          //Unpauses if paused, ignores if not
   if(Gstate_gamestate == GAMEOFF || Gstate_gamestate != PAUSE) return;

   Gstate_gamestate = PLAYING;
   Go->Enabled = true;
   Timer->Enabled = true;
   Mback->Enabled = true;
   Mpause->OnClick = Rep;
   Mpause->Caption = "&Pause";
   Buffer->Canvas->Draw(0 , 0 , temp);
}

/*---------------------------------------------------------------------------------------------------------------------\
| E R A S E ///////////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::Erase(TObject *Sender)
{                          // Asks user if they are sure they want to clear the high scores
   switch( MessageBox(Handle , "Ok To Reset The High Scores?" , "Reset The High Scores?" , (MB_YESNO + MB_ICONQUESTION)) )
   {
      case IDYES: break;
      case IDNO:  return;
   }

   EraseHigh();
}

/*---------------------------------------------------------------------------------------------------------------------\
| S E /////////////////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::se(TObject *Sender)
{                          // Set Easy Func
   diff = 1;
   Measy->Checked = true;
   Mmed->Checked = false;
   Mhard->Checked = false;
   Msane->Checked = false;
   ShowMessage("Difficulty set to easy!");
}

/*---------------------------------------------------------------------------------------------------------------------\
| S M /////////////////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::sm(TObject *Sender)
{                          // Set Med Func
   diff = 2;
   Measy->Checked = false;
   Mmed->Checked = true;
   Mhard->Checked = false;
   Msane->Checked = false;
   ShowMessage("Difficulty set to medium!");
}

/*---------------------------------------------------------------------------------------------------------------------\
| S H /////////////////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::sh(TObject *Sender)
{                          // Set Hard
   diff = 3;
   Measy->Checked = false;
   Mmed->Checked = false;
   Mhard->Checked = true;
   Msane->Checked = false;
   ShowMessage("Difficulty set to hard!");
}

/*---------------------------------------------------------------------------------------------------------------------\
| S I /////////////////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::si(TObject *Sender)
{                          // Set Insane
   diff = 4;
   Measy->Checked = false;
   Mmed->Checked = false;
   Mhard->Checked = false;
   Msane->Checked = true;
   ShowMessage("Difficulty set to insane!");
}

/*---------------------------------------------------------------------------------------------------------------------\
| S B G  //////////////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::sbg(TObject *Sender)
{                          // Bg picture switch
   if(Mback->Checked)
   {
      Mback->Checked = false;
      bgs = false;
   }
   else
   {
      Mback->Checked = true;
      bgs = true;
   }

   if(Gstate_gamestate == GAMEOFF || Gstate_gamestate == PAUSE) return;

   for(int z = 0; z < gwidth; z++)
   {
      for(int y = 0; y < gheight; y++)
      {
         if(grid[z][y] == 0) UnCover(z * squares , y * squares , 0);
      }
   }
}

/*---------------------------------------------------------------------------------------------------------------------\
| S H O W  B O M B S //////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::ShowBombs()
{                          //Show All Bombs...When Player Lost
   for(int z = 0; z < gwidth; z++)
   {
      for(int y = 0; y < gheight; y++)
      {
         if(grid[z][y] == 9) UnCover(z * squares , y * squares , 13);
         if(grid[z][y] == 12) UnCover(z * squares , y * squares , 12);
      }
   }
}

/*---------------------------------------------------------------------------------------------------------------------\
| H I T  E N T E R ////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::HitEnter(TObject *Sender, WORD &Key, TShiftState Shift)
{                          // User only has to hit enter when entering high score name
   if(Key == 10 || Key == 13) DoneHigh(Sender);
}

/*---------------------------------------------------------------------------------------------------------------------\
| S A V E /////////////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::Save(TObject *Sender, TCloseAction &Action)
{                          // Saves the game when the user leaves
   Savegame(Sender);
}

/*---------------------------------------------------------------------------------------------------------------------\
| S H O W  H I G H ////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::ShowHigh(TObject *Sender)
{                          // Shows highscores
   ScoreForm->ShowModal();
}

/*---------------------------------------------------------------------------------------------------------------------\
| Q U I T  ////////////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::Quit(TObject *Sender)
{                          // Close duh :)
   Close();
}

/*---------------------------------------------------------------------------------------------------------------------\
| S H O W  H E L P ////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::ShowHelp(TObject *Sender)
{                          // Shows the help file
   Application->HelpCommand(HELP_CONTENTS , 0);
}

/*---------------------------------------------------------------------------------------------------------------------\
| C L E A N  U P //////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::Cleanup(TObject *Sender)
{                          // Clean Up My Allocated Memory...Prevent Memory Leakage
   delete bg;
   delete temp;
}

/*---------------------------------------------------------------------------------------------------------------------\
| H I T  K E Y ////////////////////////////////////////////////////////////////////////////////////////////////////////|
\---------------------------------------------------------------------------------------------------------------------*/

void __fastcall TMainForm::Hitkey(TObject *Sender , WORD &Key , TShiftState Shift)
{                         // Key press handler
   if(!Mpause->Enabled) return;

   if(Key == 80 || Key == 19)
   {
      if(Gstate_gamestate == PAUSE) Unp(this);
      else Rep(this);
   }
}

