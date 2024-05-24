/*** includes ***/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>

/*** data ***/

struct termios original_termios;

/*** terminal ***/

// error handling, use with library calls
void die(const char *s)
{
  perror(s);
  exit(1);
}

// set terminal settings to original at program start
void disableRawMode()
{
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios) == -1)
  {
    die("tcsetattr");
  }
}

// edit terminal settings
void enableRawMode()
{
  // get terminal settings at program start and write to "original_termios"
  if (tcgetattr(STDIN_FILENO, &original_termios) == -1)
  {
    die("tcgetattr");
  }
  atexit(disableRawMode);

  struct termios raw = original_termios;
  
  // update various terminal flags
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= ~(CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  // set "read" to timeout after 100ms & return after 1 byte recieved
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
  {
    die("tcsetattr");
  }
}

/*** init ***/

int main()
{
  enableRawMode();

  while (1)
  {
    char c = '\0';

    // read 1 byte from stdin into "c" until all bytes have been read
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
    {
      die("read");
    }
    // display keypresses
    if (iscntrl(c))
    {
      printf("%d\r\n", c);
    }
    else
    {
      printf("%d ('%c')\r\n", c, c);
    }
    // quit program
    if (c == 'q')
      break;
  }

  return 0;
}