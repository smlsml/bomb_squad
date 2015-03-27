/* Bomb Squad Header File By Steve Lawson © 1999 (Last Revised 04/19/99 12:51pm)

   HISTORY (Im a bum)
   -04/27/99 - Checking stuff (release)
   -04/19/99 - Added a few vars, ready for release
   -04/15/99 - Finished final checks
   -04/03/99 - Finished code.  Last checks.
   -03/10/99 - Cleaned up code, added comments
   -03/02/99 - Added high score vars
   -02/05/99 - Started file

   VAR DESCRIPTIONS (In case I forget, Obvious vars not included here)
   -bool bgs    - toggle use of backgroud picture
   -int squares - dimensions of squares normally 15x15 (Included because I felt the need)
   -int holdf   - hold flags, for quick new games without total restart
   -int holdc   - hold counter, for quick new games without total restart

   FLAG MEANINGS (Why didnt I use a enum? Cuz I like to be confused!)
   -[0]  blank
   -[1]  1-8 numbers
   -[9]  flag(no bomb)
   -[10] flag(bomb)
   -[11] no bomb
   -[12] bomb
   -[13] Show X
   -[14] Show Red Bomb

   THINGS TO ADD (Call me a loser or a bum, cfi care)
   -Add options form...to customize colors, sizes, pictures, ect
   -Fix the drawing of icons(bombs,flags,eks) to draw correctly in squares larger than 15x15
   -Add that insanely cool,long,hard, yet faster blowup animation algo
*/

#ifndef BombH
#define BombH

#include <vcl\Mask.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Menus.hpp>
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\ExtCtrls.hpp>

struct BST_highscores
{
   int i_level;
   int i_score;
   AnsiString as_name;

   BST_highscores() : i_level(0),i_score(0),as_name("NoScorePosted"){}
};

enum BST_state{GAMEOFF , PAUSE , PLAYING , BLOWUP};
enum BST_flags{BLANK , ONE , TWO , THREE , FOUR , FIVE , SIX , SEVEN , EIGHT , NINE , FLAG_NO_BOMB , FLAG_BOMB , NO_BOMB , BOMB , SHOW_X , SHOW_RED_BOMB};

class TMainForm: public TForm
{
   __published:

    TButton *Go;
    TButton *Done;           //Enter name done button

    TLabel *Time;
    TLabel *Score;

    TTimer *Timer;           //System timer

    TPanel *Panel1;

    TImage *Buffer;          //Double buffer

    TImageList *Boom;        //Fireballs
    TImageList *Little;      //Icons, eks, bombs

    TMainMenu *Menu1;
    TMenuItem *N1;
    TMenuItem *N2;
    TMenuItem *N3;
    TMenuItem *N4;
    TMenuItem *Mnew;
    TMenuItem *Mmed;
    TMenuItem *Mfile;
    TMenuItem *Mback;
    TMenuItem *Mexit;
    TMenuItem *Measy;
    TMenuItem *Mhard;
    TMenuItem *Msane;
    TMenuItem *Mquit;
    TMenuItem *Mhelp;
    TMenuItem *Coords;
    TMenuItem *Mpause;
    TMenuItem *Moptions;
    TMenuItem *ResetHigh1;
    TMenuItem *HighScores1;

    TMaskEdit *EnterName;    //High score name box

    void __fastcall se(TObject *Sender);
    void __fastcall sm(TObject *Sender);
    void __fastcall sh(TObject *Sender);
    void __fastcall si(TObject *Sender);
    void __fastcall Rep(TObject *Sender);
    void __fastcall Unp(TObject *Sender);
    void __fastcall sbg(TObject *Sender);
    void __fastcall Tick(TObject *Sender);
    void __fastcall Quit(TObject *Sender);
    void __fastcall Setup(TObject *Sender);
    void __fastcall Erase(TObject *Sender);
    void __fastcall BlowUp(TObject *Sender);
    void __fastcall NewGame(TObject *Sender);
    void __fastcall Cleanup(TObject *Sender);
    void __fastcall GameOver(TObject *Sender);
    void __fastcall ShowHelp(TObject *Sender);
    void __fastcall DoneHigh(TObject *Sender);
    void __fastcall QuickNew(TObject *Sender);
    void __fastcall Savegame(TObject *Sender);
    void __fastcall Readgame(TObject *Sender);
    void __fastcall ShowHigh(TObject *Sender);
    void __fastcall Save(TObject *Sender , TCloseAction &Action);
    void __fastcall Mmove(TObject *Sender , TShiftState Shift , int X , int Y);
    void __fastcall HitEnter(TObject *Sender , WORD &Key , TShiftState Shift);
    void __fastcall Mup(TObject *Sender , TMouseButton Button , TShiftState Shift , int X , int Y);
    void __fastcall Mdown(TObject *Sender , TMouseButton Button , TShiftState Shift , int X , int Y);
    void __fastcall Hitkey(TObject *Sender , WORD &Key , TShiftState Shift);

   private:

    bool bgs;
    int grid[50][50];
    BST_highscores hs[16];
    Graphics::TBitmap *bg, *temp;
    TColor maincolor , darkmaincolor , lightcolor , darkcolor , highcolor , warncolor;
    int lastx , lasty , curx , cury , gwidth , gheight , squares , bombcount , test , update;
    int diff , counter , flags , level , count , delay , lives , holdf , holdc , all , allcheck;

   //new
    bool Gb_rightmouseon , Gb_leftmouseon;
    BST_state Gstate_gamestate;

    void __fastcall SetupBlow();
    void __fastcall EraseHigh();
    void __fastcall ShowBombs();
    void __fastcall TestHigh(int time);
    void __fastcall Calc(int x , int y);
    void __fastcall Cover(int x , int y);
    void __fastcall UnCover(int x , int y , int flag);

   public:

   __fastcall TMainForm(TComponent* Owner);

   friend class TScoreForm;
};

extern TMainForm *MainForm;

#endif
