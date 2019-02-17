#include <files.h>

#include <string.h>
#include <system.h>
#include <conio.h>

#define VPAB 0x3000
#define FBUF 0x3200

int force_quit=0;

unsigned char dsr_openDV(struct PAB* pab, char* fname, int vdpbuffer, unsigned char flags);
unsigned char dsr_close(struct PAB* pab);
unsigned char dsr_read(struct PAB* pab, int recordNumber);

void initPab(struct PAB* pab) {
  pab->OpCode = DSR_OPEN;
  pab->Status = DSR_TYPE_DISPLAY | DSR_TYPE_VARIABLE | DSR_TYPE_SEQUENTIAL | DSR_TYPE_INPUT;
  pab->RecordLength = 80;
  pab->RecordNumber = 0;
  pab->ScreenOffset = 0;
  pab->NameLength = 0;
  pab->CharCount = 0;
}

// Configures a PAB for filename and DV80, and opens the file
unsigned char dsr_openDV(struct PAB* pab, char* fname, int vdpbuffer, unsigned char flags) {
  initPab(pab);
  pab->OpCode = DSR_OPEN;
  pab->Status = DSR_TYPE_DISPLAY | DSR_TYPE_VARIABLE | DSR_TYPE_SEQUENTIAL | flags;
  pab->RecordLength = 80;
  pab->pName = fname;
  pab->VDPBuffer = vdpbuffer;

  return dsrlnk(pab, VPAB);
}

unsigned char dsr_close(struct PAB* pab) {
  pab->OpCode = DSR_CLOSE;

  return dsrlnk(pab, VPAB);
}

// the data read is in FBUF, the length read in pab->CharCount
// typically passing 0 in for record number will let the controller
// auto-increment it. 
unsigned char dsr_read(struct PAB* pab, int recordNumber) {
  pab->OpCode = DSR_READ;
  pab->RecordNumber = recordNumber;
  pab->CharCount = 0;

  unsigned char result = dsrlnk(pab, VPAB);
  vdpmemread(VPAB + 5, (&pab->CharCount), 1);
  return result;
}

void main(void)
{
  struct PAB pab;
  set_text();
  charsetlc();
  clrscr();
  bgcolor(COLOR_CYAN);
  textcolor(COLOR_BLACK);
  gotoxy(0,0);
  unsigned char ferr = dsr_openDV(&pab,"PI.HTTP://TI99.IRATA.ONLINE/COPYING.TXT",FBUF,DSR_TYPE_INPUT);
  if (ferr)
    {
      cprintf("Could not open License from web.");
      for (;;) {}
    }
  int i=0;
  unsigned char ch;
  while (ferr == DSR_ERR_NONE)
    {
      unsigned char cbuf[81];
      ferr = dsr_read(&pab,0);

      if (ferr == DSR_ERR_NONE)
	{
	  vdpmemread(FBUF,cbuf,pab.CharCount);
	  cbuf[pab.CharCount]=0;
	  cprintf("%s",cbuf);
	}

      if (i>4)
	{
	  i=0;
	  cprintf("                                       \r\n");
	  cprintf("  -- PRESS ANY KEY TO CONTINUE -- ");
	  ch=cgetc();
	  clrscr();
	}
      else
	{
	  i++; // Get next record.
	}
    }
  
  cprintf("                                        \r\n");
  cprintf("  END OF LICENSE. PRESS ANY KEY TO QUIT. ");
  ch=cgetc();

  ferr = dsr_close(&pab);
}
